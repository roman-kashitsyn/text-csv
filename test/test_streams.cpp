#define BOOST_TEST_MODULE csv_streams

#include "text/csv/istream.hpp"
#include "text/csv/ostream.hpp"

#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <locale>

namespace csv = ::text::csv;

namespace {

template <typename Char, std::size_t N>
void generic_input_test(const Char* text, const Char *(&parts)[N])
{
    std::basic_istringstream<Char> is(text);
    text::csv::basic_csv_istream<Char> csv_in(is);

    std::basic_string<Char> dest;
    std::size_t i = 0;
    while (csv_in) {
        csv_in >> dest;
        BOOST_ASSERT(i < N);
        BOOST_CHECK(dest == parts[i]);
        i++;
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

BOOST_AUTO_TEST_CASE(wide_grid_out_test)
{
    std::wostringstream os;
    csv::csv_wostream csv_out(os);

    csv_out << 1 << 2 << 3 << csv::endl
            << 4 << 5 << 6 << csv::endl
            ;
    BOOST_CHECK(os.str() == L"1,2,3\r\n4,5,6\r\n");
}

BOOST_AUTO_TEST_CASE(strings_with_quotes_and_commas)
{
    std::ostringstream os;
    csv::csv_ostream csv_out(os);

    csv_out << 1 << " \" Hello \" " << 1.5 << csv::endl
            << "Hello, how are you?" << 3 << 4 << csv::endl
            << "Delim , and \"quotes\"" << csv::endl
        ;

    BOOST_CHECK_EQUAL(os.str(),
                      "1, \" Hello \" ,1.5\r\n"
                      "\"Hello, how are you?\",3,4\r\n"
                      "\"Delim , and \"\"quotes\"\"\"\r\n");
}

BOOST_AUTO_TEST_CASE(read_quoted)
{
    std::istringstream is("abc,\"Delim , and \"\"quotes\"\"\"");
    csv::csv_istream csv_in(is);
    std::string left, right;
    csv_in >> left >> right;
    BOOST_CHECK_EQUAL(left, "abc");
    BOOST_CHECK_EQUAL(right, "Delim , and \"quotes\"");
}

namespace {

struct thousand_sep : std::numpunct<char> {
    string_type do_grouping() const { return "\3"; }
    char_type do_thousand_sep() const { return ','; }
};

}

BOOST_AUTO_TEST_CASE(numbers_with_commas)
{
    std::ostringstream os;
    os.imbue(std::locale(std::locale::classic(), new thousand_sep));

    csv::csv_ostream csv_out(os);

    csv_out << 1000000 << 20000 << 100 << csv::endl;

    BOOST_CHECK_EQUAL(os.str(), "\"1,000,000\",\"20,000\",100\r\n");
}

BOOST_AUTO_TEST_CASE(empty_cells_test)
{
    std::ostringstream os;
    csv::csv_ostream csv_out(os);
    csv_out << "" << "a" << "" << csv::endl
            << "b" << "" << "c" << csv::endl;
    BOOST_CHECK_EQUAL(os.str(),
                      ",a,\r\n"
                      "b,,c\r\n");
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
    for (std::size_t i = 0; i < sizeof parts / sizeof parts[0]; ++i) {
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

BOOST_AUTO_TEST_CASE(wide_input_stream)
{
    const wchar_t *parts[] = {L"1", L"2", L"3", L"4"};
    const wchar_t *const text = L"1,2,3,4";
    generic_input_test(text, parts);
}

BOOST_AUTO_TEST_SUITE_END()
