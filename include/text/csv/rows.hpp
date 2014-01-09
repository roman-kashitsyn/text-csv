#ifndef TEXT_CSV_LINES_HPP
#define TEXT_CSV_LINES_HPP

#include "text/csv/ostream.hpp"
#include "text/csv/istream.hpp"

#include <algorithm>
#include <vector>
#include <string>

namespace text {
namespace csv {

/**
 * @brief Represents a single line of a CSV file.
 * @author Roman Kashitsyn
 */
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_row
{
public:
    typedef Char char_type;
    typedef Traits traits_type;
    typedef std::basic_string<Char, Traits> field_type;
    typedef std::vector<field_type> fields_type;
    typedef field_type value_type;
    typedef typename fields_type::iterator iterator;
    typedef typename fields_type::const_iterator const_iterator;

    explicit basic_row(std::size_t n = 0)
        : fields_(n)
    {}

    field_type & at(std::size_t i)
    { return fields_.at(i); }

    const field_type & at(std::size_t i) const
    { return fields_.at(i); }

    const field_type & at(int i) const
    { return fields_.at(i); }

    field_type & operator[](int i)
    { return fields_[i]; }

    field_type & operator[](std::size_t i)
    { return fields_[i]; }

    const field_type & operator[](int i) const
    { return fields_[i]; }

    const field_type & operator[](std::size_t i) const
    { return fields_[i]; }

    bool operator==(const basic_row & rhs) const
    { return fields_ == rhs.fields_; }

    bool operator!=(const basic_row & rhs) const
    { return fields_ != rhs.fields_; }

    iterator begin()
    { return fields_.begin(); }

    iterator end()
    { return fields_.end(); }

    const_iterator cbegin() const
    { return fields_.begin(); }

    const_iterator begin() const
    { return fields_.begin(); }

    const_iterator cend() const
    { return fields_.end(); }

    const_iterator end() const
    { return fields_.end(); }

    std::size_t size() const
    { return fields_.size(); }

    void resize(std::size_t n)
    { fields_.resize(n); }

    basic_row & push_back(const field_type & field)
    { fields_.push_back(field); return *this; }

    void clear()
    {
        for (iterator i = begin(), e = end(); i != e; ++i) {
            i->clear();
        }
    }

private:
    fields_type fields_;
};

/**
 * @brief Represents the header of a CSV file.
 *
 * @details The class maintains a simple string -> index map based on
 * a sorted vector. Cost of column index lookup is O(log(N)) where
 * N is the column count.
 *
 * @author Roman Kashitsyn
 */
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_header
{
public:
    typedef Char char_type;
    typedef basic_row<Char, Traits> row_type;
    typedef std::basic_string<Char, Traits> key_type;

    static const std::size_t npos = std::size_t(-1);

    basic_header();
    basic_header(basic_csv_istream<Char, Traits> & is);
    basic_header(const row_type & row);

    void assign(const row_type & row);

    std::size_t index_of(const char_type * key) const;

    std::size_t index_of(const key_type & key) const;

    std::size_t size() const
    { return assocs_.size(); }

private:
    struct by_key;

    typedef std::pair<key_type, int> assoc;
    typedef std::vector<assoc> assocs;

    assocs assocs_;
};

template <typename Char, typename Traits>
basic_header<Char, Traits>::basic_header()
{}

template <typename Char, typename Traits>
basic_header<Char, Traits>::
basic_header(basic_csv_istream<Char, Traits> & is)
{
    row_type tmp_row;
    is >> tmp_row;
    assign(tmp_row);
}

template <typename Char, typename Traits>
basic_header<Char, Traits>::
basic_header(const row_type & row)
{
    assign(row);
}

template <typename Char, typename Traits>
void basic_header<Char, Traits>::assign(const row_type & row)
{
    if (!assocs_.empty()) assocs_.clear();

    assocs_.reserve(row.size());
    for (std::size_t i = 0, n = row.size(); i < n; ++i) {
        assocs_.push_back(assoc(row[i], i));
    }
    std::sort(assocs_.begin(), assocs_.end(), by_key());
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const key_type & key) const
{
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return i != assocs_.end() ? i->second : npos;
}

template <typename Char, typename Traits>
std::size_t basic_header<Char, Traits>::index_of(const char_type * key) const
{
    typename assocs::const_iterator i =
        std::lower_bound(assocs_.begin(), assocs_.end(), key, by_key());
    return i != assocs_.end() ? i->second : npos;
}

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

/**
 * @brief Extension of thes basic_row supporting string keys.
 * @author Roman Kashitsyn
 */
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_map_row : public basic_row<Char, Traits>
{
public:
    typedef basic_header<Char, Traits> header_type;
    typedef basic_row<Char, Traits> parent;
    typedef typename parent::field_type field_type;
    typedef typename parent::char_type char_type;
    typedef typename parent::iterator iterator;
    typedef typename parent::const_iterator const_iterator;
    typedef field_type key_type;

    using parent::operator[];
    using parent::at;
    using parent::begin;
    using parent::end;
    using parent::cbegin;
    using parent::cend;

    basic_map_row(const header_type & header)
        : parent(header.size())
        , header_(header)
    {}

    field_type & operator[](const key_type & key)
    {
        return at(header_.index_of(key));
    }

    field_type & operator[](const char_type * key)
    {
        return at(header_.index_of(key));
    }

    const field_type & operator[](const key_type & key) const
    {
        return at(header_.index_of(key));
    }

    const field_type & operator[](const char_type * key) const
    {
        return at(header_.index_of(key));
    }

    const_iterator find(const key_type & key) const
    {
        const std::size_t idx = header_.index_of(key);
        return idx != header_type::npos ? parent::begin() + idx : parent::end();
    }

    const_iterator find(const char_type * key) const
    {
        const std::size_t idx = header_.index_of(key);
        return idx != header_type::npos ? parent::begin() + idx : parent::end();
    }

    bool has_key(const key_type & key) const
    {
        return header_.index_of(key) != header_type::npos;
    }

private:
    const header_type & header_;
};

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
    typename basic_row<Char, Traits>::field_type field;

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

typedef basic_row<char> row;
typedef basic_row<wchar_t> wrow;
typedef basic_header<char> header;
typedef basic_header<wchar_t> wheader;
typedef basic_map_row<char> map_row;
typedef basic_map_row<wchar_t> map_wrow;

} }

#endif
