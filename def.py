import json
import os
import sys

# Name: Type
config_example = '''
{
    "vectordef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int"
        },
        {
            "$NN": "LONG", "$Nn": "Long", "$nn": "long", "$T": "long"
        }
    ]
}
'''

def usage():
    print(f"""\
def -- define file converion procedure

Convert matched expressions as defined by a JSON file.

Usage: {sys.argv[0]} [-h, --help] [-c, --cat] [-f, --file FILENAME] [-o, --out OUTPUT]

Optional Parameters:
-h, --help; View this help and exit
-c, --cat; Cat the files together, create include file by default

Required Parameters:
-f, --file; Specify a JSON definition file
-o, --out;  Specify a result file to paste conversions into

Conversions are prefixed by '.tmp.[\\d].<orig>' where <orig>
is the original file name.

Given a file name, a list of conversions may be created. This
list of conversions has dict of expressions to replace with
their replaced counterparts. For example:

{config_example}

This will convert vectordef.h into 2 tmp files, the first where
each $NN/Nn/nn/T expressions are replaced with INT/Int/int/int
respectively. The second follows this procedure, and is pasted
into the second tmp file.

At the end, these files are cat'ed or an include file is created.
""")

def arg_check(argv, da, ddarg):
    return da in argv or ddarg in argv

def arg_get(argv, da, ddarg):
    try:
        ndx = argv.index(da)
    except:
        try:
            ndx = argv.index(ddarg)
        except:
            return None
    if ndx + 1 < len(argv):
        return argv[ndx + 1]
    return None

if __name__ == '__main__':
    file = None
    out = None
    cFlag = False

    if len(sys.argv) == 1:
        usage()
        exit(1)

    if arg_check(sys.argv, '-h', '--help'):
        usage()
        exit(0)

    cFlag = arg_check(sys.argv, '-c', '--cat')

    tmp = arg_get(sys.argv, '-f', '--file')
    if tmp is not None:
        file = tmp
    else:
        print("Missing --file", file=sys.stderr)
        exit(1)
    
    tmp = arg_get(sys.argv, '-o', "--output")
    if tmp is not None:
        out = tmp
    else:
        print("Missing --out", file=sys.stderr)
        exit(1)

    try:
        with open(file, "r") as fp:
            obj = json.load(fp)
    except Exception as e:
        print(f"Could not read JSON {file}: {e}", file=sys.stderr)
        exit(1)

    fmt = ".tmp.%d.%s"
    tmpfiles = []
    try:
        for filename, conversion_list in obj.items():
            i = 0
            for conversion in conversion_list:
                i += 1
                first = True
                default1 = fmt % (i, filename)
                tmpfiles.append(default1)
                for unconv, conv in conversion.items():
                    if first:
                        os.system(f"sed 's/{unconv}/{conv}/g' {filename} > {default1}")
                        first = False
                    else:
                        os.system(f"sed -i 's/{unconv}/{conv}/g' {default1}")

        if cFlag:
            catted = ' '.join(tmpfiles)
            os.system(f"cat {catted} > {out}")

        else:
            # create include
            with open(out, "w") as fp:
                guard = out.upper().replace(".", "_")
                fp.write(f"#ifndef DEF_{guard}\n")
                fp.write(f"#define DEF_{guard}\n")
                for filename in tmpfiles:
                    fp.write(f'#include "{filename}"\n')
                fp.write(f"#endif // DEF_{guard}\n")

        exit(0)

    except Exception as e:
        print(f"Invalid JSON format see --help: {e}", file=sys.stderr)
        exit(1)
