C++ CSV read/write library
==========================

[![Build Status](https://travis-ci.org/roman-kashitsyn/text-csv.svg)](https://travis-ci.org/roman-kashitsyn/text-csv)

Properties:

* RFC-4180 compliant
* STL-friendly interface: streams, iterators, ranges
* Works with any character set (`char`, `wchar_t`, etc)
* Header-only: no build required, just include files
* Supports various delimiters and quote characters
* Able to handle named columns
* No external dependencies
* C++11-ready (range-based for and movement semantics) but compiles cleanly
  with C++03.

Examples:
=========

* Writing csv files:

```cpp
    #include <fstream>
    #include "text/csv/ostream.hpp"
    
    namespace csv = ::text::csv;
    
    // ...
    
    std::ofstream fs("employees.csv");
    csv::csv_ostream csvs(fs);

    csvs << "name" << "salary" << csv::endl
         << "John" << 10000 << csv::endl
         << "Andy" << 20000 << csv::endl
         ;
```

* Reading csv files:

```cpp
    #include <fstream>
    #include <iostream>
    #include <string>
    #include "text/csv/istream.hpp"
    
    namespace csv = ::text::csv;
    
    // ...
    
    std::ifstream fs("employees.csv");
    csv::csv_istream csvs(fs);
    
    std::string name;
    unsigned salary;
    
    std::string header1, header2;
    csvs >> header1 >> header2;
    
    while (csvs) {
        csvs >> name >> salary;
        std::cout << header1 << ": " << name << ", "
                  << header2 << ": " << salary << "\n";
    }
```

* Using STL algorithms with csv iterators

```cpp
    #include <fstream>
    #include <algorithm>
    #include "text/csv/iterator.hpp"
    
    namespace csv = ::text::csv;
    
    // ...
    
    typedef csv::input_column_iterator<double> I;
    std::vector<double> vec;
    
    std::ifstream fs("numbers.csv");
    csv::csv_istream csvs(fs);
    
    // Copy a single row into vector
    std::copy(I(csvs), I(), std::back_inserter(vec));
```

License
=======

The library is licensed under permissive Boost license. See file
`LICENSE_1_0.txt` for details.
