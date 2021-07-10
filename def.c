/**
 * Act as def.py, but bootstrapped so Python isn't needed.
 * 
 * JSON format:
 * Dict[str, Dict[Vec[Dict[str, str]]]]
 */

#include <stdio.h>
#include <stdlib.h>
#include "jsmn.h"
// generated, make sure protos are above, defs are below
#include "def.h"
#define DEF_DICT_STR_STR
#define DEF_VECTOR_DICT_STR_STR
#define DEF_DICT_STR_VECTOR_DICT_STR_STR
#include "def.h"

struct Options {
    const char *ifile;
    const char *ofile;
};

struct Json {
    char *textheap;
    size_t textlen;
    jsmn_parser parser;
    jsmntok_t *tokens;
    int numtoks;
};

static int json_open(struct Json *json, const char *filename);
static int json_dump(struct Json *json, struct DictStrVectorDictStrStr *out);
static int json_dump_file(struct Json *json, int i, struct VectorDictStrStr *out);
static int json_dump_file_dict(struct Json *json, int i, struct DictStrStr *out);
static void json_abort(const char *text, const jsmntok_t *t);
static int get_options(int argc, char *argv[], struct Options *opts);
//static int arg_check(int argc, char *argv[], const char *da, const char *ddarg);
static char *arg_get(int argc, char *argv[], const char *da, const char *ddarg);
char *file_read(const char *fname, size_t *size);
char *strndup(const char *str, size_t n);

int main(int argc, char *argv[])
{
    struct Json json;
    struct Options opts;
    struct DictStrVectorDictStrStr dict;

    if (get_options(argc, argv, &opts) != 0) {
        return 1;
    }

    if (json_open(&json, opts.ifile) != 0) {
        return 1;
    }

    dict = dict_str_vector_dict_str_str_init();
    if (json_dump(&json, &dict) != 0) {
        return 1;
    }

    return 0;
}

static int json_open(struct Json *json, const char *filename)
{
    json->textheap = file_read(filename, &json->textlen);
    if (!json->textheap) {
        fprintf(stderr, "Cannot open %s\n", filename);
        return 1;
    }

    jsmn_init(&json->parser);
    json->numtoks = jsmn_parse(&json->parser, json->textheap, json->textlen, NULL, 0);
    if (json->numtoks < 0) {
        fprintf(stderr, "%s is not a valid JSON\n", filename);
        return 1;
    }

    json->tokens = malloc(sizeof(*json->tokens) * json->numtoks);
    if (!json->tokens) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    jsmn_init(&json->parser);
    jsmn_parse(&json->parser, json->textheap, json->textlen, json->tokens, json->numtoks);

    return 0;
}

static int json_dump(struct Json *json, struct DictStrVectorDictStrStr *out)
{
    enum XState {
        XBEGIN,
        XKEY,
        XLIST,
        XDONE,
    };

    enum XState state = XBEGIN;
    const jsmntok_t *t = json->tokens;
    const char *text = json->textheap;
    int i;
    int count;
    char *tmp_curfile;
    char *tmp_key;
    //char *tmp_value;
    struct VectorDictStrStr *tmp_vec;
    struct DictStrStr *tmp_inner;

    for (i = 0; i < json->numtoks; i++) {
        t = &json->tokens[i];

        switch (state) {
        case XBEGIN:
            if (t->type != JSMN_OBJECT) {
                json_abort(text, t);
            }
            state = XKEY;
            count = t->size;
            break;
        case XKEY:
            if (t->type != JSMN_STRING) {
                json_abort(text, t);
            }
            tmp_curfile = strndup(text + t->start, t->end - t->start);
            assert(tmp_curfile);
            state = XLIST;
            break;
        case XLIST:
            if (t->type != JSMN_ARRAY) {
                json_abort(text, t);
            }
            tmp_inner = vector_dict_str_str_new();
            i = json_dump_file(json, i, tmp_inner);
            if (i < 0) {
                return 1;
            }
            dict_str_vector_dict_str_str_set(out, tmp_curfile, tmp_inner);
            count--;
            if (count == 0) {
                state = XDONE;
                goto out;
            }
            state = XKEY;
            break;
        }
    }

out:
    if (state != XDONE) {
        fprintf(stderr, "Unexpected EOF\n");
        return 1;
    }
    return 0;
}

static int json_dump_file(struct Json *json, int i, struct VectorDictStrStr *out)
{
    enum XState {
        XBEGIN,
        XOBJECT,
        XDONE,
    };

    enum XState state = XBEGIN;
    const char *text = json->textheap;
    const jsmntok_t *t;
    int count;
    struct DictStrStr *tmp_dict;

    for ( ; i < json->numtoks; i++) {
        t = &json->tokens[i];
        switch (state) {
        case XBEGIN:
            if (t->type != JSMN_OBJECT) {
                json_abort(text, t);
            }
            state = XOBJECT;
            count = t->size;
            break;
        case XOBJECT:
            if (t->type != JSMN_STRING) {
                json_abort(text, t);
            }
            tmp_dict = dict_str_str_new();
            i = json_dump_file_dict(json, i, tmp_dict);
            if (i < 0) {
                return -1;
            }
            vector_dict_str_str_push(out, tmp_dict);
            count--;
            if (count == 0) {
                state = XDONE;
                goto out;
            }
            state = XOBJECT;
            break;
        }
    }

out:
    if (state != XDONE) {
        fprintf(stderr, "Unexpected EOF\n");
        return -1;
    }
    return 0;
}

static int json_dump_file_dict(struct Json *json, int i, struct DictStrStr *out)
{
    enum XState {
        XBEGIN,
        XKEY,
        XVALUE,
        XDONE,
    };

    enum XState state = XBEGIN;
    const char *text = json->textheap;
    const jsmntok_t *t;
    int count;
    char *key;
    char *value;

    for ( ; i < json->numtoks; i++) {
        switch (state) {
        case XBEGIN:
            
        }
    }
}

static void json_abort(const char *text, const jsmntok_t *t)
{
    fprintf(stderr, "Unexpected token: %.*s\n", t->end - t->start, text + t->start);
    exit(1);
}

static int get_options(int argc, char *argv[], struct Options *opts)
{
    opts->ifile = arg_get(argc, argv, "-f", "--file");
    if (!opts->ifile) {
        fprintf(stderr, "Missing --file\n");
        return 1;
    }

    opts->ofile = arg_get(argc, argv, "-o", "--out");
    if (!opts->ofile) {
        fprintf(stderr,  "Missing --out\n");
        return 1;
    }

    return 0;
}

#if 0
static int arg_check(int argc, char *argv[], const char *da, const char *ddarg)
{
    int i;
    for (i = 0; i < argc; i++) {
        if ((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) {
            return i;
        }
    }
    return 0;
}
#endif

static char *arg_get(int argc, char *argv[], const char *da, const char *ddarg)
{
    int i;
    for (i = 0; i < argc; i++) {
        if (((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) && (i + 1 < argc)) {
            return argv[i + 1];
        }
    }
    return NULL;
}

char *file_read(const char *fname, size_t *size)
{
    FILE *f;
    char *buf;
    size_t bytes;
    long length;

    if (!fname) {
        return NULL;
    }

    f = fopen(fname, "rb");
    if (!f) {
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fclose(f);
        return NULL;
    }

    length = ftell(f);
    if (length < 0) {
        fclose(f);
        return NULL;
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        fclose(f);
        return NULL;
    }

    buf = malloc(length + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    bytes = fread(buf, 1, length, f);
    if (ferror(f) != 0) {
        free(buf);
        fclose(f);
        return NULL;
    }

    buf[bytes] = '\0';
    if (size) {
        *size = bytes;
    }

    fclose(f);
    return buf;
}

char *strndup(const char *str, size_t n)
{
    size_t size = strlen(str);
    size = size > n ? n : size;
    char *buf = malloc(size + 1);
    if (!buf) return NULL;
    (void)memcpy(buf, str, size);
    buf[size] = 0; // manual NUL
    return buf;
}
