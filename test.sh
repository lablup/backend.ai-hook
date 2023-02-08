#! /bin/sh
set -e

distro="$1"
arch="$(uname -m)"
if [ "$arch" = "aarch64" ]; then
  image_arch="arm64"
fi
version="$2"
if [ -z "$version" ]; then
  echo "Need version of distro: ${distro} <version>"
  exit 1
fi

case $distro in
  ubuntu) distro_ver="${distro}${version}" ;;
  centos) distro_ver="${distro}${version}" ;;
  alpine) distro_ver="${distro}${version}" ;;
  *)
    echo "Unknown distro value: ${distro}"
    exit 1
esac

user="$(id -u):$(id -g)"

echo ">> Running unit tests for ${distro} ${arch}"
docker run --rm -it -u ${user} -v $(pwd):/root \
           lablup/hook-dev:${distro} \
           /root/test-hook.${distro_ver}.${arch}.bin

echo ""
echo ">> Running integration tests for ${distro} ${arch}"
docker run --rm -it -u ${user} -v $(pwd):/root \
           --cpuset-cpus=0-1,4-5 \
           --memory=1g \
           -e LD_PRELOAD=/root/libbaihook.${distro_ver}.${arch}.so \
           lablup/hook-dev:${distro} \
           /root/test-hooked.${distro_ver}.${arch}.bin

if [ "$distro" = "ubuntu" ]; then
  echo ""
  echo ">> Running integration tests for lablup/python-ff:19.06-py36"
  docker run --rm -it -u ${user} -v $(pwd):/root -w /root/test \
             --cpuset-cpus=0-1,4-5 \
             --memory=1g \
             -e MPLCONFIGDIR=/tmp/output \
             -e LD_PRELOAD=/root/libbaihook.ubuntu18.04.x86_64.so \
             lablup/python-ff:21.08-py38-cuda11.1 \
             python /root/test/test-numpy.py

  # docker run --rm -it -u ${user} -v $(pwd):/root -w /root/test \
  #            --cpuset-cpus=0-1,4-5 \
  #            --memory=1g \
  #            -e MPLCONFIGDIR=/tmp/output \
  #            -e LD_PRELOAD=/root/libbaihook.ubuntu18.04.x86_64.so \
  #            lablup/python-ff:21.08-py38-cuda11.1 \
  #            sh -c 'mkdir -p /tmp/output; jupyter nbconvert --to notebook --execute assets/src.ipynb --output assets/result.ipynb; ls -l /tmp && ls -l assets'
fi
