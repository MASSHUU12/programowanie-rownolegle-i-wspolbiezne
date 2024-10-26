#!/usr/bin/env bash

filepath="./a.out"
iterations=$(nproc) # By default use number of CPUs.
resolution=1024

while [ $# -gt 0 ]; do
  case "$1" in
    --filepath)
      filepath="$2"
      shift 2
      ;;
    --iterations)
      iterations="$2"
      shift 2
      ;;
    --resolution)
      resolution="$2"
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

echo
echo "Resolution: $resolution"
echo

for i in $(seq 1 $iterations); do
  echo "Number of threads: $i"

  $filepath $i $resolution

  echo
done
