#!/bin/bash
set -e

MAIN_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROTOBUF_DIR="$MAIN_DIR/repo"
BIN_DIR="$MAIN_DIR/bin"
PROTOC_PATH="$BIN_DIR/protoc"

build_protoc() {
{
    mkdir -p "$BIN_DIR"
    
    (cd "$PROTOBUF_DIR" && autoreconf -f -i -Wall,no-obsolete)
    (cd "$PROTOBUF_DIR" && ./configure --disable-shared --enable-static)
    make -j$(nproc) -C "$PROTOBUF_DIR"

    cp $PROTOBUF_DIR/src/protoc "$BIN_DIR"
    git -C $PROTOBUF_DIR clean -fd
} > /dev/null
}

if [ ! -f "$PROTOC_PATH" ]; then
    build_protoc
fi

exec "$PROTOC_PATH" "$@"
