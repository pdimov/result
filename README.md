# result<T, E = std::error_code>

This repository implements `result<T, E>`, a type intended to be returned
from functions that can carry either a value of type `T` or an error of
type `E`. `E` is usually `std::error_code` or compatible.

Inspired by [Niall Douglas's Outcome library](https://github.com/ned14/outcome).

## Supported compilers

* g++ 4.7 or later
* clang++ 3.5 or later
* Visual Studio 2015, 2017

Tested on [Travis](https://travis-ci.org/pdimov/result/) and [Appveyor](https://ci.appveyor.com/project/pdimov/result/).

## Documentation

None yet.

## License

Distributed under the [Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).
