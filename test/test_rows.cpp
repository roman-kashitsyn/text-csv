#include "text/csv/rows.hpp"

#include <boost/test/unit_test.hpp>
#include <sstream>

namespace csv = ::text::csv;

BOOST_AUTO_TEST_SUITE(csv_rows)

static void string_to_row(const char *text, csv::row &r) {
    std::istringstream ss(text);
    csv::csv_istream csv_in(ss);
    csv_in >> r;
}

BOOST_AUTO_TEST_CASE(row_parsing_test) {
    std::istringstream ss("\"a\",\"b\",\"c\"\nd,e,f");
    csv::csv_istream csv_in(ss);
    csv::row r(3), p;

    csv_in >> p;
    r[0] = "a";
    r[1] = "b";
    r[2] = "c";
    BOOST_CHECK_EQUAL(r, p);

    csv_in >> p;
    r[0] = "d";
    r[1] = "e";
    r[2] = "f";
    BOOST_CHECK_EQUAL(r, p);

    BOOST_CHECK(!csv_in);
}

BOOST_AUTO_TEST_CASE(row_with_empty_field) {
    csv::row r;
    string_to_row("1,2,", r);

    BOOST_ASSERT(3 == r.size());
    BOOST_CHECK_EQUAL("1", r[0]);
    BOOST_CHECK_EQUAL("2", r[1]);
    BOOST_CHECK_EQUAL("", r[2]);
}

BOOST_AUTO_TEST_CASE(row_narrows_and_expands) {
    csv::row r;
    string_to_row("1,2,3", r);
    BOOST_ASSERT(3 == r.size());

    string_to_row("4,5", r);
    BOOST_ASSERT(2 == r.size());
    BOOST_CHECK_EQUAL("4", r[0]);
    BOOST_CHECK_EQUAL("5", r[1]);

    string_to_row("6,7,8,9", r);
    BOOST_ASSERT(4 == r.size());
    BOOST_CHECK_EQUAL("6", r[0]);
    BOOST_CHECK_EQUAL("7", r[1]);
    BOOST_CHECK_EQUAL("8", r[2]);
    BOOST_CHECK_EQUAL("9", r[3]);

    string_to_row("", r);
    BOOST_ASSERT(1 == r.size());
    BOOST_CHECK_EQUAL("", r[0]);
}

BOOST_AUTO_TEST_CASE(map_row_lookup_test) {
    const std::size_t n = 3;
    csv::row first_row(n);
    first_row[0] = "name";
    first_row[1] = "id";
    first_row[2] = "score";

    csv::header h(first_row);

    for (std::size_t i = 0; i < n; ++i) {
        BOOST_CHECK_EQUAL(first_row[i], h.name_of(i));
    }

    csv::map_row r(h);

    r[0] = "John";
    r[1] = "12";
    r[2] = "0.3";

    BOOST_CHECK_EQUAL(r[0], r["name"]);
    BOOST_CHECK_EQUAL(r[1], r["id"]);
    BOOST_CHECK_EQUAL(r[2], r["score"]);

    BOOST_CHECK_EQUAL(12, r.as<int>(1));
    BOOST_CHECK_EQUAL(12, r.as<int>("id"));
    BOOST_CHECK_EQUAL(0.3, r.as<double>(2));
    BOOST_CHECK_EQUAL(0.3, r.as<double>("score"));

    r["score"] = "0.5";

    BOOST_CHECK_EQUAL("0.5", r[2]);
    BOOST_CHECK_EQUAL("0.5", r["score"]);
    BOOST_CHECK_EQUAL(0.5, r.as<double>(2));
    BOOST_CHECK_EQUAL(0.5, r.as<double>("score"));

    for (std::size_t i = 0; i < n; ++i) {
        BOOST_CHECK(r.has_key(first_row[i]));
    }
}

BOOST_AUTO_TEST_SUITE_END()
