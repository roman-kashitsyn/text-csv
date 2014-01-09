#ifndef TEXT_CSV_ITERATOR_HPP
#define TEXT_CSV_ITERATOR_HPP

#include "text/csv/rows.hpp"

namespace text {
namespace csv {

template < typename ValueType,
           typename Char = char,
           typename Traits = std::char_traits<Char> >
class input_column_iterator
{
public:
    typedef basic_csv_istream<Char, Traits> istream_type;
    typedef ValueType value_type;
    typedef std::input_iterator_tag iterator_category;
    typedef const value_type & reference;
    typedef const value_type * pointer;
    typedef ptrdiff_t difference_type;

    input_column_iterator()
        : is_(0)
        , value_()
        , pending_end_(false)
    {}

    input_column_iterator(istream_type & is)
        : is_(&is)
        , value_()
        , pending_end_(false)
    {
        advance();
    }

    input_column_iterator & operator++()
    {
        advance();
        return *this;
    }

    input_column_iterator operator++(int)
    {
        input_column_iterator tmp = *this;
        advance();
        return tmp;
    }

    bool operator==(const input_column_iterator & rhs) const
    { return equals(rhs); }

    bool operator!=(const input_column_iterator & rhs) const
    { return !equals(rhs); }

    reference operator*() const
    { return value_; }

    pointer operator->() const
    { return &value_; }

private:
    void advance()
    {
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

    bool equals(const input_column_iterator & rhs) const
    {
        if (is_ == 0 && rhs.is_ == 0) return true;
        return is_ == rhs.is_ && value_ == rhs.value_;
    }

    istream_type * is_;
    ValueType value_;
    bool pending_end_;
};

template < typename ValueType,
           typename Char = char,
           typename Traits = std::char_traits<Char>
           >
class output_column_iterator
{
public:
    typedef basic_csv_ostream<Char, Traits> ostream_type;
    typedef std::output_iterator_tag iterator_category;
    typedef ValueType value_type;
    typedef void pointer;
    typedef void reference;
    typedef void difference_type;

    output_column_iterator(ostream_type & is)
        : is_(is)
    {}

    output_column_iterator & operator=(const value_type & value)
    { is_ << value; return *this; }

    output_column_iterator & operator*()
    { return *this; }

    output_column_iterator & operator++()
    { return *this; }

    output_column_iterator & operator++(int)
    { return *this; }

private:
    ostream_type & is_;
};

template < typename RangeType, typename RowType >
class input_row_iterator
{
public:
    typedef RangeType range_type;

    typedef std::input_iterator_tag iterator_category;
    typedef RowType value_type;
    typedef const value_type * pointer;
    typedef const value_type & reference;

    input_row_iterator()
        : range_ptr_(0)
        , row_ptr_(0)
    {}

    input_row_iterator(range_type & range, value_type & row)
        : range_ptr_(&range)
        , row_ptr_(&row)
    {}

    input_row_iterator & operator++()
    {
        advance();
        return *this;
    }

    input_row_iterator operator++(int)
    {
        input_row_iterator copy = *this;
        advance();
        return copy;
    }

    bool operator==(const input_row_iterator & rhs) const
    { return range_ptr_ == rhs.range_ptr_; }

    bool operator!=(const input_row_iterator & rhs) const
    { return range_ptr_ != rhs.range_ptr_; }

    reference operator*() const
    { return *row_ptr_; }

    pointer operator->() const
    { return row_ptr_;  }

private:
    void advance()
    {
        if (range_ptr_->empty()) {
            range_ptr_ = 0;
            row_ptr_ = 0;
        } else {
            range_ptr_->move_next();
        }
    }

    range_type * range_ptr_;
    pointer row_ptr_;
};

template < typename Char, typename Traits = std::char_traits<Char> >
class basic_row_range
{
public:
    typedef basic_row<Char, Traits> row_type;
    typedef basic_csv_istream<Char, Traits> stream_type;

    typedef input_row_iterator<basic_row_range, row_type> iterator;

    basic_row_range(std::basic_istream<Char, Traits> & in)
        : is_(in)
        , started_(false)
    {}

    iterator begin()
    {
        if (!started_) {
            started_ = true;
            move_next();
        }
        return iterator(*this, last_row_);
    }

    iterator end()
    { return iterator(); }

    bool empty()
    { return !is_; }

    void move_next()
    { is_ >> last_row_; }

private:
    stream_type is_;
    row_type last_row_;
    bool started_;
};

template < typename Char, typename Traits = std::char_traits<Char> >
class basic_map_row_range
{
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

    iterator begin()
    {
        if (!started_) {
            started_ = true;
            move_next();
        }
        return iterator(*this, last_row_);
    }

    iterator end()
    { return iterator(); }

    bool empty()
    { return !is_; }

    void move_next()
    { is_ >> last_row_; }

private:
    stream_type is_;
    header_type header_;
    row_type last_row_;
    bool started_;
};

typedef basic_row_range<char> row_range;
typedef basic_map_row_range<char> map_row_range;
typedef basic_row_range<wchar_t> wrow_range;

} }

#endif
