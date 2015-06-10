#ifndef TEXT_CSV_ITERATOR_HPP
#define TEXT_CSV_ITERATOR_HPP

//          Copyright Roman Kashitsyn 2014 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "rows.hpp"
#include <utility>
#include <iterator>

namespace text {
namespace csv {

template <typename ValueType, typename Char = char,
          typename Traits = std::char_traits<Char> >
class input_column_iterator {
public:
    typedef basic_csv_istream<Char, Traits> istream_type;
    typedef ValueType value_type;
    typedef std::input_iterator_tag iterator_category;
    typedef const value_type &reference;
    typedef const value_type *pointer;
    typedef std::ptrdiff_t difference_type;

    input_column_iterator();

    input_column_iterator(istream_type &is);

    input_column_iterator &operator++() {
        advance();
        return *this;
    }

    input_column_iterator operator++(int) {
        input_column_iterator tmp = *this;
        advance();
        return tmp;
    }

    bool operator==(const input_column_iterator &rhs) const {
        return equals(rhs);
    }

    bool operator!=(const input_column_iterator &rhs) const {
        return !equals(rhs);
    }

    reference operator*() const { return value_; }

    pointer operator->() const { return &value_; }

private:
    void advance();
    bool equals(const input_column_iterator &rhs) const;

private:
    istream_type *is_;
    ValueType value_;
    bool pending_end_;
};

template <typename ValueType, typename Char = char,
          typename Traits = std::char_traits<Char> >
class output_column_iterator {
public:
    typedef basic_csv_ostream<Char, Traits> ostream_type;
    typedef std::output_iterator_tag iterator_category;
    typedef ValueType value_type;
    typedef void pointer;
    typedef void reference;
    typedef void difference_type;

    output_column_iterator(ostream_type &is) : is_(is) {}

    output_column_iterator &operator=(const value_type &value) {
        is_ << value;
        return *this;
    }

    output_column_iterator &operator*() { return *this; }

    output_column_iterator &operator++() { return *this; }

    output_column_iterator &operator++(int) { return *this; }

private:
    ostream_type &is_;
};

template <typename RangeType, typename RowType>
class input_row_iterator {
public:
    typedef RangeType range_type;

    typedef std::input_iterator_tag iterator_category;
    typedef RowType value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    input_row_iterator()
        : range_ptr_(0)
        , row_ptr_(0) {}

    input_row_iterator(range_type &range, value_type &row)
        : range_ptr_(&range)
        , row_ptr_(&row) {}

    input_row_iterator &operator++() {
        advance();
        return *this;
    }

    input_row_iterator operator++(int) {
        input_row_iterator copy = *this;
        advance();
        return copy;
    }

    bool operator==(const input_row_iterator &rhs) const {
        return range_ptr_ == rhs.range_ptr_;
    }

    bool operator!=(const input_row_iterator &rhs) const {
        return range_ptr_ != rhs.range_ptr_;
    }

    reference operator*() const { return *row_ptr_; }

    pointer operator->() const { return row_ptr_; }

private:
    void advance() {
        if (range_ptr_->empty()) {
            range_ptr_ = 0;
            row_ptr_ = 0;
        } else {
            range_ptr_->move_next();
        }
    }

    range_type *range_ptr_;
    pointer row_ptr_;
};

template <typename Char, typename Traits = std::char_traits<Char> >
class basic_row_range {
public:
    typedef basic_row<Char, Traits> row_type;
    typedef basic_csv_istream<Char, Traits> stream_type;

    typedef input_row_iterator<basic_row_range, row_type> iterator;

    basic_row_range(std::basic_istream<Char, Traits> &in)
        : is_(in)
        , started_(false) {}

    iterator begin() {
        if (!started_) {
            started_ = true;
            move_next();
        }
        return iterator(*this, last_row_);
    }

    iterator end() { return iterator(); }

    bool empty() { return !is_; }

    void move_next() { is_ >> last_row_; }

private:
    stream_type is_;
    row_type last_row_;
    bool started_;
};

template <typename Char, typename Traits = std::char_traits<Char> >
class basic_map_row_range {
public:
    typedef basic_map_row<Char, Traits> row_type;
    typedef basic_csv_istream<Char, Traits> stream_type;
    typedef basic_header<Char, Traits> header_type;
    typedef input_row_iterator<basic_map_row_range, row_type> iterator;

    basic_map_row_range(std::basic_istream<Char, Traits> & in)
        : is_(in)
        , header_(is_)
        , last_row_(header_)
        , started_(false)
    {}

    iterator begin() {
        if (!started_) {
            started_ = true;
            move_next();
        }
        return iterator(*this, last_row_);
    }

    iterator end() { return iterator(); }

    bool empty() { return !is_; }

    void move_next() { is_ >> last_row_; }

private:
    stream_type is_;
    header_type header_;
    row_type last_row_;
    bool started_;
};

template <typename MapRow>
class zipping_iterator {
    typedef typename MapRow::value_type string_type;

public:
    typedef std::pair<string_type, string_type> value_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::random_access_iterator_tag iterator_category;

    zipping_iterator(const MapRow &row, std::size_t pos);

    pointer operator->() { return &value_; }

    reference operator*() { return value_; }

    bool operator==(const zipping_iterator &) const;
    bool operator!=(const zipping_iterator &) const;

    zipping_iterator &operator++();
    zipping_iterator operator++(int);

    zipping_iterator &operator--();
    zipping_iterator operator--(int);

    zipping_iterator &operator+=(difference_type);
    zipping_iterator operator+(difference_type) const;

    zipping_iterator &operator-=(difference_type);
    zipping_iterator operator-(difference_type) const;

private:
    void load();

private:
    const MapRow *row_;
    value_type value_;
    std::size_t pos_;
};

template <typename MapRow>
zipping_iterator<MapRow> pairs_begin(const MapRow &row) {
    return zipping_iterator<MapRow>(row, 0);
}

template <typename MapRow>
zipping_iterator<MapRow> pairs_end(const MapRow &row) {
    return zipping_iterator<MapRow>(row, row.size());
}

typedef basic_row_range<char> row_range;
typedef basic_map_row_range<char> map_row_range;
typedef basic_row_range<wchar_t> wrow_range;

// Implementation

template < typename ValueType
         , typename Char
         , typename Traits
         >
input_column_iterator<ValueType, Char, Traits>::input_column_iterator()
    : is_(0)
    , value_()
    , pending_end_(false)
{}

template < typename ValueType
         , typename Char
         , typename Traits
         >
input_column_iterator<ValueType, Char, Traits>::input_column_iterator(
        basic_csv_istream<Char, Traits> &is)
    : is_(&is)
    , value_()
    , pending_end_(false)
{
    advance();
}

template <typename ValueType, typename Char, typename Traits>
void input_column_iterator<ValueType, Char, Traits>::advance() {
    if (pending_end_) {
        is_ = 0;
        return;
    }

    if (is_) {
        *is_ >> value_;
        if (!is_->has_more_fields()) {
            pending_end_ = true;
        }
    }
}

template <typename ValueType, typename Char, typename Traits>
bool input_column_iterator<ValueType, Char, Traits>::equals(
    const input_column_iterator<ValueType, Char, Traits> &rhs) const {
    if (is_ == 0 && rhs.is_ == 0)
        return true;
    return is_ == rhs.is_ && value_ == rhs.value_;
}

template <typename MapRow>
zipping_iterator<MapRow>::zipping_iterator(const MapRow &row, std::size_t pos)
    : row_(&row)
    , value_()
    , pos_(pos)
{
    load();
}

template <typename MapRow>
bool zipping_iterator<MapRow>::
operator==(const zipping_iterator<MapRow> &rhs) const {
    return this->row_ == rhs.row_ && this->pos_ == rhs.pos_;
}

template <typename MapRow>
bool zipping_iterator<MapRow>::
operator!=(const zipping_iterator<MapRow> &rhs) const {
    return !(*this == rhs);
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator++() {
    return (*this) += 1;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator++(int) {
    zipping_iterator<MapRow> copy(*this);
    (*this) += 1;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator--() {
    return (*this) -= 1;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator--(int) {
    zipping_iterator<MapRow> copy(*this);
    (*this) -= 1;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::
operator+=(typename zipping_iterator<MapRow>::difference_type i) {
    pos_ += i;
    load();
    return *this;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::
operator+(typename zipping_iterator<MapRow>::difference_type i) const {
    zipping_iterator<MapRow> copy(*this);
    copy += i;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::
operator-=(typename zipping_iterator<MapRow>::difference_type i) {
    pos_ -= i;
    load();
    return *this;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::
operator-(typename zipping_iterator<MapRow>::difference_type i) const {
    zipping_iterator<MapRow> copy(*this);
    copy += i;
    return copy;
}

template <typename MapRow>
void zipping_iterator<MapRow>::load() {
    if (pos_ < row_->size()) {
        value_.first = row_->name_of(pos_);
        value_.second = (*row_)[pos_];
    } else {
        value_.first.clear();
        value_.second.clear();
    }
}
} // namespace csv
} // namespace text

#endif
