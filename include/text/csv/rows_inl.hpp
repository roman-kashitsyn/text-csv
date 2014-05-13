#ifndef TEXT_CSV_ROWS_HPP
#error "This header should not be included directly, use rows.hpp instead"
#endif

#include <sstream>
#include <utility>

namespace text {
namespace csv {

/**
 * @brief Helper std::less-like class to sort and search in a map
 * implemented as vector of pairs.
 */
template <typename Char, typename Traits>
struct basic_header<Char, Traits>::by_key
{
    bool operator()(const assoc & lhs, const assoc & rhs) const
    {
        return lhs.first < rhs.first;
    }

    bool operator()(const assoc & a, const key_type & key) const
    {
        return a.first < key;
    }

    bool operator()(const assoc & a, const char_type * key) const
    {
        return a.first < key;
    }

    bool operator()(const key_type & key, const assoc & a) const
    {
        return key < a.first;
    }

    bool operator()(const char_type * key, const assoc & a) const
    {
        return key < a.first;
    }
};

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header()
{}

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header(basic_csv_istream<Char, Traits>& is)
{
    row_type tmp_row;
    is >> tmp_row;
    assign(tmp_row);
}

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header(const row_type& row)
{
    assign(row);
}

template <typename Char, typename Traits>
void basic_header<Char, Traits>::assign(const row_type& row)
{
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
const typename basic_header<Char, Traits>::key_type&
basic_header<Char, Traits>::name_of(std::size_t i) const
{
    return assocs_[r_index_[i]].first;
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const key_type& key) const
{
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return i != assocs_.end() ? i->second : npos;
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const char_type* key) const
{
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return i != assocs_.end() ? i->second : npos;
}

template <typename Char, typename Traits>
basic_row<Char, Traits>::basic_row(std::size_t n) : base(n)
{}

template <typename Char, typename Traits>
basic_row<Char, Traits>::basic_row(basic_csv_istream<Char, Traits> &is)
{
    is >> *this;
}

template <typename Char, typename Traits>
template <typename T>
T basic_row<Char, Traits>::as(std::size_t pos) const
{
    T sink;
    std::basic_stringstream<Char, Traits> s((*this)[pos]);
    s >> sink;
    return sink;
}

template <typename Char, typename Traits>
bool basic_row<Char, Traits>::operator==(
        const basic_row<Char, Traits> &rhs) const
{
    return static_cast<const base &>(*this) ==
           static_cast<const base &>(rhs);
}

template <typename Char, typename Traits>
void basic_row<Char, Traits>::clear()
{
    for (iterator i = begin(), e = end(); i != e; ++i) {
        i->clear();
    }
}

template <typename Char, typename Traits>
basic_csv_ostream<Char, Traits> &
operator<<(basic_csv_ostream<Char, Traits> & os,
           const basic_row<Char, Traits> & row)
{
    for (std::size_t i = 0, n = row.size(); i < n; ++i) {
        os << row[i];
    }
    os.next_line();
    return os;
}

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits> &
operator<<(std::basic_ostream<Char, Traits> & os,
           const basic_row<Char, Traits> & row)
{
    os << "row{";
    for (std::size_t i = 0, n = row.size(); i < n; ++i) {
        if (i != 0) os << ",";
        os << "{" << row[i] << "}";
    }
    os << "}";
    return os;
}

template <typename Char, typename Traits>
basic_csv_istream<Char, Traits> &
operator>>(basic_csv_istream<Char, Traits> & is,
           basic_row<Char, Traits> & row)
{

    row.clear();

    const std::size_t size = row.size();
    std::size_t i = 0;
    typename basic_row<Char, Traits>::value_type field;

    while (is && is.has_more_fields() && i < size) {
        is >> field;
        row[i++] = field;
    }

    while (is && is.has_more_fields()) {
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
    , header_(std::move(header))
{}

#else

template <typename Char, typename Traits>
basic_map_row<Char, Traits>::basic_map_row(
        const typename basic_map_row<Char, Traits>::header_type &header)
    : basic_map_row<Char, Traits>::base(header.size())
    , header_(header)
{}

#endif

template <typename Char, typename Traits>
basic_map_row<Char, Traits>::basic_map_row(
        basic_csv_istream<Char, Traits> &is)
    : header_(is)
{
    is >> *this;
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](int i)
{
    return (*this)[std::size_t(i)];
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](int i) const
{
    return (*this)[std::size_t(i)];
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](
        const typename basic_map_row<Char, Traits>::key_type &key)
{
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](
        const typename basic_map_row<Char, Traits>::key_type &key) const
{
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](
        const typename basic_map_row<Char, Traits>::char_type *key)
{
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
const typename basic_map_row<Char, Traits>::value_type &
basic_map_row<Char, Traits>::operator[](
        const typename basic_map_row<Char, Traits>::char_type *key) const
{
    return at(header_.index_of(key));
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::const_iterator
basic_map_row<Char, Traits>::find(
        const typename basic_map_row<Char, Traits>::key_type &key) const
{
    const std::size_t idx = header_.index_of(key);
    return idx != header_type::npos ? base::begin() + idx : base::end();
}

template <typename Char, typename Traits>
typename basic_map_row<Char, Traits>::const_iterator
basic_map_row<Char, Traits>::find(
        const typename basic_map_row<Char, Traits>::char_type *key) const
{
    const std::size_t idx = header_.index_of(key);
    return idx != header_type::npos ? base::begin() + idx : base::end();
}

template <typename Char, typename Traits>
template <typename T>
T basic_map_row<Char, Traits>::as(
        const typename basic_map_row<Char, Traits>::key_type &key) const
{
    T sink;
    std::basic_stringstream<Char, Traits> is((*this)[key]);
    is >> sink;
    return sink;
}

template <typename Char, typename Traits>
template <typename T>
T basic_map_row<Char, Traits>::as(
        const typename basic_map_row<Char, Traits>::char_type *key) const
{
    T sink;
    std::basic_stringstream<Char, Traits> is((*this)[key]);
    is >> sink;
    return sink;
}

template <typename Char, typename Traits>
bool basic_map_row<Char, Traits>::has_key(
        const basic_map_row<Char, Traits>::key_type &key) const
{
    return header_.index_of(key) != header_type::npos;
}

} }
