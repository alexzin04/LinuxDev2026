#!/bin/sh
set -eu

for algorithm in md5 sha1; do
    expected=$(printf abc | "${algorithm}sum" | cut -d ' ' -f1)
    name=$(printf '%s' "$algorithm" | tr '[:lower:]' '[:upper:]')
    actual=$(printf '%s "abc\n' "$name" | ./rhasher)
    test "$actual" = "$expected"
done
