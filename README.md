# Generic C Collections
Currently contains vector and list. The code is generic as it uses a text replacement tool `def.py`. `def.py` is configured by a JSON file as follows:
```json
{
    "vectordef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int",
            "$ZEROVALUE": "(!a)",
            "$FREEVALUE": "NULL"
        },
        {
            "$NN": "STR", "$Nn": "Str", "$nn": "str", "$T": "const char *",
            "$ZEROVALUE": "(!a)",
            "$FREEVALUE": "NULL"
        }
    ],
    "listdef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int",
            "$ZEROVALUE": "(!a)",
            "$FREEVALUE": "NULL"
        }
    ],
    "dictdef.h": [
        {
            "$TT": "STR", "$Tt": "Str", "$tt": "str", "$T": "const char *",
            "$VV": "INT", "$Vv": "Int", "$vv": "int", "$V": "int",
            "$REFKEY": "(a)",
            "$FREEKEY": "NULL",
            "$FREEVALUE": "NULL",
            "$SIZEOFKEY": "strlen((const char *)a)",
            "$CMPKEY": "strcmp(a, b)",
            "$ZEROKEY": "(!a)",
            "$ZEROVALUE": "(!a)"
        }
    ]
}
```

Each key in a file definition is literal text to be replaced with it's value counterpart in the file noted.

This definition will go into `vectordef.h` and replace all instances of `$NN` with `INT`, `$Nn` with `Int`, and so forth. This allows the definition of generic code on a per-file basis. After going through `vectordef.h` the `def.py` tool will go into `listdef.h` then `dictdef.h` and perform the same process. This can be repeated for as many pattern replacements and files as wanted.

Key Definitions:
* $UU..........UPPER case version of a definition name
* $Uu..........Pascal case version of a definition name
* $uu..........lower case version of a definition name
* $T...........The corresponding C type to the name
* $ZEROVALUE...Given `a` is the C type, return true if `a` is the zero-value of its type
* $FREEVALUE...Given `a` is the C type, specify a `free` function or `NULL`
* $REFKEY......Given `a` is the C type, get the address of the value. ie. `char *` :: `(a)`, `int` :: `(&a)`, ...
* $FREEKEY.....See $FREEVALUE
* $SIZEOFKEY...Given `a` is the C type, return the byte count of `a`
* $CMPKEY......Given `a` and `b` are the same C type, return -1, 0, or 1 for equality as-per strcmp (0 means equal)
* $ZEROKEY.....See $ZEROVALUE

Example:
```bash
# Generate collections.h based on the JSON definition, make sure python is python3.6+
python def.py -f collections.json -o collections.h
```

## Usage
You can use `def.py` in your own projects. Add this repo as a submodule to your own (`git add submodule github.com/JacobLondon/collections`), and add it to your compiler's include path as a switch: `gcc <YOUR STUFF> -I <PATH_TO_SUBMODULES>` (assuming `collections` is in that folder). Everything can be generated with `make` in this project root directory. If you've added this project as a submodule to your own git repo, make sure you clone your project with `--recurse-submodules` as a switch, and run `git submodule update --recursive --remote` to get the latest push.

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
