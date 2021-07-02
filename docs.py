'''
Generate docs
'''

DOCLINE = "DEF_PROTO"
files = {
    'listdef.h': [],
    'vectordef.h': [],
}

bufout = ["# Collections Docs"]
for key in files.keys():
    bufout.append('\n## %s' % (key,))
    bufout.append('\n```C\n')
    with open(key, 'r') as fp:
        for line in fp.readlines():
            if line.startswith(DOCLINE):
                bufout.append(line[len(DOCLINE) + 1:])
    bufout.append('```\n')

with open('DOCS.md', 'w') as fp:
    fp.write(''.join(bufout))
