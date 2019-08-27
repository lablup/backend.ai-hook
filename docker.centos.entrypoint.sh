#! /bin/bash
source scl_source enable devtoolset-7
export CC=$(which gcc)
export CXX=$(which g++)
exec "$@"
