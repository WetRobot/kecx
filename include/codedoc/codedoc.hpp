#ifndef CODEDOC_HPP
#define CODEDOC_HPP

#include "store.hpp"
#include "extract.hpp"

/*
@doc README.md

# `codedoc.cpp`

## Using this library

Simply include this library as a sub-dir in your project and `include`
`./include/codedoc/codedoc.hpp` in your code. E.g. in a directory structure

```
my_project
    my_file.hpp
    codedoc/
        include/
            codedoc/
                codedoc.hpp
                ...
```

you can use this line in `my_file.hpp`:

```
#include "./codedoc/include/codedoc/codedoc.hpp"
```

*/

namespace codedoc {
    namespace store = store;
    namespace extract = extract;
}

#endif
