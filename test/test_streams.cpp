#define BOOST_TEST_MODULE csv_streams

#include "text/csv/istream.hpp"
#include "text/csv/ostream.hpp"

#include <boost/test/included/unit_test.hpp>
#include <sstream>

namespace csv = ::text::csv;

namespace {

template <std::size_t N>
void generic_input_test(const char* text, const char *(&parts)[N])
{
    std::istringstream is(text);
    text::csv::csv_istream csv_in(is);

    std::string dest;
    std::size_t i = 0;
    while (csv_in) {
        csv_in >> dest;
        BOOST_ASSERT(i < N);
        BOOST_CHECK_EQUAL(dest, parts[i++]);
    }
    BOOST_ASSERT(i > 0);
}

}

BOOST_AUTO_TEST_SUITE(csv_streams)

BOOST_AUTO_TEST_CASE(simple_grid_out_test)
{
    std::ostringstream os;
    csv::csv_ostream csv_out(os);

    csv_out << 1 << 2 << 3 << csv::endl
            << 4 << 5 << 6 << csv::endl
            << 7 << 8 << 9 << csv::endl
        ;

    BOOST_CHECK_EQUAL(os.str(),
                      "1,2,3\r\n"
                      "4,5,6\r\n"
                      "7,8,9\r\n");
}


BOOST_AUTO_TEST_CASE(strings_with_quotes_and_commas)
{
    std::ostringstream os;
    csv::csv_ostream csv_out(os);

    csv_out << 1 << " \" Hello \" " << 1.5 << csv::endl
            << "Hello, how are you?" << 3 << 4 << csv::endl
        ;

    BOOST_CHECK_EQUAL(os.str(),
                      "1,\" \"\" Hello \"\" \",\"1.5\"\r\n"
                      "\"Hello, how are you?\",3,4\r\n");
}

BOOST_AUTO_TEST_CASE(simple_grid_in_test)
{
    const char * parts[] = {"1", "2", "3", "4", "5", "6"};
    const char * text = "1,2,3\r\n4,5,6\r\n";
    generic_input_test(text, parts);
}

BOOST_AUTO_TEST_CASE(custom_delimiter_test)
{
    const char * parts[] = {"a", "b", "c"};
    const char * const text = "a|b|c";
    std::istringstream ss(text);
    csv::csv_istream csv_in(ss, '|');
    std::string sink;
    for (int i = 0; i < sizeof parts / sizeof parts[0]; ++i) {
        csv_in >> sink;
        BOOST_CHECK_EQUAL(sink, parts[i]);
    }
    BOOST_ASSERT(!csv_in);
}

BOOST_AUTO_TEST_CASE(integral_input_test)
{
    const char * const text = "1,2,3\n\"4\",5,\"6\"\n7,8,9";
    std::istringstream ss(text);
    csv::csv_istream csv_in(ss);
    for (unsigned x, i = 1; i <= 9; ++i) {
        BOOST_CHECK(bool(csv_in));
        csv_in >> x;
        BOOST_CHECK_EQUAL(i, x);
    }
    BOOST_ASSERT(!csv_in);
}

BOOST_AUTO_TEST_CASE(double_input_test)
{
    const char * const text = "1.15,\"1.30\",1.45";
    const double nums[] = {1.15, 1.30, 1.45};
    std::istringstream ss(text);
    csv::csv_istream csv_in(ss);

    for (std::size_t i = 0; i < 3; ++i) {
        BOOST_CHECK(bool(csv_in));
        double d;
        csv_in >> d;
        BOOST_CHECK_EQUAL(nums[i], d);
    }

    BOOST_ASSERT(!csv_in);
}

BOOST_AUTO_TEST_CASE(quoted_strings_in_test)
{
    const char * parts[] = {"1,2,3", "the \"x\"", "\"x\" and \"y\""};
    const char * const text = "\"1,2,3\""
        ",\"the \"\"x\"\"\""
        ",\"\"\"x\"\" and \"\"y\"\"\"\r\n";
    generic_input_test(text, parts);
}

BOOST_AUTO_TEST_CASE(mixed_line_endings_in_input)
{
    const char * parts[] = {"1", "2", "3", "4"};
    const char * const text = "1\r\n2\n3\r\"4\"\n";
    generic_input_test(text, parts);
}

BOOST_AUTO_TEST_CASE(line_splitting)
{
    std::istringstream is("1,2\n3,4\n5,6\n");
    csv::csv_istream csv_in(is);
    std::string dest;
    while (csv_in) {
        csv_in >> dest;
        BOOST_CHECK(csv_in.has_more_fields());
        csv_in >> dest;
        BOOST_CHECK(!csv_in.has_more_fields());
    }
}

BOOST_AUTO_TEST_SUITE_END()