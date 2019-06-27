#! /bin/sh
set -e

distro="$1"
arch="x86_64"
case $distro in
  "ubuntu") distro_ver="${distro}16.04" ;;
  "alpine") distro_ver="${distro}3.8" ;;
esac

echo "Running tests for ${distro} ${arch}"

docker run --rm -it -v $(pwd):/root lablup/hook-dev:${distro} /root/test-hook.${distro_ver}.${arch}.bin
