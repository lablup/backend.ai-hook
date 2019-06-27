#! /bin/sh
set -e

distro="$1"
arch="x86_64"
case $distro in
  ubuntu) distro_ver="${distro}16.04" ;;
  alpine) distro_ver="${distro}3.8" ;;
  *)
    echo "Unknown distro value: ${distro}"
    exit 1
esac

echo ">> Running unit tests for ${distro} ${arch}"
docker run --rm -it -v $(pwd):/root \
           lablup/hook-dev:${distro} \
           /root/test-hook.${distro_ver}.${arch}.bin

echo ""
echo ">> Running integration tests for ${distro} ${arch}"
docker run --rm -it -v $(pwd):/root \
           --cpuset-cpus=0-1,4-5 \
           -e LD_PRELOAD=/root/libbaihook.${distro_ver}.${arch}.so \
           lablup/hook-dev:${distro} \
           /root/test-hooked.${distro_ver}.${arch}.bin
