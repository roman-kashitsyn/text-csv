#include "text/csv/iterator.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

namespace csv = ::text::csv;

namespace {

template <typename ValueType, std::size_t N>
void generic_input_column_iterator_test(
    text::csv::input_column_iterator<ValueType> it,
    const ValueType (&expected)[N]) {
    text::csv::input_column_iterator<ValueType> end;

    for (std::size_t i = 0; i < N; ++i) {
        BOOST_CHECK_EQUAL(expected[i], *it);
        ++it;
    }

    BOOST_CHECK(it == end);
}
}

BOOST_AUTO_TEST_SUITE(csv_iterators)

BOOST_AUTO_TEST_CASE(input_column_iterator_test) {
    std::istringstream ss("1,2,3\n4,5,6");
    csv::csv_istream is(ss);

    const int first_column[] = { 1, 2, 3 };
    const int second_column[] = { 4, 5, 6 };

    generic_input_column_iterator_test(csv::input_column_iterator<int>(is),
                                       first_column);
    generic_input_column_iterator_test(csv::input_column_iterator<int>(is),
                                       second_column);
}

BOOST_AUTO_TEST_CASE(input_column_iterator_algo_test) {
    typedef csv::input_column_iterator<int> I;
    const int values[] = { 1, 2, 3, 4, 5, 6 };
    const std::size_t n = sizeof values / sizeof values[0];

    std::istringstream ss("1,2,3,4,5,6");
    csv::csv_istream is(ss);
    std::vector<int> v, expected(values, values + n);

    std::copy(I(is), I(), std::back_inserter(v));
    BOOST_CHECK(expected == v);
}

BOOST_AUTO_TEST_CASE(output_column_iterator_int_test) {
    std::ostringstream ss;
    csv::csv_ostream os(ss, '|');
    const int values[] = { 1, 2, 3, 4, 5 };
    const std::size_t n = sizeof values / sizeof values[0];
    std::copy(values, values + n, csv::output_column_iterator<int>(os));
    BOOST_CHECK_EQUAL(ss.str(), "1|2|3|4|5");
}

BOOST_AUTO_TEST_CASE(output_column_iterator_string_test) {
    std::ostringstream ss;
    csv::csv_ostream os(ss);

    std::vector<std::string> values;
    values.push_back("a");
    values.push_back("b");
    values.push_back("c");

    std::copy(values.begin(), values.end(),
              csv::output_column_iterator<std::string>(os));
    BOOST_CHECK_EQUAL(ss.str(), "a,b,c");
}

BOOST_AUTO_TEST_CASE(zipping_iterator_test) {
    std::istringstream is("c,b,a\n3,2,1");
    csv::csv_istream cis(is);
    csv::header h(cis);
    csv::map_row mr(h);
    cis >> mr;

    csv::zipping_iterator<csv::map_row> b = pairs_begin(mr), e = pairs_end(mr);
    BOOST_CHECK_EQUAL("c", b->first);
    BOOST_CHECK_EQUAL("3", b->second);
    ++b;
    BOOST_CHECK_EQUAL("b", b->first);
    BOOST_CHECK_EQUAL("2", b->second);
    ++b;
    BOOST_CHECK_EQUAL("a", b->first);
    BOOST_CHECK_EQUAL("1", b->second);
    ++b;
    BOOST_CHECK(b == e);
}

BOOST_AUTO_TEST_CASE(map_construction_test) {
    typedef std::map<std::string, std::string> string_map;

    std::istringstream is("c,a,b\n3,1,2");
    csv::csv_istream cis(is);
    csv::map_row row(cis);

    string_map map(csv::pairs_begin(row), csv::pairs_end(row));

    BOOST_CHECK_EQUAL(map.size(), 3);
    BOOST_CHECK_EQUAL(map["a"], "1");
    BOOST_CHECK_EQUAL(map["b"], "2");
    BOOST_CHECK_EQUAL(map["c"], "3");
}

BOOST_AUTO_TEST_SUITE_END()
