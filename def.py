import json
import os
import sys

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

Usage: {sys.argv[0]} [-h, --help] [-f, --file FILENAME] [-o, --out OUTPUT]

Optional Parameters:
-h, --help; View this help and exit

Required Parameters:
-f, --file; Specify a JSON definition file
-o, --out;  Specify a result file to paste conversions into

Given a file name, a list of conversions may be created. This
list of conversions has dict of expressions to replace with
their replaced counterparts. For example:

{config_example}

This will convert vectordef.h twice; the first where each
$NN/Nn/nn/T expressions are replaced with INT/Int/int/int
respectively. The second follows this procedure, until the
nth definition has been performed.

At the end, all converted text is pasted into the output file.
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

def main():
    file = None
    out = None

    if len(sys.argv) == 1:
        print("-- no options specified", file=sys.stderr)
        usage()
        return 1

    if arg_check(sys.argv, '-h', '--help'):
        usage()
        return 0

    tmp = arg_get(sys.argv, '-f', '--file')
    if tmp is not None:
        file = tmp
    else:
        print("Missing --file", file=sys.stderr)
        return 1

    tmp = arg_get(sys.argv, '-o', "--output")
    if tmp is not None:
        out = tmp
    else:
        print("Missing --out", file=sys.stderr)
        return 1

    try:
        with open(file, "r") as fp:
            obj = json.load(fp)
    except Exception as e:
        print(f"Could not read JSON {file}: {e}", file=sys.stderr)
        return 1

    if type(obj) is not dict:
        print("Invalid JSON: File is not an object", file=sys.stderr)
        return 1

    converted_texts = []
    for filename, conversion_list in obj.items():
        if type(filename) is not str:
            print(f"{filename} is not a string", file=sys.stderr)
            return 1
        elif type(conversion_list) is not list:
            print(f"{conversion_list} is not a list", file=sys.stderr)

        try:
            with open(filename, "r") as fp:
                text = fp.read()
        except:
            print(f"Could not open input file {filename}", file=sys.stderr)
            return 1

        for conversion in conversion_list:
            if type(conversion) is not dict:
                print(f"Conversion {conversion} is not an object", file=sys.stderr)
                return 1

            ptext = text
            for unconv, conv in conversion.items():
                if type(unconv) is not str:
                    print(f"Unconverted item {unconv} is not a string", file=sys.stderr)
                    return 1
                elif type(conv) is not str:
                    print(f"Converted item {conv} is not a string", file=sys.stderr)
                    return 1

                ptext = ptext.replace(unconv, conv)
            converted_texts.append(ptext)

    try:
        with open(out, "w") as fp:
            for text in converted_texts:
                fp.write(text)
    except:
        print(f"Could not open output file {filename}", file=sys.stderr)
        return 1
    return 0

if __name__ == '__main__':
    exit(main())
