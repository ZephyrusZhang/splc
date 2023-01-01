#!/usr/bin/env bash

INPUT_FILE=$1
IR_FILE="${INPUT_FILE/.spl/.ir}"
GCC_OUT_FILE="${INPUT_FILE/.spl/.out}"

echo "Input file: ${INPUT_FILE}, IR file: ${IR_FILE}"
echo
../bin/splc "$INPUT_FILE"
echo
[ -f "$IR_FILE" ] || exit 1

GCC_SRC=$(mktemp)
cat <<EOF >> "$GCC_SRC"
#include <stdio.h>
int read() {
    int r;
    scanf("%d", &r);
    return r;
}
void write(int r) {
    printf("%d\n", r);
}

EOF

cat "$INPUT_FILE" >> "$GCC_SRC"
gcc -x c "$GCC_SRC" -o "$GCC_OUT_FILE"

INPUTS=$(mktemp)
grep '//' "$INPUT_FILE" | sed "s/\/\/ //g" > "$INPUTS"

function print_failed() {
    echo "Failed."
    echo "gcc output:"
    cat "$1"
    echo "irsim output:"
    cat "$2"
}

while read -r p; do
  echo "=== Test ==="
  echo "Input: $p"
  GCC_INPUT="${p/,/ }"
  GCC_OUTPUT=$(mktemp)
  IRSIM_OUTPUT=$(mktemp)
  echo "$GCC_INPUT" | ./"$GCC_OUT_FILE" > "$GCC_OUTPUT"
  python3 ../irsim/irsim.py "$IR_FILE" --non-interactive -i "$p" -O "$IRSIM_OUTPUT"
  diff "$GCC_OUTPUT" "$IRSIM_OUTPUT" > /dev/null || print_failed "$GCC_OUTPUT" "$IRSIM_OUTPUT"
  rm "$GCC_OUTPUT"
  rm "$IRSIM_OUTPUT"
  echo
done <"$INPUTS"

rm "$GCC_SRC"
rm "$INPUTS"
