set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -fPIC")

add_executable(test-hook
    main.cc
    test-hook.cc
)
target_link_libraries(test-hook
    baihook
    gtest
    pthread  # only required for ubuntu
    dl       # only required for ubuntu
)
include_directories("../baihook")
install(TARGETS test-hook DESTINATION bin)
