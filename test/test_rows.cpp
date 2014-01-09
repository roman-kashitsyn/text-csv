#include "text/csv/rows.hpp"

#include <boost/test/unit_test.hpp>
#include <sstream>

namespace csv = ::text::csv;

BOOST_AUTO_TEST_SUITE(csv_rows)

BOOST_AUTO_TEST_CASE(row_parsing_test)
{
    std::istringstream ss("\"a\",\"b\",\"c\"\nd,e,f");
    csv::csv_istream csv_in(ss);
    csv::row r(3), p;

    csv_in >> p;
    r[0] = "a"; r[1] = "b"; r[2] = "c";
    BOOST_CHECK_EQUAL(r, p);

    csv_in >> p;
    r[0] = "d"; r[1] = "e"; r[2] = "f";
    BOOST_CHECK_EQUAL(r, p);

    BOOST_CHECK(!csv_in);
}

BOOST_AUTO_TEST_CASE(map_row_lookup_test)
{
    csv::row first_row(3);
    first_row[0] = "id";
    first_row[1] = "name";
    first_row[2] = "score";

    csv::header h(first_row);

    csv::map_row r(h);

    r[0] = "12";
    r[1] = "John";
    r[2] = "0.3";

    BOOST_CHECK_EQUAL(r[1], r["name"]);
    BOOST_CHECK_EQUAL(r[0], r["id"]);
    BOOST_CHECK_EQUAL(r[2], r["score"]);

    r["score"] = "0.5";

    BOOST_CHECK_EQUAL("0.5", r[2]);
}

BOOST_AUTO_TEST_SUITE_END()
