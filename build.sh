#! /bin/bash
set -e

FORCE_CMAKE=0
CLEAN=0


usage() {
  echo "Backend.AI Hook Build Script Script"
  echo ""
  echo "Usage: $0 [OPTIONS] DISTRO"
  echo ""
  echo "DISTRO can be one of: centos ubuntu alpine."
  echo "You can also pass `all` as DISTRO to build Backend.AI Hook for all supported distros."
  echo ""
  echo "OPTIONS"
  echo "  -h, --help        Show this help message and exit."
  echo "  --clean           Run 'make clean' instead of 'make'."
  echo "  --force-cmake     Force to re-run cmake to refresh build scripts."
}

build() {
  distro_ver=$1
  # match the container's user/group with this script
  user="$(id -u):$(id -g)"
  # to prevent "fatal: unable to look up current user in the passwd file: no such user" error from git
  git_fix="-e GIT_COMMITTER_NAME=devops -e GIT_COMMITTER_EMAIL=devops@lablup.com"

  docker build -t lablup/hook-dev:${distro} -f Dockerfile.${distro} .
  docker_run="docker run --rm -it ${git_fix} -v "$(pwd):/root" -u ${user} -w=/root lablup/hook-dev:${distro} /bin/sh -c"

  if [ "$FORCE_CMAKE" -eq 1 -o ! -f "Makefile" ]; then
    echo ">> Running CMake to (re-)generate build scripts..."
    $docker_run 'cmake CMakeLists.txt'
  fi
  if [ "$CLEAN" -eq 1 ]; then
    echo ">> Cleaning up..."
    $docker_run 'make clean'
  else
    echo ">> Building for ${distro} ${arch} ..."
    $docker_run 'make'
    cp "baihook/libbaihook.so" "libbaihook.${distro_ver}.${arch}.so"
    cp "test/test-hook" "test-hook.${distro_ver}.${arch}.bin"
    cp "test/test-hooked" "test-hooked.${distro_ver}.${arch}.bin"
  fi
}

build_all() {
  FORCE_CMAKE=1
  distro="ubuntu" build "ubuntu20.04"
  distro="centos" build "centos7.6"
  distro="alpine" build "alpine3.8"
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
arch="$(uname -m)"
if [ "${arch}" = "arm64" ]; then
  arch="aarch64"
fi

case $distro in
  ubuntu) build "${distro}20.04" ;;
  centos) build "${distro}7.6" ;;
  alpine) build "${distro}3.8" ;;
  all) build_all ;;
  *)
    echo "Unknown distro value: ${distro}"
    exit 1
esac

