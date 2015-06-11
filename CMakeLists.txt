cmake_minimum_required(VERSION 2.8.7)
project(test-csv)

include_directories(include)

enable_testing()

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
    ${Boost_LIBRARIES}
    )

  add_test(basic_test csv_test)

  if (CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(csv_test
      PROPERTIES
      COMPILE_FLAGS
      "-Wall -Wextra -Werror"
      )
  endif()
endif()

install(
    FILES include/text/csv/istream.hpp
          include/text/csv/ostream.hpp
          include/text/csv/iterator.hpp
          include/text/csv/rows.hpp
          include/text/csv/stream_fwd.hpp
    DESTINATION include/text/csv/)
