#ifndef TEXT_CSV_ISTREAM_HPP
#define TEXT_CSV_ISTREAM_HPP

//          Copyright Roman Kashitsyn 2014 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stream_fwd.hpp"
#include <istream>
#include <string>
#include <stdexcept>

// http://www.ietf.org/rfc/rfc4180.txt
// ===================================
//
// The ABNF grammar [2] appears as follows:
//
// file = [header CRLF] record *(CRLF record) [CRLF]
// header = name *(COMMA name)
// record = field *(COMMA field)
// name = field
// field = (escaped / non-escaped)
// escaped = DQUOTE *(TEXTDATA / COMMA / CR / LF / 2DQUOTE) DQUOTE
// non-escaped = *TEXTDATA
// COMMA = %x2C
// CR = %x0D

namespace text {
namespace csv {

template <typename Char, typename Traits>
class basic_csv_istream {
public:
    typedef Char char_type;
    typedef std::basic_istream<Char, Traits> stream_type;
    typedef std::basic_string<Char, Traits> string_type;

    basic_csv_istream(stream_type & is)
        : is_(is)
        , delim_(is.widen(COMMA))
        , quote_(is.widen(QUOTE))
        , line_(1)
        , pos_(0)
        , more_fields_(true)
    {}

    basic_csv_istream(stream_type & is, char_type delimiter)
        : is_(is)
        , delim_(delimiter)
        , quote_(is.widen(QUOTE))
        , line_(1)
        , pos_(0)
        , more_fields_(true)
    {}

    basic_csv_istream(stream_type & is, char_type delimiter, char_type quote)
        : is_(is)
        , delim_(delimiter)
        , quote_(quote)
        , line_(1)
        , pos_(0)
        , more_fields_(true)
    {}

    basic_csv_istream &operator>>(string_type &);

    basic_csv_istream &operator>>(bool &b) { return read_raw(b); }

    basic_csv_istream &operator>>(int &i) { return read_raw(i); }

    basic_csv_istream &operator>>(unsigned &u) { return read_raw(u); }

    basic_csv_istream &operator>>(long &l) { return read_raw(l); }

    basic_csv_istream &operator>>(unsigned long &ul) { return read_raw(ul); }

    basic_csv_istream &operator>>(float &f) { return read_raw(f); }

    basic_csv_istream &operator>>(double &d) { return read_raw(d); }

    bool eof() { return is_eof(peek_char()); }

    bool good() const { return is_.good(); }

    operator bool() { return is_.good() && !eof(); }

    bool has_more_fields() const { return more_fields_; }

    void has_more_fields(bool more_fields) { more_fields_ = more_fields; }

    std::size_t line_number() const { return line_; }

    unsigned column_number() const { return pos_; }

private:
    stream_type &is_;
    const char_type delim_;
    const char_type quote_;
    std::size_t line_;
    unsigned pos_;
    bool more_fields_;

private:
    basic_csv_istream(basic_csv_istream const &);
    basic_csv_istream &operator=(basic_csv_istream const &);

    template <typename T>
    basic_csv_istream &read_raw(T &dest);

    void read_non_escaped(string_type &dest);
    void read_escaped(string_type &dest);
    void next_line();
    char_type get_char();
    char_type peek_char();
    void skip_char();
    void put_back(char_type c);
    void read_ending(char_type c);
    void unexpected(char_type c);
    void unexpected_eof();
    bool is_eof(char_type);
};

template <typename Char, typename Traits>
basic_csv_istream<Char, Traits> &basic_csv_istream<Char, Traits>::
operator>>(string_type &dest) {
    dest.clear();

    const char_type c = peek_char();
    if (c == quote_) {
        read_escaped(dest);
    } else {
        read_non_escaped(dest);
    }
    return *this;
}

template <typename Char, typename Traits>
template <typename T>
basic_csv_istream<Char, Traits> &
basic_csv_istream<Char, Traits>::read_raw(T &dest) {
    const char_type c = peek_char();
    if (c == quote_) {
        skip_char();
        is_ >> dest;
        const char_type rest = get_char();
        if (rest != quote_) {
            unexpected(rest);
        }
    } else {
        is_ >> dest;
    }
    read_ending(get_char());
    return *this;
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::read_non_escaped(string_type &dest) {
    const char_type wlf = is_.widen(LF);
    const char_type wcr = is_.widen(CR);

    while (is_) {
        const char_type c = get_char();
        if (c == delim_) {
            more_fields_ = true;
            return;
        } else if (c == wcr) {
            const char_type look_ahead = peek_char();
            if (look_ahead == wlf) {
                skip_char();
            }
            next_line();
            return;
        } else if (c == wlf) {
            next_line();
            return;
        } else if (is_eof(c)) {
            more_fields_ = false;
            return;
        } else {
            dest.push_back(c);
        }
    }
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::read_escaped(string_type &dest) {
    skip_char(); // ignore starting quote
    while (is_) {
        const char_type c = get_char();

        if (c == quote_) {
            const char_type look_ahead = get_char();
            if (look_ahead == quote_) {
                // found a quoted quote
                dest.push_back(c);
            } else {
                read_ending(look_ahead);
                return;
            }
        } else if (is_eof(c)) {
            unexpected_eof();
        } else {
            dest.push_back(c);
        }
    }
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::read_ending(char_type c) {
    if (c == delim_) {
        more_fields_ = true;
    } else if (c == is_.widen(CR)) {
        const char_type look_ahead = get_char();
        if (look_ahead != is_.widen(LF)) {
            put_back(look_ahead);
        }
        next_line();
    } else if (c == is_.widen(LF)) {
        next_line();
    } else if (is_eof(c)) {
        more_fields_ = false;
    } else {
        unexpected(c);
    }
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::next_line() {
    line_ += 1;
    pos_ = 0;
    more_fields_ = false;
}

template <typename Char, typename Traits>
Char basic_csv_istream<Char, Traits>::get_char() {
    pos_ += 1;
    return is_.get();
}

template <typename Char, typename Traits>
Char basic_csv_istream<Char, Traits>::peek_char() {
    return is_.peek();
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::skip_char() {
    pos_ += 1;
    is_.ignore();
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::put_back(char_type c) {
    pos_ -= 1;
    is_.putback(c);
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::unexpected(char_type /* c */) {
    throw std::runtime_error("Unexpected character");
}

template <typename Char, typename Traits>
void basic_csv_istream<Char, Traits>::unexpected_eof() {
    throw std::runtime_error("Unexpected end of input");
}

template <typename Char, typename Traits>
bool basic_csv_istream<Char, Traits>::is_eof(char_type c) {
    return Traits::to_char_type(Traits::eof()) == c;
}
} // namespace csv
} // namespace text

#endif
