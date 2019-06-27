set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -fPIC")

add_executable(test-hook
    main.cc
    test-hook.cc
)
target_link_libraries(test-hook
    baihook
    gtest
)
include_directories("../baihook")
install(TARGETS test-hook DESTINATION bin)
