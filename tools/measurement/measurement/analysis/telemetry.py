# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Parseur du FLUX de telemetrie produit par le firmware de banc.

Le flux est une suite de trames B2 (magic/version/seq/len/payload/crc32). Le
champ ``seq`` de la trame est un numero MONOTONE : une discontinuite cote
capture revele un ``transport_gap``.

Deux pertes, JAMAIS fusionnees :
  ``producer_drop`` : echantillon perdu AVANT serialisation (tampon sature) ;
                      annonce par un MARQUEUR DE LACUNE a sa position exacte ;
  ``transport_gap`` : trame serialisee mais absente ou rejetee cote capture ;
                      deduite des numeros de sequence manquants.

Le parseur ne fabrique rien : une trame illisible est comptee, jamais devinee."""

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

STREAM_VERSION = 1
NO_SEQ = 0xFFFFFFFF

# Statuts terminaux, miroir de bench_sample_status_t.
STATUS_NAMES = {
    0: "ok",
    1: "timeout",
    2: "rejected",
    3: "unpaired",
    4: "duplicate",
    5: "out_of_order",
}


class TelemetryError(Exception):
    """Flux inexploitable (et non simplement lacunaire)."""


def _u16(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 2], "big")


def _u32(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 4], "big")


def _u64(b: bytes, o: int) -> int:
    return int.from_bytes(b[o : o + 8], "big")


def _decode_frame(data: bytes, off: int) -> tuple[dict[str, Any] | None, int]:
    """Decode la trame a ``off``. Retourne (trame|None, offset_suivant)."""
    if off + _OVERHEAD > len(data):
        return None, len(data)
    if data[off] != _MAGIC0 or data[off + 1] != _MAGIC1:
        return None, off + 1  # resynchronisation octet par octet
    if data[off + 2] != _FRAME_VERSION:
        return None, off + 1
    payload_len = _u16(data, off + 7)
    if payload_len > _MAX_PAYLOAD:
        return None, off + 1
    total = _OVERHEAD + payload_len
    if off + total > len(data):
        return None, len(data)  # trame incomplete en fin de flux
    body_end = off + _HEADER_SIZE + payload_len
    computed = zlib.crc32(data[off:body_end]) & 0xFFFFFFFF
    received = _u32(data, body_end)
    if computed != received:
        return None, off + 1
    return (
        {
            "seq": _u32(data, off + 3),
            "payload": data[off + _HEADER_SIZE : body_end],
        },
        off + total,
    )


def _parse_header(p: bytes) -> dict[str, Any]:
    o = 1
    stream_version = _u16(p, o)
    o += 2
    clock_id = p[o]
    o += 1
    tick_width_bits = p[o]
    o += 1
    wrap_policy = p[o]
    o += 1
    tick_hz = _u64(p, o)
    o += 8
    ring_capacity = _u32(p, o)
    o += 4
    histogram_enabled = p[o]
    o += 1
    histogram_version = _u16(p, o)
    o += 2

    def _s(offset: int) -> tuple[str, int]:
        n = p[offset]
        return p[offset + 1 : offset + 1 + n].decode("ascii"), offset + 1 + n

    profile_id, o = _s(o)
    variant, o = _s(o)
    mode, o = _s(o)
    return {
        "stream_version": stream_version,
        "clock_id": clock_id,
        "tick_width_bits": tick_width_bits,
        "wrap_policy": wrap_policy,
        "tick_hz": tick_hz,
        "ring_capacity": ring_capacity,
        "histogram_enabled": bool(histogram_enabled),
        "histogram_version": histogram_version,
        "profile_id": profile_id,
        "variant": variant,
        "mode": mode,
    }


def _parse_summary(p: bytes) -> dict[str, int]:
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


def _parse_histogram(p: bytes) -> dict[str, Any]:
    version = _u32(p, 1)
    bin_count = _u32(p, 5)
    underflow = _u32(p, 9)
    overflow = _u32(p, 13)
    sample_count = _u32(p, 17)
    o = 21
    edges = [_u64(p, o + 8 * i) for i in range(bin_count + 1)]
    o += 8 * (bin_count + 1)
    counts = [_u32(p, o + 4 * i) for i in range(bin_count)]
    return {
        "version": version,
        "bin_edges": edges,
        "bin_counts": counts,
        "underflow": underflow,
        "overflow": overflow,
        "sample_count": sample_count,
    }


def parse_stream(data: bytes) -> dict[str, Any]:
    """Decode un flux complet. Ne leve que si le flux est totalement inexploitable."""
    frames: list[dict[str, Any]] = []
    off = 0
    resyncs = 0
    while off < len(data):
        frame, nxt = _decode_frame(data, off)
        if frame is None:
            if nxt > off + 1 or nxt >= len(data):
                off = nxt
                continue
            resyncs += 1
            off = nxt
            continue
        frames.append(frame)
        off = nxt

    if not frames:
        raise TelemetryError("aucune trame valide dans le flux")

    header: dict[str, Any] | None = None
    summary: dict[str, int] | None = None
    histogram: dict[str, Any] | None = None
    samples: list[dict[str, Any]] = []
    gaps: list[dict[str, int]] = []

    for f in frames:
        p = f["payload"]
        if not p:
            continue
        kind = p[0]
        if kind == TM_HEADER:
            header = _parse_header(p)
        elif kind == TM_SAMPLE:
            samples.append(
                {
                    "sequence_id": _u32(p, 1),
                    "t_start": _u64(p, 5),
                    "t_end": _u64(p, 13),
                    "status": p[21],
                    "flags": p[22],
                }
            )
        elif kind == TM_GAP:
            gaps.append({"lost_count": _u32(p, 1), "after_sequence_id": _u32(p, 5)})
        elif kind == TM_SUMMARY:
            summary = _parse_summary(p)
        elif kind == TM_HISTOGRAM:
            histogram = _parse_histogram(p)

    seqs = {f["seq"] for f in frames}
    max_seq = max(seqs)
    # Toute sequence manquante dans [0, max_seq] est une trame serialisee mais
    # absente ou rejetee cote capture : un transport_gap.
    transport_gap = (max_seq + 1) - len(seqs)

    return {
        "header": header,
        "samples": samples,
        "gaps": gaps,
        "summary": summary,
        "device_histogram": histogram,
        "frames_decoded": len(frames),
        "frames_resync": resyncs,
        "transport_gap": transport_gap,
        "producer_drop_from_gaps": sum(g["lost_count"] for g in gaps),
    }


# Colonnes de la vue NORMALISEE (CSV). La latence y est une colonne DERIVEE,
# recalculee wrap-safe : elle n'est jamais transportee sur le fil.
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
    variant = header.get("variant", "")
    mode = header.get("mode", "")
    rows: list[list[Any]] = []
    for s in parsed["samples"]:
        rows.append(
            [
                s["sequence_id"],
                s["t_start"],
                s["t_end"],
                elapsed_wrap_safe(s["t_start"], s["t_end"]),
                STATUS_NAMES.get(s["status"], f"unknown_{s['status']}"),
                variant,
                mode,
            ]
        )
    return rows


def build_analysis(parsed: dict[str, Any], edges: list[int] | None = None) -> dict[str, Any]:
    """Assemble le bloc d'analyse : statistiques, reconciliation, completude.

    L'eligibilite des quantiles a un verdict est calculee ICI, pas laissee a
    l'appreciation du lecteur : une serie lacunaire ne peut pas fonder un
    verdict sur P95/P99, car les pertes se concentrent sur les rafales et
    biaisent la queue de distribution dans le sens favorable."""
    from . import histogram as hist
    from . import stats as st

    summary = parsed.get("summary") or {}
    transport_gap = int(parsed.get("transport_gap", 0))
    producer_drop = int(summary.get("producer_drop", 0))

    latencies = valid_latencies(parsed)
    completeness = st.series_completeness(producer_drop, transport_gap)

    block: dict[str, Any] = {
        "latency": st.latency_stats(latencies),
        "reconciliation": st.reconcile(summary),
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
        "series_completeness": completeness,
        # Regle MECANIQUE : pas de verdict sur les quantiles d'une serie lacunaire.
        "quantiles_verdict_eligible": completeness == "complete",
        "invalid_sample_count": (
            int(summary.get("issued", 0)) - int(summary.get("ok", 0)) if summary else None
        ),
    }

    device_hist = parsed.get("device_histogram")
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


def valid_latencies(parsed: dict[str, Any]) -> list[int]:
    """Latences de la population PRINCIPALE : statut ``ok`` uniquement.

    Les timeouts, rejets, non apparies, doublons et hors-ordre en sont exclus ;
    ils restent conserves et publies separement."""
    from .stats import elapsed_wrap_safe

    return [
        elapsed_wrap_safe(s["t_start"], s["t_end"])
        for s in parsed["samples"]
        if s["status"] == 0
    ]
