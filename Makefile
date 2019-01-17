alpine:
	docker build -t lablup/hook-dev:alpine -f Dockerfile.alpine .
	docker run --rm -it -v $(shell pwd):/root lablup/hook-dev:alpine /bin/sh -c 'cd /root; make inner distro=alpine3.8 arch=x86_64'

ubuntu:
	docker build -t lablup/hook-dev:ubuntu -f Dockerfile.ubuntu .
	docker run --rm -it -v $(shell pwd):/root lablup/hook-dev:ubuntu /bin/sh -c 'cd /root; make inner distro=ubuntu16.04 arch=x86_64'

inner:
	g++ -std=c++14 -Wall -fPIC -static-libstdc++ -static-libgcc -shared -Wl,--version-script=exports.txt -o libbaihook.$(distro).$(arch).so patch-libs.cc utils.cc -ldl
