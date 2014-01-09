C++ CSV read/write library
==========================

Properties:

* RFC-4180 compliant
* STL-friendly interface: streams, iterators, ranges
* Works with any character set (`char`, `wchar_t`, etc)
* Header-only: no build required, just include files
* Supports various delimiters and quote characters
* Able to handle named columns
* (NYI) C++11 optional support

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
    
    typedef double value_type;
    typedef csv::input_column_iterator<value_type> I;
    std::vector<value_type> vec;
    
    std::ifstream fs("numbers.csv");
    csv::csv_istream csvs(fs);
    
    // Copy a single row into vector
    std::copy(I(csvs), I(), std::back_inserter(vec));
```
