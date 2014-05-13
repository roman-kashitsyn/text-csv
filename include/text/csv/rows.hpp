#ifndef TEXT_CSV_ROWS_HPP
#define TEXT_CSV_ROWS_HPP

#include "text/csv/ostream.hpp"
#include "text/csv/istream.hpp"

#include <algorithm>
#include <vector>
#include <string>

namespace text {
namespace csv {

/**
 * @brief Represents a single row of a CSV file.
 */
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_row : private std::vector< std::basic_string<Char, Traits> >
{
    typedef std::vector< std::basic_string<Char, Traits> > base;
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

    bool operator==(const basic_row & rhs) const;

    bool operator!=(const basic_row & rhs) const { return !(*this == rhs); }

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
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_header
{
    typedef std::vector<std::size_t> reverse_index;
public:
    typedef Char char_type;
    typedef basic_row<Char, Traits> row_type;
    typedef std::basic_string<Char, Traits> key_type;

    static const std::size_t npos = std::size_t(-1);

    basic_header();
    basic_header(basic_csv_istream<Char, Traits> & is);
    basic_header(const row_type & row);

    /// @brief (Re)Initializes header with given row.
    void assign(const row_type & row);

    /// @brief Returns name of the column with index <tt>i</tt>.
    const key_type &name_of(std::size_t i) const;

    /// @brief Returns index of the column with name <tt>key</tt>.
    std::size_t index_of(const char_type * key) const;

    /// @brief Returns index of the column with name <tt>key</tt>.
    std::size_t index_of(const key_type & key) const;

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

/// @brief Extension of thes basic_row supporting string keys.
template < typename Char, typename Traits = std::char_traits<Char> >
class basic_map_row : public basic_row<Char, Traits>
{
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

    value_type & operator[](const key_type &key);
    const value_type & operator[](const key_type &key) const;

    value_type & operator[](const char_type *key);
    const value_type & operator[](const char_type *key) const;

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

} }

#include "text/csv/rows_inl.hpp"

#endif
