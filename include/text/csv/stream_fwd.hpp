#pragma once

#include <iosfwd>

// Forward declarations for csv streams

namespace text {
namespace csv {

template < typename Char, typename Traits = std::char_traits<Char> >
class basic_csv_istream;

template < typename Char, typename Traits = std::char_traits<Char> >
class basic_csv_ostream;

typedef basic_csv_istream<char> csv_istream;
typedef basic_csv_ostream<char> csv_ostream;

typedef basic_csv_istream<wchar_t> csv_wistream;
typedef basic_csv_ostream<wchar_t> csv_wostream;

const char COMMA = ',';
const char CR = '\r';
const char LF = '\n';
const char QUOTE = '"';

} }
