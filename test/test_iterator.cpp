#include "text/csv/iterator.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace csv = ::text::csv;

namespace {

template <typename ValueType, std::size_t N>
void generic_input_column_iterator_test
( text::csv::input_column_iterator<ValueType> it
, const ValueType (&expected)[N]
)
{
    text::csv::input_column_iterator<ValueType> end;

    for (std::size_t i = 0; i < N; ++i) {
        BOOST_CHECK_EQUAL(expected[i], *it);
        ++it;
    }

    BOOST_CHECK(it == end);
}

}

BOOST_AUTO_TEST_SUITE(csv_iterators)

BOOST_AUTO_TEST_CASE(input_column_iterator_test)
{
    std::istringstream ss("1,2,3\n4,5,6");
    csv::csv_istream is(ss);

    const int first_column[] = {1, 2, 3};
    const int second_column[] = {4, 5, 6};

    generic_input_column_iterator_test(csv::input_column_iterator<int>(is),
                                       first_column);
    generic_input_column_iterator_test(csv::input_column_iterator<int>(is),
                                       second_column);
}

BOOST_AUTO_TEST_CASE(input_column_iterator_algo_test)
{
    typedef csv::input_column_iterator<int> I;
    const int values[] = {1, 2, 3, 4, 5, 6};
    const std::size_t n = sizeof values / sizeof values[0];

    std::istringstream ss("1,2,3,4,5,6");
    csv::csv_istream is(ss);
    std::vector<int> v, expected(values, values + n);

    std::copy(I(is), I(), std::back_inserter(v));
    BOOST_CHECK(expected == v);
}

BOOST_AUTO_TEST_CASE(output_column_iterator_int_test)
{
    std::ostringstream ss;
    csv::csv_ostream os(ss, '|');
    const int values[] = {1, 2, 3, 4, 5};
    const std::size_t n = sizeof values / sizeof values[0];
    std::copy(values, values + n, csv::output_column_iterator<int>(os));
    BOOST_CHECK_EQUAL(ss.str(), "1|2|3|4|5");
}

BOOST_AUTO_TEST_CASE(output_column_iterator_string_test)
{
    std::ostringstream ss;
    csv::csv_ostream os(ss);

    std::vector<std::string> values;
    values.push_back("a");
    values.push_back("b");
    values.push_back("c");

    std::copy(values.begin(), values.end(),
              csv::output_column_iterator<std::string>(os));
    BOOST_CHECK_EQUAL(ss.str(), "\"a\",\"b\",\"c\"");
}

BOOST_AUTO_TEST_SUITE_END()
