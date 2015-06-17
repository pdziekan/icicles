add_executable(icicles main.cpp micro.cpp opts.cpp)


# libmpdata++ and its dependencies
find_package(libmpdata++)

if (NOT libmpdataxx_FOUND)
  message(FATAL_ERROR "libmpdata++ not found")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  # make the Release flags the default
  set(CMAKE_CXX_FLAGS ${libmpdataxx_CXX_FLAGS_RELEASE})
  # ignore CMake defaults
  set(CMAKE_CXX_FLAGS_RELEASE "")
else()
  set(CMAKE_CXX_FLAGS_DEBUG ${libmpdataxx_CXX_FLAGS_DEBUG})
endif()

target_link_libraries(icicles ${libmpdataxx_LIBRARIES})
target_include_directories(icicles PUBLIC ${libmpdataxx_INCLUDE_DIRS})

# libcloudph++ and its dependencies
find_package(libcloudph++)

if (NOT libcloudphxx_FOUND)
  message(FATAL_ERROR "libcloudph++ not found")
endif()

# TODO: flags

target_link_libraries(icicles ${libcloudphxx_LIBRARIES})
target_include_directories(icicles PUBLIC ${libcloudphxx_INCLUDE_DIRS})
