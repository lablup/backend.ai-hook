#! /bin/bash
set -e

FORCE_CMAKE=0
CLEAN=0


usage() {
  echo "Backend.AI Hook Build Script Script"
  echo ""
  echo "Usage: $0 [OPTIONS] DISTRO"
  echo ""
  echo "DISTRO is either ubuntu or alpine."
  echo ""
  echo "OPTIONS"
  echo "  -h, --help        Show this help message and exit."
  echo "  --clean           Run 'make clean' instead of 'make'."
  echo "  --force-cmake     Force to re-run cmake to refresh build scripts."
}

while [ $# -gt 0 ]; do
  case $1 in
    -h | --help) usage; exit 1 ;;
    --force-cmake) FORCE_CMAKE=1 ;;
    --clean) CLEAN=1 ;;
    *)
      break
  esac
  shift
done

distro="$1"
arch="x86_64"
case $distro in
  ubuntu) distro_ver="${distro}16.04" ;;
  alpine) distro_ver="${distro}3.8" ;;
  *)
    echo "Unknown distro value: ${distro}"
    exit 1
esac

echo "Building for ${distro} ${arch}"
docker build -t lablup/hook-dev:${distro} -f Dockerfile.${distro} .

docker_run="docker run --rm -it -v $(pwd):/root -w=/root lablup/hook-dev:${distro} /bin/sh -c"
if [ "$FORCE_CMAKE" -eq 1 -o ! -f "Makefile" ]; then
  $docker_run 'cmake CMakeLists.txt'
fi
if [ "$CLEAN" -eq 1 ]; then
  $docker_run 'make clean'
else
  $docker_run 'make'
  cp "baihook/libbaihook.so" "libbaihook.${distro_ver}.${arch}.so"
  cp "test/test-hook" "test-hook.${distro_ver}.${arch}.bin"
fi
