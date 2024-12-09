#!/usr/bin/env bash

filepath="./main"
size=8192

while [ $# -gt 0 ]; do
  case "$1" in
    --size)
      size="$2"
      shift 2
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

if ! [ -f "$filepath" ]; then
  echo "Executable not found at $filepath."
  exit 1
fi

cat /proc/cpuinfo | grep -E "model name|siblings|cache size" | sort | uniq

$filepath $size "horizontal"
$filepath $size "quadrants" 2
$filepath $size "quadrants" 4
