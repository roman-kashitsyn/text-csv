cmake_minimum_required(VERSION 2.8)
project(test-csv)

include_directories(include)

find_package(Boost
  COMPONENTS unit_test_framework)

if (Boost_FOUND)
  add_executable(csv_test
    test/test_rows.cpp
    test/test_streams.cpp
    test/test_iterator.cpp
    test/test_ranges.cpp
    )
  target_link_libraries(csv_test
    boost_unit_test_framework
    )
endif()
