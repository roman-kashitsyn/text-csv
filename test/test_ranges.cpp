#include "text/csv/iterator.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <boost/test/unit_test.hpp>

using text::csv::row_range;
using text::csv::map_row_range;

static const std::string NUMERIC_DATA = "1,2,3\n4,5,6";

BOOST_AUTO_TEST_SUITE(csv_ranges)

BOOST_AUTO_TEST_CASE(numeric_row_range_test) {
    std::istringstream in(NUMERIC_DATA);

    std::vector<std::vector<std::string> > grid(2);
    grid[0].push_back("1");
    grid[1].push_back("4");
    grid[0].push_back("2");
    grid[1].push_back("5");
    grid[0].push_back("3");
    grid[1].push_back("6");

    row_range range(in);

    std::size_t i = 0;
    for (row_range::iterator r = range.begin(), e = range.end(); r != e; ++r) {
        for (std::size_t j = 0; j < grid[i].size(); ++j) {
            BOOST_CHECK_EQUAL(grid[i][j], (*r)[j]);
        }
        ++i;
    }
    BOOST_CHECK_EQUAL(i, grid.size());
}

BOOST_AUTO_TEST_CASE(map_row_range_test) {
    const char *const values[] = { "1", "2", "3", "4", "5", "6" };
    std::istringstream in("x,y,z\n1,2,3\n4,5,6");
    map_row_range range(in);

    std::size_t i = 0;
    for (map_row_range::iterator r = range.begin(), e = range.end(); r != e;
         ++r) {
        BOOST_CHECK_EQUAL(values[i++], (*r)["x"]);
        BOOST_CHECK_EQUAL(values[i++], (*r)["y"]);
        BOOST_CHECK_EQUAL(values[i++], (*r)["z"]);
    }
    BOOST_CHECK_EQUAL(sizeof values / sizeof values[0], i);
}

BOOST_AUTO_TEST_SUITE_END()
