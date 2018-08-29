alpine:
	docker build -t lablup/jail-hook-dev:alpine -f Dockerfile.alpine .
	docker run --rm -it -v $(shell pwd):/root lablup/jail-hook-dev:alpine /bin/sh -c 'cd /root; make inner'

debian:
	docker build -t lablup/jail-hook-dev:debian -f Dockerfile.debian .
	docker run --rm -it -v $(shell pwd):/root lablup/jail-hook-dev:debian /bin/sh -c 'cd /root; make inner'

inner:
	g++ -std=c++14 -Wall -shared -fPIC -Wl,--version-script=exports.txt -o libbaihook.so patch-libs.cc utils.cc -ldl
	strip -x -d libbaihook.so
