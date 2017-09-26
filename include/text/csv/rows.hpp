#ifndef TEXT_CSV_ROWS_HPP
#define TEXT_CSV_ROWS_HPP

//          Copyright Roman Kashitsyn 2014 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "ostream.hpp"
#include "istream.hpp"

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <utility>

namespace text {
namespace csv {

/**
 * @brief Represents a single row of a CSV file.
 */
template <typename Char, typename Traits = std::char_traits<Char> >
class basic_row : private std::vector<std::basic_string<Char, Traits> > {
    typedef std::vector<std::basic_string<Char, Traits> > base;

public:
    typedef typename base::value_type value_type;

    typedef Char char_type;
    typedef Traits traits_type;
    typedef typename base::iterator iterator;
    typedef typename base::const_iterator const_iterator;

    using base::at;
    using base::operator[];
    using base::begin;
    using base::end;
    using base::size;
    using base::resize;
    using base::push_back;

    explicit basic_row(std::size_t n = 0);
    explicit basic_row(basic_csv_istream<Char, Traits> &is);

    bool operator==(const basic_row &rhs) const;

    bool operator!=(const basic_row &rhs) const { return !(*this == rhs); }

    template <typename T>
    T as(std::size_t pos) const;

    const_iterator cbegin() const { return begin(); }

    const_iterator cend() const { return end(); }

    void clear();
};

/// @brief Represents the header of a CSV file.
///
/// @details The class maintains a simple string -> index map based on
/// a sorted vector. Cost of column index lookup is O(log(N)) where
/// N is the column count.
template <typename Char, typename Traits = std::char_traits<Char> >
class basic_header {
    typedef std::vector<std::size_t> reverse_index;

public:
    typedef Char char_type;
    typedef basic_row<Char, Traits> row_type;
    typedef std::basic_string<Char, Traits> key_type;

    static const std::size_t npos;

    basic_header();
    basic_header(basic_csv_istream<Char, Traits> &is);
    basic_header(const row_type &row);

    /// @brief (Re)Initializes header with given row.
    void assign(const row_type &row);

    /// @brief Returns name of the column with index <tt>i</tt>.
    const key_type &name_of(std::size_t i) const;

    /// @brief Returns index of the column with name <tt>key</tt>.
    std::size_t index_of(const char_type *key) const;

    /// @brief Returns index of the column with name <tt>key</tt>.
    std::size_t index_of(const key_type &key) const;

    /// @brief Returns number of columns.
    std::size_t size() const { return assocs_.size(); }

private:
    struct by_key;
    typedef std::pair<key_type, int> assoc;
    typedef std::vector<assoc> assocs;

private:
    assocs assocs_;
    reverse_index r_index_;
};

template <typename Char, typename Traits>
const size_t basic_header<Char, Traits>::npos = static_cast<std::size_t>(-1);

/// @brief Extension of thes basic_row supporting string keys.
template <typename Char, typename Traits = std::char_traits<Char> >
class basic_map_row : public basic_row<Char, Traits> {
    typedef basic_row<Char, Traits> base;

public:
    typedef basic_header<Char, Traits> header_type;
    typedef typename base::value_type value_type;
    typedef typename base::char_type char_type;
    typedef typename base::iterator iterator;
    typedef typename base::const_iterator const_iterator;
    typedef value_type key_type;

    using base::as;
    using base::operator[];
    using base::at;
    using base::begin;
    using base::end;
    using base::cbegin;
    using base::cend;
    using base::size;

#if __cplusplus >= 201103
    basic_map_row(header_type header);
#else
    basic_map_row(const header_type &header);
#endif
    basic_map_row(basic_csv_istream<Char, Traits> &is);

    value_type &operator[](int i);
    const value_type &operator[](int i) const;

    value_type &operator[](const key_type &key);
    const value_type &operator[](const key_type &key) const;

    value_type &operator[](const char_type *key);
    const value_type &operator[](const char_type *key) const;

    const_iterator find(const key_type &key) const;
    const_iterator find(const char_type *key) const;

    template <typename T>
    T as(const key_type &key) const;

    template <typename T>
    T as(const char_type *key) const;

    const key_type &name_of(std::size_t i) const { return header_.name_of(i); }
    bool has_key(const key_type &key) const;

private:
    const header_type header_;
};

typedef basic_row<char> row;
typedef basic_row<wchar_t> wrow;
typedef basic_header<char> header;
typedef basic_header<wchar_t> wheader;
typedef basic_map_row<char> map_row;
typedef basic_map_row<wchar_t> map_wrow;

// Implementation
/**
 * @brief Helper std::less-like class to sort and search in a map
 * implemented as vector of pairs.
 */
template <typename Char, typename Traits>
struct basic_header<Char, Traits>::by_key {
    bool operator()(const assoc &lhs, const assoc &rhs) const {
        return lhs.first < rhs.first;
    }

    bool operator()(const assoc &a, const key_type &key) const {
        return a.first < key;
    }

    bool operator()(const assoc &a, const char_type *key) const {
        return a.first < key;
    }

    bool operator()(const key_type &key, const assoc &a) const {
        return key < a.first;
    }

    bool operator()(const char_type *key, const assoc &a) const {
        return key < a.first;
    }
};

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header() {}

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header(basic_csv_istream<Char, Traits> &is) {
    row_type tmp_row;
    is >> tmp_row;
    assign(tmp_row);
}

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header(const row_type &row) {
    assign(row);
}

template <typename Char, typename Traits>
void basic_header<Char, Traits>::assign(const row_type &row) {
    if (!assocs_.empty()) {
        assocs_.clear();
        r_index_.clear();
    }

    const std::size_t n = row.size();

    assocs_.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
#if __cplusplus >= 201103
        assocs_.emplace_back(row[i], i);
#else
        assocs_.push_back(assoc(row[i], i));
#endif
    }
    std::sort(assocs_.begin(), assocs_.end(), by_key());

    r_index_.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        r_index_[assocs_[i].second] = i;
    }
}

template <typename Char, typename Traits>
const typename basic_header<Char, Traits>::key_type &
basic_header<Char, Traits>::name_of(std::size_t i) const {
    return assocs_[r_index_[i]].first;
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const key_type &key) const {
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return (i != assocs_.end() && i->first == key) ? i->second : npos;
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const char_type *key) const {
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return (i != assocs_.end() && i->first == key) ? i->second : npos;
}

template <typename Char, typename Traits>
basic_row<Char, Traits>::basic_row(std::size_t n)
    : base(n) {}

template <typename Char, typename Traits>
basic_row<Char, Traits>::basic_row(basic_csv_istream<Char, Traits> &is) {
    is >> *this;
}

template <typename Char, typename Traits>
template <typename T>
T basic_row<Char, Traits>::as(std::size_t pos) const {
    T sink;
    std::basic_stringstream<Char, Traits> s((*this)[pos]);
    s >> sink;
    return sink;
}

template <typename Char, typename Traits>
bool basic_row<Char, Traits>::
operator==(const basic_row<Char, Traits> &rhs) const {
    return static_cast<const base &>(*this) == static_cast<const base &>(rhs);
}

template <typename Char, typename Traits>
void basic_row<Char, Traits>::clear() {
    for (iterator i = begin(), e = end(); i != e; ++i) {
        i->clear();
    }
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &operator<<(
    basic_csv_ostream<Char, Traits> &os, const basic_row<Char, Traits> &row) {
    for (std::size_t i = 0, n = row.size(); i < n; ++i) {
        os << row[i];
    }
    os.end_line();
    return os;
}

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits> &operator<<(
    std::basic_ostream<Char, Traits> &os, const basic_row<Char, Traits> &row) {
    os << "row{";
    for (std::size_t i = 0, n = row.size(); i < n; ++i) {
        if (i != 0)
            os << ",";
        os << "{" << row[i] << "}";
    }
    os << "}";
    return os;
}

template <typename Char, typename Traits>
basic_csv_istream<Char, Traits> &operator>>(basic_csv_istream<Char, Traits> &is,
                                            basic_row<Char, Traits> &row) {

    row.clear();

    const std::size_t size = row.size();
    std::size_t i = 0;
    typename basic_row<Char, Traits>::value_type field;

    while (is.good() && is.has_more_fields() && i < size) {
        is >> field;
        row[i++] = field;
    }

    while (is.good() && is.has_more_fields()) {
        is >> field;
        row.push_back(field);
        ++i;
    }

    is.has_more_fields(true);

    row.resize(i);

    return is;
}

#if __cplusplus >= 201103

template <typename Char, typename Traits>
basic_map_row<Char, Traits>::basic_map_row(
    typename basic_map_row<Char, Traits>::header_type header)
    : basic_map_row<Char, Traits>::base(header.size())
    , header_(std::move(header)) {}

#else

template <typename Char, typename Traits>
basic_map_row<Char, Traits>::basic_map_row(
    const typename basic_map_row<Char, Traits>::header_type &header)
    : basic_map_row<Char, Traits>::base(header.size())
    , header_(header) {}

#endif

template <typename Char, typename Traits>
basic_map_row<Char, Traits>::basic_map_row(basic_csv_istream<Char, Traits> &is)
    : header_(is) {
    is >> *this;
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &basic_map_row<Char, Traits>::
operator[](int i) {
    return (*this)[std::size_t(i)];
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::
operator[](int i) const {
    return (*this)[std::size_t(i)];
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &basic_map_row<Char, Traits>::
operator[](const typename basic_map_row<Char, Traits>::key_type &key) {
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::
operator[](const typename basic_map_row<Char, Traits>::key_type &key) const {
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &basic_map_row<Char, Traits>::
operator[](const typename basic_map_row<Char, Traits>::char_type *key) {
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::
operator[](const typename basic_map_row<Char, Traits>::char_type *key) const {
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::const_iterator
basic_map_row<Char, Traits>::find(
    const typename basic_map_row<Char, Traits>::key_type &key) const {
    const std::size_t idx = header_.index_of(key);
    return idx != header_type::npos ? base::begin() + idx : base::end();
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::const_iterator
basic_map_row<Char, Traits>::find(
    const typename basic_map_row<Char, Traits>::char_type *key) const {
    const std::size_t idx = header_.index_of(key);
    return idx != header_type::npos ? base::begin() + idx : base::end();
}

template <typename Char, typename Traits>
template <typename T>
T basic_map_row<Char, Traits>::as(
    const typename basic_map_row<Char, Traits>::key_type &key) const {
    T sink;
    std::basic_stringstream<Char, Traits> is((*this)[key]);
    is >> sink;
    return sink;
}

template <typename Char, typename Traits>
template <typename T>
T basic_map_row<Char, Traits>::as(
    const typename basic_map_row<Char, Traits>::char_type *key) const {
    T sink;
    std::basic_stringstream<Char, Traits> is((*this)[key]);
    is >> sink;
    return sink;
}

template <typename Char, typename Traits>
bool basic_map_row<Char, Traits>::has_key(
    const basic_map_row<Char, Traits>::key_type &key) const {
    return header_.index_of(key) != header_type::npos;
}
} // namespace csv
} // namespace text

#endif
