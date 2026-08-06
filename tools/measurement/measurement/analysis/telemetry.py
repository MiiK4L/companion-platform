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
STREAM_VERSION = 4
NO_SEQ = 0xFFFFFFFF
_SEQ_MOD = 1 << 32
_SEQ_HALF = 1 << 31

#: Largeurs de compteur de ticks acceptees (validation STRICTE).
SUPPORTED_TICK_WIDTHS = (8, 16, 32, 64)
#: Longueur maximale d'une chaine d'en-tete (miroir de BENCH_TELEMETRY_STR_MAX).
STR_MAX = 48
#: Nombre maximal de classes d'histogramme (miroir de BENCH_HISTOGRAM_MAX_BINS).
HISTOGRAM_MAX_BINS = 16

_SAMPLE_WIRE_SIZE = 44
_GAP_MAX_PRODUCERS = 4
_GAP_BODY = 1 + 4 + 4 + 1 + 4 * _GAP_MAX_PRODUCERS
_PRODUCER_SUMMARY_BODY = 1 + 16 * 4 + 2 * 8
_FOOTER_BODY = 1 + 5 * 4
_HEADER_FIXED = 1 + 2 + 1 + 1 + 1 + 8 + 4 + 1 + 2 + 1 + 1 + 1 + 8
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
        "topology": "spi-separated" if p[21] else "spi-shared",
        "arb_policy": p[22],
        "producer_count": p[23],
        "starvation_threshold_ticks": _u64(p, 24),
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
        "producer_id": p[1],
        # Les DEUX numerotations sont transportees : aucune n'est reconstruite.
        "producer_sequence_id": _u32(p, 2),
        "global_event_seq": _u32(p, 6),
        # Les QUATRE instants de bus, en ticks bruts.
        "t_request": _u64(p, 10),
        "t_grant": _u64(p, 18),
        "t_release": _u64(p, 26),
        "t_end": _u64(p, 34),
        "status": p[42],
        "timeout_cause": p[43],
        "flags": p[44],
    }


def _parse_gap(p: bytes) -> dict[str, Any]:
    _need(p, 1 + 4 + 4 + 1, "gap")
    n = p[9]
    if n > _GAP_MAX_PRODUCERS:
        raise TelemetryError(f"gap : producer_count hors bornes ({n})")
    _exact(p, 1 + 4 + 4 + 1 + 4 * n, "gap")
    return {
        "lost_count": _u32(p, 1),
        "after_global_seq": _u32(p, 5),
        # Une perte dans le flux global n'efface PAS l'identite du producteur.
        "lost_by_producer": [_u32(p, 10 + 4 * i) for i in range(n)],
    }


def _parse_summary(p: bytes) -> dict[str, Any]:
    _need(p, 2, "summary")
    n = p[1]
    if n > 4:
        raise TelemetryError(f"summary : producer_count hors bornes ({n})")
    expected = 2 + n * _PRODUCER_SUMMARY_BODY + 4 + 4 + 8
    _exact(p, expected, "summary")

    keys = (
        "issued",
        "ok",
        "timeout",
        "rejected",
        "unpaired",
        "duplicate",
        "out_of_order",
        "producer_drop",
        "timeout_bus_wait",
        "timeout_peripheral_response",
        "timeout_transport",
        "timeout_scheduler",
        "queue_overflow_count",
        "requests_over_starvation_threshold",
        "max_queue_depth",
        "_reserved",
    )
    per: list[dict[str, int]] = []
    off = 2
    for _ in range(n):
        entry: dict[str, int] = {"producer_id": p[off]}
        off += 1
        for k in keys:
            entry[k] = _u32(p, off)
            off += 4
        entry.pop("_reserved", None)
        entry["max_bus_wait_ticks"] = _u64(p, off)
        off += 8
        entry["oldest_pending_age_ticks"] = _u64(p, off)
        off += 8
        per.append(entry)

    out: dict[str, Any] = {
        "producer_count": n,
        "per_producer": per,
        "gap_records_merged": _u32(p, off),
        "gap_capacity": _u32(p, off + 4),
        "timeout_budget_ticks": _u64(p, off + 8),
    }
    # Agregat GLOBAL, derive de la ventilation : la reconciliation doit se
    # fermer globalement ET pour chaque producteur pris isolement.
    for k in (
        "issued",
        "ok",
        "timeout",
        "rejected",
        "unpaired",
        "duplicate",
        "out_of_order",
        "producer_drop",
    ):
        out[k] = sum(e[k] for e in per)
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

    # --- MACHINE D'ETAT du flux (ordre UNIQUE, documente et teste) -----------
    #   HEADER -> (SAMPLE | GAP)* -> SUMMARY -> HISTOGRAM? -> FOOTER
    # Toute transition non prevue est une erreur de STRUCTURE, jamais une
    # tolerance silencieuse.
    ST_INIT, ST_BODY, ST_SUMMARY, ST_HISTOGRAM, ST_END = range(5)
    _ALLOWED = {
        ST_INIT: {TM_HEADER},
        ST_BODY: {TM_SAMPLE, TM_GAP, TM_SUMMARY},
        ST_SUMMARY: {TM_HISTOGRAM, TM_FOOTER},
        ST_HISTOGRAM: {TM_FOOTER},
        ST_END: set(),
    }
    _NAMES = {
        TM_HEADER: "header",
        TM_SAMPLE: "sample",
        TM_GAP: "gap",
        TM_SUMMARY: "summary",
        TM_HISTOGRAM: "histogram",
        TM_FOOTER: "footer",
    }

    state = ST_INIT
    header: dict[str, Any] | None = None
    summary: dict[str, int] | None = None
    footer: dict[str, int] | None = None
    footer_frame_seq: int | None = None
    histogram: dict[str, Any] | None = None
    samples: list[dict[str, Any]] = []
    gaps: list[dict[str, int]] = []

    for f in frames:
        p = f["payload"]
        if not p:
            raise TelemetryError("message vide")
        kind = p[0]
        if kind not in _NAMES:
            # Version 3 : un type inconnu est REJETE. Il n'existe pas de zone
            # d'extension ; le tolerer laisserait un flux inconnu passer pour
            # eligible.
            raise TelemetryError(f"type de message inconnu : {kind}")
        if kind not in _ALLOWED[state]:
            raise TelemetryError(f"{_NAMES[kind]} : transition interdite (etat {state})")

        if kind == TM_HEADER:
            header = _parse_header(p)
            state = ST_BODY
        elif kind == TM_SAMPLE:
            samples.append(_parse_sample(p))
        elif kind == TM_GAP:
            gaps.append(_parse_gap(p))
        elif kind == TM_SUMMARY:
            summary = _parse_summary(p)
            state = ST_SUMMARY
        elif kind == TM_HISTOGRAM:
            if header is not None and not header["histogram_enabled"]:
                raise TelemetryError("histogram present alors qu'il n'est pas annonce")
            histogram = _parse_histogram(p)
            state = ST_HISTOGRAM
        else:  # TM_FOOTER
            footer = _parse_footer(p)
            footer_frame_seq = f["seq"]
            state = ST_END

    if header is None:
        raise TelemetryError("header absent : flux inexploitable")

    seqs = [f["seq"] for f in frames]
    last_expected = footer["last_stream_seq"] if footer else None
    sequence = analyse_sequence(seqs, last_expected)
    transport_gap = sequence["missing_total"]

    # --- RECONCILIATION DE FLUX (convention des compteurs, archivee) ---------
    # Les compteurs du footer sont captures AVANT son emission ; le "+1" des
    # relations ci-dessous correspond donc au footer lui-meme.
    unique_frames = len(set(seqs))
    checks: dict[str, bool] = {}
    if footer is not None:
        expected_frames = (footer["last_stream_seq"] + 1) % _SEQ_MOD
        if expected_frames == 0:
            expected_frames = _SEQ_MOD
        checks["first_sequence_is_zero"] = seqs[0] == 0
        checks["footer_frame_seq_matches"] = footer_frame_seq == footer["last_stream_seq"]
        checks["frames_attempted_matches_last_seq"] = (
            footer["frames_attempted"] == expected_frames
        )
        checks["frames_accounted"] = (
            footer["frames_accepted"] + footer["frames_refused"] + 1
            == footer["frames_attempted"]
        )
        checks["decoded_plus_gap_matches_attempted"] = (
            unique_frames + transport_gap == footer["frames_attempted"]
        )
        if summary is not None:
            checks["samples_attempted_matches_issued"] = (
                footer["samples_attempted"] == summary["issued"]
            )
    footer_consistent = bool(checks) and all(checks.values())

    # Une lacune fusionnee (ou l'absence de place declaree) rend la LOCALISATION
    # des pertes incomplete, meme si le TOTAL reste exact.
    if summary is not None:
        merged = summary.get("gap_records_merged", 0)
        gap_capacity = summary.get("gap_capacity", 0)
        producer_drop = summary.get("producer_drop", 0)
        gap_localization_complete = merged == 0 and not (
            gap_capacity == 0 and producer_drop > 0
        )
    else:
        gap_localization_complete = False

    # Un histogramme ANNONCE ne peut pas disparaitre silencieusement.
    histogram_required = bool(header["histogram_enabled"])
    histogram_present = histogram is not None

    stream_completeness = (
        "complete"
        if (footer is not None and footer_consistent and transport_gap == 0)
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
        "frames_unique": unique_frames,
        "frames_resync": resyncs,
        "sequence": sequence,
        "transport_gap": transport_gap,
        "summary_present": summary is not None,
        "footer_present": footer is not None,
        "footer_consistent": footer_consistent,
        "footer_checks": checks,
        "gap_localization_complete": gap_localization_complete,
        "histogram_required": histogram_required,
        "histogram_present": histogram_present,
        "stream_completeness": stream_completeness,
        "producer_drop_from_gaps": sum(g["lost_count"] for g in gaps),
    }


CSV_COLUMNS = (
    "producer_id",
    "producer_sequence_id",
    "global_event_seq",
    "t_request_ticks",
    "t_grant_ticks",
    "t_release_ticks",
    "t_end_ticks",
    "latency_ticks",
    "bus_wait_ticks",
    "bus_hold_ticks",
    "status",
    "timeout_cause",
    "variant",
    "mode",
)

TIMEOUT_CAUSES = {
    0: "none",
    1: "bus_wait",
    2: "peripheral_response",
    3: "transport",
    4: "scheduler",
}


def bus_wait_ticks(sample: dict[str, Any], width_bits: int = 64) -> int:
    """Invariant : bus_wait_ticks = t_grant - t_request (wrap-safe)."""
    from .stats import elapsed_wrap_safe

    return elapsed_wrap_safe(sample["t_request"], sample["t_grant"], width_bits)


def bus_hold_ticks(sample: dict[str, Any], width_bits: int = 64) -> int:
    """Invariant : bus_hold_ticks = t_release - t_grant (wrap-safe)."""
    from .stats import elapsed_wrap_safe

    return elapsed_wrap_safe(sample["t_grant"], sample["t_release"], width_bits)


def to_series_rows(parsed: dict[str, Any]) -> list[list[Any]]:
    """Construit les lignes de la vue normalisee depuis le flux decode."""
    from .stats import elapsed_wrap_safe

    header = parsed.get("header") or {}
    width = header.get("tick_width_bits", 64)
    variant = header.get("variant", "")
    mode = header.get("mode", "")
    return [
        [
            s["producer_id"],
            s["producer_sequence_id"],
            s["global_event_seq"],
            s["t_request"],
            s["t_grant"],
            s["t_release"],
            s["t_end"],
            elapsed_wrap_safe(s["t_request"], s["t_end"], width),
            bus_wait_ticks(s, width),
            bus_hold_ticks(s, width),
            STATUS_NAMES.get(s["status"], f"unknown_{s['status']}"),
            TIMEOUT_CAUSES.get(s["timeout_cause"], f"unknown_{s['timeout_cause']}"),
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
        elapsed_wrap_safe(s["t_request"], s["t_end"], width)
        for s in parsed["samples"]
        if s["status"] == 0
    ]


def per_producer_analysis(parsed: dict[str, Any]) -> dict[int, dict[str, Any]]:
    """Statistiques PAR PRODUCTEUR, avec verification des invariants de bus.

    La reconciliation doit se fermer pour CHAQUE producteur pris isolement, pas
    seulement globalement."""
    from . import stats as st

    width = (parsed.get("header") or {}).get("tick_width_bits", 64)
    summary = parsed.get("summary") or {}
    by_producer: dict[int, dict[str, Any]] = {}

    for entry in summary.get("per_producer", []):
        pid = entry["producer_id"]
        samples = [s for s in parsed["samples"] if s["producer_id"] == pid]
        latencies = [
            st.elapsed_wrap_safe(s["t_request"], s["t_end"], width)
            for s in samples
            if s["status"] == 0
        ]
        waits = [bus_wait_ticks(s, width) for s in samples]
        # Sequence LOCALE continue : verifiee sans jamais la deduire de l'ordre
        # global, et inversement.
        local = [s["producer_sequence_id"] for s in samples]
        by_producer[pid] = {
            "counts": entry,
            "reconciliation": st.reconcile(entry),
            "latency": st.latency_stats(latencies),
            "bus_wait_total": sum(waits),
            "bus_wait_max": max(waits) if waits else 0,
            "local_sequence_contiguous": local == sorted(local),
            "recorded_sample_count": len(samples),
        }
    return by_producer


def check_bus_invariants(parsed: dict[str, Any]) -> dict[str, Any]:
    """Verifie les invariants d'instants de bus sur chaque enregistrement."""
    width = (parsed.get("header") or {}).get("tick_width_bits", 64)
    violations: list[dict[str, Any]] = []
    for s in parsed["samples"]:
        wait = bus_wait_ticks(s, width)
        hold = bus_hold_ticks(s, width)
        if wait > (1 << (width - 1)) or hold > (1 << (width - 1)):
            # Un ecart superieur a la demi-plage revele un ordre incoherent
            # (t_grant avant t_request, ou t_release avant t_grant).
            violations.append({"global_event_seq": s["global_event_seq"]})
    topology = (parsed.get("header") or {}).get("topology")
    zero_wait_expected = topology == "spi-separated"
    nonzero = [s["global_event_seq"] for s in parsed["samples"] if bus_wait_ticks(s, width)]
    return {
        "ordering_violations": violations,
        "topology": topology,
        # En topologie separee, aucune attente n'est possible hors faute injectee.
        "separated_zero_bus_wait": (not zero_wait_expected) or not nonzero,
        "samples_with_bus_wait": nonzero,
    }


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
    reconciliation = st.reconcile(summary)

    # --- ELIGIBILITE AU VERDICT : conjonction EXPLICITE -----------------------
    # Une serie n'est pas qualifiee complete au seul motif qu'aucune trame ne
    # semble manquer : sans bilan reconcilie, on ignore si TOUTES les
    # transactions attendues sont representees.
    hist_required = bool(parsed.get("histogram_required", False))
    hist_present = bool(parsed.get("histogram_present", False))
    device_hist = parsed.get("device_histogram")
    hist_saturated = bool((device_hist or {}).get("saturated", False))
    # Un histogramme ANNONCE ne peut pas disparaitre silencieusement, et un
    # histogramme SATURE n'est plus reconciliable : les deux retirent l'usage.
    hist_usable = (not hist_saturated) and (not hist_required or hist_present)

    conditions = {
        "summary_present": bool(parsed.get("summary_present", False)),
        "reconciliation_balanced": bool(reconciliation["balanced"]),
        "summary_footer_consistent": bool(parsed.get("footer_consistent", False)),
        "series_complete": series_completeness == "complete",
        "stream_complete": stream_completeness == "complete",
        "gap_localization_complete": bool(parsed.get("gap_localization_complete", False)),
        "histogram_usable_if_required": hist_usable,
    }
    eligible = all(conditions.values())
    blocking = sorted(k for k, v in conditions.items() if not v)

    block: dict[str, Any] = {
        "latency": st.latency_stats(latencies),
        "reconciliation": reconciliation,
        "losses": {
            # Les deux natures de perte ne sont JAMAIS fusionnees.
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
        "summary_present": conditions["summary_present"],
        "footer_present": bool(parsed.get("footer_present", False)),
        "footer_checks": parsed.get("footer_checks", {}),
        "gap_localization_complete": conditions["gap_localization_complete"],
        "histogram_required": hist_required,
        "histogram_present": hist_present,
        "sequence": parsed.get("sequence", {}),
        "eligibility": {"conditions": conditions, "blocking": blocking},
        "quantiles_verdict_eligible": eligible,
        "invalid_sample_count": (
            int(summary.get("issued", 0)) - int(summary.get("ok", 0)) if summary else None
        ),
    }

    if edges is None and device_hist is not None:
        edges = list(device_hist["bin_edges"])
    if edges:
        tooling = hist.derive(latencies, edges)
        block["histogram"] = {
            "tooling": tooling,
            "device": device_hist,
            "comparison": hist.compare(device_hist, tooling),
        }
    return block
