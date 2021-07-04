# Generic C Collections
Currently contains vector and list. The code is generic as it uses a text replacement tool `def.py`. `def.py` is configured by a JSON file as follows:
```json
{
    "vectordef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int"
        },
        {
            "$NN": "STR", "$Nn": "Str", "$nn": "str", "$T": "const char *"
        }
    ],
    "listdef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int"
        }
    ]
}
```

This definition will go into `vectordef.h` and replace all instances of `$NN` with `INT`, `$Nn` with `Int`, and so forth. This allows the definition of generic code on a per-file basis. After going through `vectordef.h` the `def.py` tool will go into `listdef.h` and perform the same process. This can be repeated for as many pattern replacements and files as wanted. The above format was chosen to explicitly define the name of code generated (NN/Nn/nn) and the type associated with it.

Example:
```bash
# Generate collections.h based on the JSON definition
python def.py -f collections.json -o collections.h
```

## Usage
You can use `def.py` in your own projects. Add this repo as a submodule to your own, and add it to your compiler's include path as a switch: `gcc <YOUR STUFF> -I <PATH_TO_SUBMODULES>` (assuming `collections` is in that folder). Everything can be generated with `make` in this project root directory.

### Including
To use any library defined in `collections.json` then this header file may be included in your project. All functions are defined in the `collections.h` file, but only the prototypes are included by default.

In _one_ location in your project, above the `#include <collections/collections.h>` you may define symbols to allow use of a specific type of collection:
```
#define DEF_LIST_INT // import list_int function definitions
#define DEF_VECTOR_STR // import vector_str function definitions
#include <collections/collections.h>
// - snip -
```

To include user-defined functions and use them in the JSON definition, simply `#include` the relevant header file ABOVE the `#include <collections/collections.h>` where the type is defined with,for example `DEF_LIST_INT`

### Docs
Run `make docs` then see `DOCS.md`.

If a `DEF_<COLLECTION>_<TYPE>` is defined without the JSON definition defining it, then the program will not compile.

## Requirements
* GNU Make (for building the test file)
* Python 3.6+ (for fstrings, may add support for Python 2)
  * python36+ will need to be symlinked to `python` to be able to execute the `def.py` to run as `./def.py`. Otherwise, use `python def.py`
* Any C99+ compiler

## Additional Notes
* All `init` or `new` functions use macros to achieve default arguments. Furthermore, these functions don't necessarily all have something to return. They both set a global variable and create an object out of convenience.

# TODO
* Specify ifree in JSON
* Dict delete
