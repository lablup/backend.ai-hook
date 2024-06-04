#! /bin/bash
export CC=$(which gcc)
export CXX=$(which g++)
exec "$@"
