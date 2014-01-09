#ifndef TEXT_CSV_OSTREAM_HPP
#define TEXT_CSV_OSTREAM_HPP

#include "text/csv/stream_fwd.hpp"
#include <ostream>
#include <string>

namespace text {
namespace csv {

template <typename Char, typename Traits>
class basic_csv_ostream
{
public:
    typedef Char char_type;
    typedef Traits traits_type;
    typedef basic_csv_ostream & (*manip)(basic_csv_ostream &);

    basic_csv_ostream(std::basic_ostream<Char, Traits> & os);

    basic_csv_ostream(std::basic_ostream<Char, Traits> & os,
                      char_type delimiter);

    basic_csv_ostream(std::basic_ostream<Char, Traits> & os,
                      char_type delimiter,
                      char_type quote);

    basic_csv_ostream & operator<<(bool b)
    { return insert(b); }

    basic_csv_ostream & operator<<(int i)
    { return insert(i); }

    basic_csv_ostream & operator<<(long l)
    { return insert(l); }

    basic_csv_ostream & operator<<(float f)
    { return insert_quoted(f); }

    basic_csv_ostream & operator<<(double d)
    { return insert_quoted(d); }

    basic_csv_ostream & operator<<(long double ld)
    { return insert_quoted(ld); }

    basic_csv_ostream & operator<<(char_type const *);

    basic_csv_ostream & operator<<(std::basic_string<Char, Traits> const&);

    //    basic_csv_ostream & operator<<(line const&);

    basic_csv_ostream & operator<<(manip m)
    { return m(*this); }

    basic_csv_ostream & end_line();

private:
    basic_csv_ostream(basic_csv_ostream const&);
    basic_csv_ostream & operator=(basic_csv_ostream const&);

    void insert_delimiter()
    {
        if (!first_) {
            os_.put(delim_);
        } else {
            first_ = false;
        }
    }

    template <typename T>
    basic_csv_ostream & insert(T const& t)
    {
        insert_delimiter();
        os_ << t;
        return *this;
    }

    template <typename T>
    basic_csv_ostream & insert_quoted(T const & t)
    {
        insert_delimiter();
        os_ << quote_ << t << quote_;
        return *this;
    }

private:
    std::basic_ostream<Char, Traits> & os_;
    const char_type delim_;
    const char_type quote_;
    bool first_;
};

// Implementation
// --------------

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits>::basic_csv_ostream
(std::basic_ostream<Char, Traits> & os)
    : os_(os)
    , delim_(os.widen(COMMA))
    , quote_(os.widen(QUOTE))
    , first_(true)
{}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits>::basic_csv_ostream
(std::basic_ostream<Char, Traits> & os, char_type delimiter)
    : os_(os)
    , delim_(delimiter)
    , quote_(os.widen(QUOTE))
    , first_(true)
{}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits>::basic_csv_ostream
(std::basic_ostream<Char, Traits> & os,
 char_type delimiter,
 char_type quote)
    : os_(os)
    , delim_(delimiter)
    , quote_(quote)
    , first_(true)
{}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &
basic_csv_ostream<Char, Traits>::operator<<
(char_type const * c)
{
    insert_delimiter();
    os_.put(quote_);
    while (*c) {
        if (*c == quote_) {
            os_.put(quote_);
        }
        os_.put(*c);
        ++c;
    }
    os_.put(quote_);

    return *this;
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &
basic_csv_ostream<Char, Traits>::operator<<
(std::basic_string<Char, Traits> const& s)
{
    insert_delimiter();

    os_.put(quote_);
    for (std::size_t i = 0, n = s.size(); i < n; ++i) {
        if (s[i] == quote_) {
            os_.put(quote_);
        }
        os_.put(s[i]);
    }
    os_.put(quote_);

    return *this;
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &
basic_csv_ostream<Char, Traits>::end_line()
{
    os_.put(os_.widen(CR));
    os_.put(os_.widen(LF));
    first_ = true;

    return *this;
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> & endl(basic_csv_ostream<Char, Traits> & cos)
{
    return cos.end_line();
}

} }

#endif
