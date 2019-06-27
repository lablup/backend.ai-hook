#! /bin/sh
set -e

distro="$1"
arch="x86_64"

# TODO: add version suffix to $distro

echo "Running tests for ${distro} ${arch}"

docker run --rm -it -v $(pwd):/root lablup/hook-dev:${distro} /root/test-hook.${distro}.${arch}.bin
