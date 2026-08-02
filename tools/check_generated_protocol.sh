#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
# Sibling whad-protocol repo in the standard 4-repo workspace layout.
# Override with PROTO_REF=... when running from a different layout.
SOURCE_DIR="${PROTO_REF:-$ROOT_DIR/../whad-protocol/dist/nanopb/whad/protocol}"
DEST_DIR="$ROOT_DIR/whad/protocol"

diff -u "$SOURCE_DIR/whad.pb.h" "$DEST_DIR/whad.pb.h"
diff -u "$SOURCE_DIR/whad.pb.c" "$DEST_DIR/whad.pb.c"
diff -u "$SOURCE_DIR/device.pb.h" "$DEST_DIR/device.pb.h"
diff -u "$SOURCE_DIR/device.pb.c" "$DEST_DIR/device.pb.c"
diff -u "$SOURCE_DIR/board/board.pb.h" "$DEST_DIR/board/board.pb.h"
diff -u "$SOURCE_DIR/board/board.pb.c" "$DEST_DIR/board/board.pb.c"
