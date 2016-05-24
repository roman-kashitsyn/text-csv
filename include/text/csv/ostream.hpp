#ifndef TEXT_CSV_OSTREAM_HPP
#define TEXT_CSV_OSTREAM_HPP

//          Copyright Roman Kashitsyn 2014 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stream_fwd.hpp"
#include <algorithm>
#include <ostream>
#include <string>
#include <sstream>

namespace text {
namespace csv {

template <typename Char, typename Traits>
class basic_csv_ostream {
public:
    typedef Char char_type;
    typedef Traits traits_type;
    typedef std::basic_ostream<Char, Traits> stream_type;
    typedef std::basic_string<Char, Traits> string_type;
    typedef basic_csv_ostream &(*manip)(basic_csv_ostream &);

    basic_csv_ostream(stream_type &os);

    basic_csv_ostream(stream_type &os, char_type delimiter);

    basic_csv_ostream(stream_type &os, char_type delimiter, char_type quote);

    basic_csv_ostream &operator<<(bool b) { return insert(b); }

    basic_csv_ostream &operator<<(int i) { return insert(i); }

    basic_csv_ostream &operator<<(long l) { return insert(l); }

    basic_csv_ostream &operator<<(float f) { return insert(f); }

    basic_csv_ostream &operator<<(double d) { return insert(d); }

    basic_csv_ostream &operator<<(long double ld) { return insert(ld); }

    basic_csv_ostream &operator<<(char_type const *);

    basic_csv_ostream &operator<<(string_type const &);

    basic_csv_ostream &operator<<(manip m) { return m(*this); }

    basic_csv_ostream &end_line();

private:
    basic_csv_ostream(basic_csv_ostream const &);
    basic_csv_ostream &operator=(basic_csv_ostream const &);

    void insert_delimiter() {
        if (!first_) {
            os_.put(delim_);
        } else {
            first_ = false;
        }
    }

    basic_csv_ostream &insert(char_type const *begin, char_type const *end);

    template <typename T>
    basic_csv_ostream &insert(T const &t) {
        std::basic_ostringstream<Char, Traits> buf;
        buf.copyfmt(os_);
        buf << t;
        return (*this) << buf.str();
    }

private:
    stream_type &os_;
    char_type const delim_;
    char_type const quote_;
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
basic_csv_ostream<Char, Traits> &basic_csv_ostream<Char, Traits>::operator<<(
    char_type const *c) {
    char_type const *end = c;
    while (*end)
        ++end;
    return insert(c, end);
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &basic_csv_ostream<Char, Traits>::operator<<(
    string_type const &s) {
    return insert(s.data(), s.data() + s.size());
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &basic_csv_ostream<Char, Traits>::end_line() {
    os_.put(os_.widen(CR));
    os_.put(os_.widen(LF));
    first_ = true;

    return *this;
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &endl(basic_csv_ostream<Char, Traits> &cos) {
    return cos.end_line();
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &
basic_csv_ostream<Char, Traits>::insert(char_type const *begin,
                                        char_type const *end) {
    insert_delimiter();

    const char_type special_symbols[] = {
        delim_, quote_, os_.widen(CR), os_.widen(LF),
    };
    const char_type *const special_symbols_end =
        special_symbols + sizeof(special_symbols) / sizeof(special_symbols[0]);

    const bool has_special_symbols =
        std::find_first_of(begin, end, special_symbols, special_symbols_end) !=
        end;

    if (!has_special_symbols) {
        os_.write(begin, std::streamsize(end - begin));
    } else {
        os_.put(quote_);
        for (char_type const *pos = begin; pos != end; ++pos) {
            if (*pos == quote_) {
                os_.put(quote_);
            }
            os_.put(*pos);
        }
        os_.put(quote_);
    }
    return *this;
}
} // namespace csv
} // namespace text

#endif
