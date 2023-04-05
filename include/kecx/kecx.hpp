#ifndef kecx_HPP
#define kecx_HPP

#include "store.hpp"
#include "extract.hpp"

/*
@doc README.md

# `kecx.cpp`

## Using this library

Simply include this library as a sub-dir in your project and `include`
`./include/kecx/kecx.hpp` in your code. E.g. in a directory structure

```
my_project
    my_file.hpp
    kecx/
        include/
            kecx/
                kecx.hpp
                ...
```

you can use this line in `my_file.hpp`:

```
#include "./kecx/include/kecx/kecx.hpp"
```

*/

namespace kecx {
    namespace store = store;
    namespace extract = extract;
}

#endif
