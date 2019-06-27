#! /bin/sh
set -e
distro=$1
arch="x86_64"

docker run --rm -it -v $(pwd):/root lablup/hook-dev:${distro} /root/test-hook.${distro}.${arch}.bin
