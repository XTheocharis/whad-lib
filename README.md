WHAD C/C++ library for compatible firmwares
===========================================

This library provides a set of functions and classes that makes the communication
with a WHAD host easier.

Documentation
-------------

Coming soon (we promise), meanwhile the code is fully documented. You can also have a
look at our nRF52 compatible firmware called [ButteRFly](https://github.com/whad-team/butterfly) that uses it.

---

## What's different on the `clue` branch

This is the `XTheocharis/whad-lib` fork (branch `clue`) tracking `upstream/whad-team/whad-lib#main`. The branch adds the **Board domain C/C++ API** and a meaningful **transport-layer hardening pass** that was forced by the new domain's tight encoded-size budget (1019 bytes).

### Board domain C API (`inc/domains/board.h` + `src/domains/board.c`, NEW 160+1049L)
- **X-macro source of truth**: `WHAD_BOARD_MESSAGE_LIST(X)` enumerates 49 entries (28 requests + `command_result` + 20 responses/events). Expanded twice — `WHAD_BOARD_DECLARE_MESSAGE` declares prototypes, `WHAD_BOARD_DEFINE_MESSAGE` emits bodies — producing 98 `whad_board_<name>` / `whad_board_<name>_parse` functions in 8 lines of macro.
- **Two C enums** mapping 1:1 to nanopb constants: `whad_board_msgtype_t` (49 values), `whad_board_command_t` (28 values 0x00-0x1B).
- **28 hand-written validators** in `src/domains/board.c` enforce nanopb invariants at the trust boundary: enum ranges, byte-array sizes ≤ buffer capacity, repeated-field counts ≤ `max_count`, status-flag bitmasks, nested lease-token validity, span checks for streaming chunks. Every message that crosses the C API boundary is structurally validated before acceptance — a malformed/hostile peer cannot overrun fixed buffers in `butterfly`.
- Generic `whad_board_pack` / `whad_board_unpack` serializers with defense-in-depth re-validation.
- Dispatch helpers: `whad_board_get_message_type(Message*)` → tag enum; 28-arm `whad_board_command_from_message_type(msgtype, &cmd)` reverse-map.

### Board domain C++ API (`inc/cpp/board/base.hpp` + `src/cpp/domains/board/`, NEW 77+26L base)
Thin `BoardMsg : NanoPbMsg` wrapper plus **50 per-message C++ subclasses** in `src/cpp/domains/board/` (one .cpp per message; ble=41, phy=27, dot15d4=16, esb=12, unifying=13 for comparison). Downstream firmware (`butterfly`) uses the `BoardMsg` base + C API directly — the subclasses are library API surface for other consumers.

### Transport hardening (`src/transport.c`, `src/ringbuf.c`, `inc/transport.h`, `inc/ringbuf.h`)
Forced by the new 1019-byte transport budget that links the proto-encoded size to the embedded ring buffer:
- **Ring buffer off-by-one fix**: `WHAD_RINGBUF_CAPACITY = WHAD_RINGBUF_MAX_SIZE - 1` (was reporting 1024 free in a 1024-byte array, causing off-by-one push into a full buffer).
- **Transport rewrite**: `whad_transport_send_message` replaces the two-step "send header, then payload" (which could leave half-frames on partial failure) with a **byte-by-byte push loop that counts `pushed` and rolls back via `whad_ringbuf_skip` on any failure**. Pre-validates oversize payloads against `WHAD_MAX_ENCODED_MESSAGE_SIZE` (= 1019). RX path rejects oversize frames and resyncs byte-by-byte on bad magic / malicious length.
- **New constant** `WHAD_MAX_ENCODED_MESSAGE_SIZE = WHAD_RINGBUF_CAPACITY - 4` (= 1019 — matches `board_manifest.json`'s `encoded_message_limit`).

### NanoPbMsg ownership model (`inc/cpp/message.hpp` + `src/cpp/message.cpp`)
- New `m_ownsMessage` flag + `disown()` method for queue handoff semantics.
- Default destructor was previously `#if 0`-disabled (leaked every message). Now under `BOARD_CLUE` releases via `messagePoolReleaseMessage`; else via `free`.
- Default ctor under `BOARD_CLUE` allocates from `messagePoolAllocateMessage(NULL)` instead of `calloc`. This is the link to butterfly's fixed-size message pool (no `malloc` under SoftDevice).
- `MessageDomain` enum gets `DomainBoard` entry.

### Discovery schema updates (`inc/discovery.h`, `inc/cpp/discovery/domaininfo.hpp`)
- `DOMAIN_BOARD = discovery_Domain_Board` (= 0x0C000000).
- Four capability aliases: `CAP_READ/WRITE/STREAM/STORE` mapped to the new `discovery_Capability_*` constants from the Board-aware device.proto.
- Convenience defines: `CAP_BOARD_READ/WRITE/STREAM/STORE`.
- C++ `Domains` enum gets `DomainBoard`.

### Build / tests
- `Makefile` — `test`/`clean` goals bypass the `ARCH_ARM` gate (so `make test` works from host CC). Added `-Iwhad/protocol/board` to include path. `lib/` now `mkdir -p`'d on first build.
- `tests/` (NEW) — host-only C test suite: `tests/test_transport.c` (229L, 8 cases — zero-size frame, limit-size frame, oversize rejection, full-ring rejection, wraparound ordering, partial-input reassembly, magic-byte resync, malicious-length resync) and `tests/test_board.c` (76 entries — pack/parse round-trips + 38 validator cases covering enum ranges, byte-array bounds, repeated-field counts, status bitmasks, nested lease tokens, streaming chunks). Run with `make test`.

### Drift checker (`tools/check_generated_protocol.sh`, NEW 13L)
`diff -u`s checked-in `whad/protocol/*.pb.{c,h}` against `../whad-protocol/dist/nanopb/whad/protocol/` (sibling repo in standard 4-repo workspace layout; `PROTO_REF` env var overrides). Run after copying fresh nanopb outputs to confirm byte-identical sync.

### Provenance (`whad/protocol/board/protocol_source.json`)
Records where the checked-in nanopb outputs came from. Re-pointed to `../whad-protocol` (sibling layout) — was a stale `/tmp/opencode/...` path.

### Forward references
Consumed by:
- `butterfly` firmware (git submodule of butterfly; dispatcher `src/boardModule.cpp` calls the C API directly).

See workspace `README.md` for the integrated 4-repo picture and `TODO.md` for outstanding work.
