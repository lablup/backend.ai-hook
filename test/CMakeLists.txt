set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")

add_executable(test-hook
    main.cc
    test-hook.cc
)
add_executable(test-hooked
    main.cc
    test-hooked.cc
)
target_link_libraries(test-hook
    baihook
    gtest
    pthread  # only required for ubuntu
    dl       # only required for ubuntu
)
target_link_libraries(test-hooked
    gtest
    pthread  # only required for ubuntu
    dl       # only required for ubuntu
)
include_directories("../baihook")
install(TARGETS test-hook DESTINATION bin)
