# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Parseur du FLUX de telemetrie produit par le firmware de banc.

Le flux est une suite de trames B2 (magic/version/seq/len/payload/crc32). Le
champ ``seq`` de la trame est un numero MONOTONE ; il est analyse DANS L'ORDRE
DE RECEPTION, avec des etats distincts (attendu, lacune, doublon, hors ordre),
en comparaison MODULAIRE sur 32 bits.

Le flux se termine par une CLOTURE (footer) qui declare le dernier numero de
sequence et les compteurs de trames. Sans clôture valide, une perte des
DERNIERES trames serait indetectable : la capture est alors declaree
``incomplete``, jamais complete par defaut.

Deux pertes, JAMAIS fusionnees :
  ``producer_drop`` : echantillon perdu AVANT serialisation (tampon sature) ;
  ``transport_gap`` : trame serialisee mais absente ou rejetee cote capture.

Le parseur est strictement BORNE : toute trame de longueur incoherente, de
chaine invalide ou de structure inattendue produit une erreur CONTROLEE
(``TelemetryError``), jamais une exception Python accidentelle."""

import zlib
from typing import Any

_MAGIC0 = 0xC5
_MAGIC1 = 0xB5
_FRAME_VERSION = 1
_HEADER_SIZE = 9
_CRC_SIZE = 4
_OVERHEAD = _HEADER_SIZE + _CRC_SIZE
_MAX_PAYLOAD = 256

TM_HEADER = 1
TM_SAMPLE = 2
TM_GAP = 3
TM_SUMMARY = 4
TM_HISTOGRAM = 5
TM_FOOTER = 6

#: Version du FORMAT de flux acceptee (miroir de BENCH_TELEMETRY_STREAM_VERSION).
STREAM_VERSION = 2
NO_SEQ = 0xFFFFFFFF
_SEQ_MOD = 1 << 32
_SEQ_HALF = 1 << 31

#: Largeurs de compteur de ticks acceptees (validation STRICTE).
SUPPORTED_TICK_WIDTHS = (8, 16, 32, 64)
#: Longueur maximale d'une chaine d'en-tete (miroir de BENCH_TELEMETRY_STR_MAX).
STR_MAX = 48
#: Nombre maximal de classes d'histogramme (miroir de BENCH_HISTOGRAM_MAX_BINS).
HISTOGRAM_MAX_BINS = 16

_SAMPLE_WIRE_SIZE = 22
_GAP_BODY = 1 + 8
_SUMMARY_BODY = 1 + 8 * 4 + 8
_FOOTER_BODY = 1 + 5 * 4
_HEADER_FIXED = 1 + 2 + 1 + 1 + 1 + 8 + 4 + 1 + 2
_HISTOGRAM_FIXED = 1 + 5 * 4 + 1

STATUS_NAMES = {
    0: "ok",
    1: "timeout",
    2: "rejected",
    3: "unpaired",
    4: "duplicate",
    5: "out_of_order",
}


class TelemetryError(Exception):
    """Flux structurellement invalide (et non simplement lacunaire)."""


def _u16(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 2], "big")


def _u32(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 4], "big")


def _u64(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 8], "big")


def _need(payload: bytes, size: int, what: str) -> None:
    if len(payload) < size:
        raise TelemetryError(f"{what} : payload trop court ({len(payload)} < {size})")


def _exact(payload: bytes, size: int, what: str) -> None:
    if len(payload) != size:
        raise TelemetryError(f"{what} : longueur inattendue ({len(payload)} != {size})")


def _decode_frame(data: bytes, off: int) -> tuple[dict[str, Any] | None, int]:
    """Decode la trame a ``off``. Retourne (trame|None, offset_suivant)."""
    if off + _OVERHEAD > len(data):
        return None, len(data)
    if data[off] != _MAGIC0 or data[off + 1] != _MAGIC1:
        return None, off + 1
    if data[off + 2] != _FRAME_VERSION:
        return None, off + 1
    payload_len = _u16(data, off + 7)
    if payload_len > _MAX_PAYLOAD:
        return None, off + 1
    total = _OVERHEAD + payload_len
    if off + total > len(data):
        return None, len(data)
    body_end = off + _HEADER_SIZE + payload_len
    if zlib.crc32(data[off:body_end]) & 0xFFFFFFFF != _u32(data, body_end):
        return None, off + 1
    return (
        {"seq": _u32(data, off + 3), "payload": data[off + _HEADER_SIZE : body_end]},
        off + total,
    )


def _parse_header(p: bytes) -> dict[str, Any]:
    _need(p, _HEADER_FIXED, "header")
    stream_version = _u16(p, 1)
    if stream_version != STREAM_VERSION:
        raise TelemetryError(f"version de flux non supportee : {stream_version}")
    tick_width_bits = p[4]
    if tick_width_bits not in SUPPORTED_TICK_WIDTHS:
        raise TelemetryError(f"largeur de tick non supportee : {tick_width_bits}")
    out: dict[str, Any] = {
        "stream_version": stream_version,
        "clock_id": p[3],
        "tick_width_bits": tick_width_bits,
        "wrap_policy": p[5],
        "tick_hz": _u64(p, 6),
        "ring_capacity": _u32(p, 14),
        "histogram_enabled": bool(p[18]),
        "histogram_version": _u16(p, 19),
    }

    off = _HEADER_FIXED
    for field in ("profile_id", "variant", "mode"):
        if off >= len(p):
            raise TelemetryError(f"header : chaine '{field}' absente")
        n = p[off]
        off += 1
        if n > STR_MAX:
            raise TelemetryError(f"header : chaine '{field}' trop longue ({n})")
        if off + n > len(p):
            raise TelemetryError(f"header : chaine '{field}' tronquee")
        raw = p[off : off + n]
        try:
            value = raw.decode("ascii")
        except UnicodeDecodeError as exc:
            raise TelemetryError(f"header : chaine '{field}' non ASCII") from exc
        if any(c < 0x20 or c > 0x7E for c in raw):
            raise TelemetryError(f"header : chaine '{field}' non imprimable")
        out[field] = value
        off += n
    if off != len(p):
        raise TelemetryError("header : octets excedentaires")
    # tick_hz = 0 : aucune conversion physique n'est possible.
    out["physical_time_available"] = out["tick_hz"] > 0
    return out


def _parse_sample(p: bytes) -> dict[str, int]:
    _exact(p, 1 + _SAMPLE_WIRE_SIZE, "sample")
    return {
        "sequence_id": _u32(p, 1),
        "t_start": _u64(p, 5),
        "t_end": _u64(p, 13),
        "status": p[21],
        "flags": p[22],
    }


def _parse_gap(p: bytes) -> dict[str, int]:
    _exact(p, _GAP_BODY, "gap")
    return {"lost_count": _u32(p, 1), "after_sequence_id": _u32(p, 5)}


def _parse_summary(p: bytes) -> dict[str, int]:
    _exact(p, _SUMMARY_BODY, "summary")
    keys = (
        "issued",
        "ok",
        "timeout",
        "rejected",
        "unpaired",
        "duplicate",
        "out_of_order",
        "producer_drop",
    )
    out = {k: _u32(p, 1 + 4 * i) for i, k in enumerate(keys)}
    out["timeout_budget_ticks"] = _u64(p, 1 + 4 * len(keys))
    return out


def _parse_footer(p: bytes) -> dict[str, int]:
    _exact(p, _FOOTER_BODY, "footer")
    keys = (
        "last_stream_seq",
        "frames_attempted",
        "frames_accepted",
        "frames_refused",
        "samples_attempted",
    )
    return {k: _u32(p, 1 + 4 * i) for i, k in enumerate(keys)}


def _parse_histogram(p: bytes) -> dict[str, Any]:
    _need(p, _HISTOGRAM_FIXED, "histogram")
    bin_count = _u32(p, 5)
    if bin_count == 0 or bin_count > HISTOGRAM_MAX_BINS:
        raise TelemetryError(f"histogram : bin_count hors bornes ({bin_count})")
    expected = _HISTOGRAM_FIXED + (bin_count + 1) * 8 + bin_count * 4
    _exact(p, expected, "histogram")
    off = _HISTOGRAM_FIXED
    edges = [_u64(p, off + 8 * i) for i in range(bin_count + 1)]
    off += 8 * (bin_count + 1)
    counts = [_u32(p, off + 4 * i) for i in range(bin_count)]
    for i in range(bin_count):
        if not edges[i] < edges[i + 1]:
            raise TelemetryError("histogram : bornes non strictement croissantes")
    return {
        "version": _u32(p, 1),
        "bin_edges": edges,
        "bin_counts": counts,
        "underflow": _u32(p, 9),
        "overflow": _u32(p, 13),
        "sample_count": _u32(p, 17),
        "saturated": bool(p[21]),
    }


def analyse_sequence(seqs: list[int], last_expected: int | None) -> dict[str, int]:
    """Analyse les numeros de sequence DANS L'ORDRE DE RECEPTION.

    Comparaison MODULAIRE sur 32 bits : un ecart superieur a 2^31 est interprete
    comme un retour en arriere (hors ordre), pas comme une lacune geante. Le
    nombre attendu de trames vient de la CLOTURE ; sans elle, il ne peut pas
    etre etabli et la capture est incomplete."""
    states = {"expected": 0, "gap": 0, "duplicate": 0, "out_of_order": 0, "wrap": 0}
    missing = 0
    seen: set[int] = set()
    previous: int | None = None
    for seq in seqs:
        if seq in seen:
            states["duplicate"] += 1
            continue
        seen.add(seq)
        if previous is None:
            states["expected"] += 1
            previous = seq
            continue
        delta = (seq - previous) % _SEQ_MOD
        if delta == 1:
            states["expected"] += 1
        elif delta >= _SEQ_HALF:
            states["out_of_order"] += 1
        else:
            states["gap"] += 1
            missing += delta - 1
        if seq < previous:
            states["wrap"] += 1
        previous = seq

    trailing = 0
    if last_expected is not None and previous is not None:
        trailing = (last_expected - previous) % _SEQ_MOD
        if trailing >= _SEQ_HALF:
            trailing = 0  # cloture anterieure a la derniere trame recue
    states["missing_total"] = missing + trailing
    states["trailing_missing"] = trailing
    return states


def parse_stream(data: bytes) -> dict[str, Any]:
    """Decode un flux complet. Leve ``TelemetryError`` si structurellement invalide."""
    frames: list[dict[str, Any]] = []
    off = 0
    resyncs = 0
    while off < len(data):
        frame, nxt = _decode_frame(data, off)
        if frame is None:
            if nxt == off + 1:
                resyncs += 1
            off = nxt
            continue
        frames.append(frame)
        off = nxt

    if not frames:
        raise TelemetryError("aucune trame valide dans le flux")

    header: dict[str, Any] | None = None
    summary: dict[str, int] | None = None
    footer: dict[str, int] | None = None
    histogram: dict[str, Any] | None = None
    samples: list[dict[str, Any]] = []
    gaps: list[dict[str, int]] = []
    unknown_messages = 0

    for index, f in enumerate(frames):
        p = f["payload"]
        if not p:
            raise TelemetryError("message vide")
        kind = p[0]
        if kind == TM_HEADER:
            if index != 0:
                raise TelemetryError("header : doit etre le PREMIER message")
            if header is not None:
                raise TelemetryError("header : duplique")
            header = _parse_header(p)
        elif kind == TM_SAMPLE:
            if summary is not None:
                raise TelemetryError("sample apres le bilan")
            samples.append(_parse_sample(p))
        elif kind == TM_GAP:
            gaps.append(_parse_gap(p))
        elif kind == TM_SUMMARY:
            if summary is not None:
                raise TelemetryError("summary : duplique")
            summary = _parse_summary(p)
        elif kind == TM_HISTOGRAM:
            if histogram is not None:
                raise TelemetryError("histogram : duplique")
            if header is not None and not header["histogram_enabled"]:
                raise TelemetryError("histogram present alors qu'il n'est pas annonce")
            histogram = _parse_histogram(p)
        elif kind == TM_FOOTER:
            if footer is not None:
                raise TelemetryError("footer : duplique")
            footer = _parse_footer(p)
        else:
            unknown_messages += 1  # compte explicitement, ne devine rien

    if header is None:
        raise TelemetryError("header absent : flux inexploitable")
    if header["histogram_enabled"] and histogram is None:
        # Annonce mais absent : ce n'est pas fatal, c'est une lacune de capture.
        pass

    seqs = [f["seq"] for f in frames]
    last_expected = footer["last_stream_seq"] if footer else None
    sequence = analyse_sequence(seqs, last_expected)

    # Sans cloture valide, l'absence des DERNIERES trames est indetectable :
    # la capture ne peut pas etre declaree complete.
    footer_valid = footer is not None
    if footer_valid:
        expected_frames = (footer["last_stream_seq"] + 1) % _SEQ_MOD
        if expected_frames == 0:
            expected_frames = _SEQ_MOD
        transport_gap = sequence["missing_total"]
        footer_consistent = footer["frames_attempted"] == expected_frames
    else:
        transport_gap = sequence["missing_total"]
        footer_consistent = False

    stream_completeness = (
        "complete"
        if (footer_valid and footer_consistent and transport_gap == 0)
        else "incomplete"
    )

    return {
        "header": header,
        "samples": samples,
        "gaps": gaps,
        "summary": summary,
        "footer": footer,
        "device_histogram": histogram,
        "frames_decoded": len(frames),
        "frames_resync": resyncs,
        "unknown_messages": unknown_messages,
        "sequence": sequence,
        "transport_gap": transport_gap,
        "footer_present": footer_valid,
        "footer_consistent": footer_consistent,
        "stream_completeness": stream_completeness,
        "producer_drop_from_gaps": sum(g["lost_count"] for g in gaps),
    }


CSV_COLUMNS = (
    "sequence_id",
    "t_start_ticks",
    "t_end_ticks",
    "latency_ticks",
    "status",
    "variant",
    "mode",
)


def to_series_rows(parsed: dict[str, Any]) -> list[list[Any]]:
    """Construit les lignes de la vue normalisee depuis le flux decode."""
    from .stats import elapsed_wrap_safe

    header = parsed.get("header") or {}
    width = header.get("tick_width_bits", 64)
    variant = header.get("variant", "")
    mode = header.get("mode", "")
    return [
        [
            s["sequence_id"],
            s["t_start"],
            s["t_end"],
            elapsed_wrap_safe(s["t_start"], s["t_end"], width),
            STATUS_NAMES.get(s["status"], f"unknown_{s['status']}"),
            variant,
            mode,
        ]
        for s in parsed["samples"]
    ]


def valid_latencies(parsed: dict[str, Any]) -> list[int]:
    """Latences de la population PRINCIPALE : statut ``ok`` uniquement."""
    from .stats import elapsed_wrap_safe

    width = (parsed.get("header") or {}).get("tick_width_bits", 64)
    return [
        elapsed_wrap_safe(s["t_start"], s["t_end"], width)
        for s in parsed["samples"]
        if s["status"] == 0
    ]


def build_analysis(parsed: dict[str, Any], edges: list[int] | None = None) -> dict[str, Any]:
    """Assemble le bloc d'analyse : statistiques, reconciliation, completude.

    L'eligibilite des quantiles a un verdict est calculee ICI, pas laissee a
    l'appreciation du lecteur : une serie lacunaire, une cloture manquante ou un
    histogramme sature retirent cette eligibilite."""
    from . import histogram as hist
    from . import stats as st

    summary = parsed.get("summary") or {}
    header = parsed.get("header") or {}
    transport_gap = int(parsed.get("transport_gap", 0))
    producer_drop = int(summary.get("producer_drop", 0))
    stream_completeness = parsed.get("stream_completeness", "incomplete")

    latencies = valid_latencies(parsed)
    series_completeness = st.series_completeness(producer_drop, transport_gap)
    eligible = series_completeness == "complete" and stream_completeness == "complete"

    block: dict[str, Any] = {
        "latency": st.latency_stats(latencies),
        "reconciliation": st.reconcile(summary),
        "losses": {
            "producer_drop": producer_drop,
            "transport_gap": transport_gap,
            "gap_markers": parsed.get("gaps", []),
        },
        "timeouts": {
            "timeout_count": int(summary.get("timeout", 0)),
            "timeout_ratio": (
                int(summary.get("timeout", 0)) / int(summary["issued"])
                if int(summary.get("issued", 0)) > 0
                else None
            ),
            "timeout_budget_ticks": int(summary.get("timeout_budget_ticks", 0)),
        },
        "clock": {
            "tick_width_bits": header.get("tick_width_bits"),
            "tick_hz": header.get("tick_hz"),
            # tick_hz = 0 : aucune conversion en secondes/microsecondes possible,
            # donc aucun verdict exprime en unites physiques.
            "physical_time_available": bool(header.get("physical_time_available", False)),
        },
        "series_completeness": series_completeness,
        "stream_completeness": stream_completeness,
        "footer_present": bool(parsed.get("footer_present", False)),
        "sequence": parsed.get("sequence", {}),
        "quantiles_verdict_eligible": eligible,
        "invalid_sample_count": (
            int(summary.get("issued", 0)) - int(summary.get("ok", 0)) if summary else None
        ),
    }

    device_hist = parsed.get("device_histogram")
    if edges is None and device_hist is not None:
        edges = list(device_hist["bin_edges"])
    if edges:
        tooling = hist.derive(latencies, edges)
        comparison = hist.compare(device_hist, tooling)
        block["histogram"] = {
            "tooling": tooling,
            "device": device_hist,
            "comparison": comparison,
        }
        if device_hist is not None and device_hist.get("saturated"):
            # Un histogramme sature n'est plus reconciliable : il reste utile au
            # diagnostic, mais ne peut alimenter aucun verdict.
            block["quantiles_verdict_eligible"] = False
    return block
