set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -fPIC -g")
set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -Wl,--version-script=exports.txt -Wl,--wrap=memcpy -Wl,--no-as-needed -ldl")

add_library(baihook SHARED
    patch-libs.cc
    utils.cc
    compat.c
)
install(TARGETS baihook DESTINATION lib)
