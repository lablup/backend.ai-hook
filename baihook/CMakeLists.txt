set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -Wl,--version-script=exports.txt")

add_library(baihook SHARED
    patch-libs.cc
    utils.cc
)
install(TARGETS baihook DESTINATION lib)
