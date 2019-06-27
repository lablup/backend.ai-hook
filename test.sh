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
             -e LD_PRELOAD=/root/libbaihook.ubuntu16.04.x86_64.so \
             lablup/python-ff:19.06-py36 \
             python /root/test/test-numpy.py

  docker run --rm -it -u ${user} -v $(pwd):/root -w /root/test \
             --cpuset-cpus=0-1,4-5 \
             --memory=1g \
             -e LD_PRELOAD=/root/libbaihook.ubuntu16.04.x86_64.so \
             lablup/python-ff:19.06-py36 \
             sh -c 'mkdir -p /tmp/output; jupyter nbconvert --to notebook --execute assets/src.ipynb --output assets/result.ipynb && ls -l /tmp && ls -l assets'
fi
