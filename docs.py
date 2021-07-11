DOCLINE = "DEF_PROTO"
files = {
    'listdef.h': [],
    'vectordef.h': [],
    'dictdef.h': [],
}

bufout = ["# Collections Docs"]
for key in files.keys():
    bufout.append('\n## %s' % (key,))
    bufout.append('\n```C\n')
    try:
        fp = open(key, 'r')
    except:
        exit(1)
    for line in fp.readlines():
        if line.startswith(DOCLINE):
            bufout.append(line[len(DOCLINE) + 1:])
    fp.close()
    bufout.append('```\n')

try:
    fp = open("DOCS.md", "w")
except:
    exit(1)
fp.write(''.join(bufout))
fp.close()
