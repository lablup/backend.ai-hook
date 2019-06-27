#! /bin/sh
set -e

distro="$1"
arch="x86_64"

# TODO: add version suffix to $distro

echo "Building for ${distro} ${arch}"
docker build -t lablup/hook-dev:${distro} -f Dockerfile.${distro} .

docker_run="docker run --rm -it -v $(pwd):/root -w=/root lablup/hook-dev:${distro} /bin/sh -c"
#$docker_run 'cmake CMakeLists.txt'
$docker_run 'make'
cp "baihook/libbaihook.so" "libbaihook.${distro}.${arch}.so"
cp "test/test-hook" "test-hook.${distro}.${arch}.bin"
