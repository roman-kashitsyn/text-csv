#ifndef TEXT_CSV_ITERATOR_HPP
#error "This header should not be included directly, use iterator.hpp instead"
#endif

namespace text {
namespace csv {

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

template < typename ValueType
         , typename Char
         , typename Traits
         >
void input_column_iterator<ValueType, Char, Traits>::advance()
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

template < typename ValueType
         , typename Char
         , typename Traits
         >
bool input_column_iterator<ValueType, Char, Traits>::equals(
        const input_column_iterator<ValueType, Char, Traits> &rhs) const
{
    if (is_ == 0 && rhs.is_ == 0) return true;
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
bool zipping_iterator<MapRow>::operator==(
        const zipping_iterator<MapRow> &rhs) const
{
    return this->row_ == rhs.row_ && this->pos_ == rhs.pos_;
}

template <typename MapRow>
bool zipping_iterator<MapRow>::operator!=(
        const zipping_iterator<MapRow> &rhs) const
{
    return !(*this == rhs);
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator++()
{
    return (*this) += 1;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator++(int)
{
    zipping_iterator<MapRow> copy(*this);
    (*this) += 1;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator--()
{
    return (*this) -= 1;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator--(int)
{
    zipping_iterator<MapRow> copy(*this);
    (*this) -= 1;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator+=(
        typename zipping_iterator<MapRow>::difference_type i)
{
    pos_ += i;
    load();
    return *this;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator+(
        typename zipping_iterator<MapRow>::difference_type i) const
{
    zipping_iterator<MapRow> copy(*this);
    copy += i;
    return copy;
}

template <typename MapRow>
zipping_iterator<MapRow> &zipping_iterator<MapRow>::operator-=(
        typename zipping_iterator<MapRow>::difference_type i)
{
    pos_ -= i;
    load();
    return *this;
}

template <typename MapRow>
zipping_iterator<MapRow> zipping_iterator<MapRow>::operator-(
        typename zipping_iterator<MapRow>::difference_type i) const
{
    zipping_iterator<MapRow> copy(*this);
    copy += i;
    return copy;
}

template <typename MapRow>
void zipping_iterator<MapRow>::load()
{
    if (pos_ < row_->size()) {
        value_.first = row_->name_of(pos_);
        value_.second = (*row_)[pos_];
    } else {
        value_.first.clear();
        value_.second.clear();
    }
}

} }
