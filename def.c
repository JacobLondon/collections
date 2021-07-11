/**
 * Act as def.py, but bootstrapped so Python isn't needed.
 * 
 * JSON format:
 * Dict[str, Dict[Vec[Dict[str, str]]]]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void json_close(struct Json *json);
static int json_dump(struct Json *json, struct DictStrVectorDictStrStr *out);
static int json_dump_file(struct Json *json, int i, struct VectorDictStrStr *out);
static int json_dump_file_dict(struct Json *json, int i, struct DictStrStr *out);
static void json_abort(const char *text, const jsmntok_t *t);
static int get_options(int argc, char *argv[], struct Options *opts);
static int arg_check(int argc, char *argv[], const char *da, const char *ddarg);
static char *arg_get(int argc, char *argv[], const char *da, const char *ddarg);
char *file_read(const char *fname, size_t *size);
int streplace(char **s, const char *old, const char *new);

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

    FILE *out = fopen(opts.ofile, "w");
    if (!out) {
        fprintf(stderr, "Could not open %s\n", opts.ofile);
        return 1;
    }

    dict = dict_str_vector_dict_str_str_init();
    if (json_dump(&json, &dict) != 0) {
        return 1;
    }

    struct DictPairStrVectorDictStrStr t;
    struct DictStrStr **d;
    struct DictPairStrStr kv;
    char *deftext;
    char *deftextcopy;

    // for each filename
    for (t = dict_str_vector_dict_str_str_iter(&dict);
         t.key;
         t = dict_str_vector_dict_str_str_next(&dict, t))
    {
        deftext = file_read(*t.key, NULL);
        if (!deftext) {
            fprintf(stderr, "Could not open %s\n", *t.key);
            continue;
        }

        // for each replacement dict
        for (d = vector_dict_str_str_iter(*t.value);
             d;
             d = vector_dict_str_str_next(*t.value, d))
        {
            deftextcopy = strdup(deftext);
            if (!deftextcopy) {
                fprintf(stderr, "Out of memory\n");
                return 1;
            }

            // for each replacement
            for (kv = dict_str_str_iter(*d);
                 kv.key;
                 kv = dict_str_str_next(*d, kv))
            {
                if (streplace(&deftextcopy, *kv.key, *kv.value) != 0) {
                    fprintf(stderr, "Total failure to replace in %s\n", *t.key);
                    return 1;
                }
            }

            (void)fwrite(deftextcopy, 1, strlen(deftextcopy), out);
            free(deftextcopy);
        }
        free(deftext);
    }

    json_close(&json);
    fclose(out);
    dict_str_vector_dict_str_str_deinit(&dict);
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

static void json_close(struct Json *json)
{
    assert(json);
    assert(json->textheap);
    assert(json->tokens);

    free(json->textheap);
    free(json->tokens);
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
    char *def_file;
    struct VectorDictStrStr *tmp_vec;

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
            def_file = (char *)text + t->start;
            def_file[t->end - t->start] = 0;
            state = XLIST;
            break;
        case XLIST:
            if (t->type != JSMN_ARRAY) {
                json_abort(text, t);
            }
            tmp_vec = vector_dict_str_str_new();
            i = json_dump_file(json, i, tmp_vec);
            if (i < 0) {
                return 1;
            }
            dict_str_vector_dict_str_str_set(out, def_file, tmp_vec);
            count--;
            if (count == 0) {
                state = XDONE;
                goto out;
            }
            state = XKEY;
            break;
        case XDONE:
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
            if (t->type != JSMN_ARRAY) {
                json_abort(text, t);
            }
            state = XOBJECT;
            count = t->size;
            break;
        case XOBJECT:
            if (t->type != JSMN_OBJECT) {
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
        case XDONE:
            break;
        }
    }

out:
    if (state != XDONE) {
        fprintf(stderr, "Unexpected EOF\n");
        return -1;
    }
    return i;
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
            key = (char *)text + t->start;
            key[t->end - t->start] = 0;
            state = XVALUE;
            break;
        case XVALUE:
            if (t->type != JSMN_STRING) {
                json_abort(text, t);
            }
            value = (char *)text + t->start;
            value[t->end - t->start] = 0;
            dict_str_str_set(out, key, value);
            count--;
            if (count == 0) {
                state = XDONE;
                goto out;
            }
            state = XKEY;
            break;
        case XDONE:
            break;
        }
    }

out:
    if (state != XDONE) {
        fprintf(stderr, "Unexpected EOF\n");
        return -1;
    }
    return i;
}

static void json_abort(const char *text, const jsmntok_t *t)
{
    fprintf(stderr, "Unexpected token: %.*s\n", t->end - t->start, text + t->start);
    exit(1);
}

static int get_options(int argc, char *argv[], struct Options *opts)
{
    if (arg_check(argc, argv, "-h", "--help")) {
        fprintf(stderr,
            "def -- JSON defined text replacement\n\n"
            "Options\n"
            "\t-f, --file; required, specify JSON definition\n"
            "\t-o, --out; required, specify output file\n"
            "\t-h, --help; optional, view this help\n\n"
            "JSON Format:\n"
            "\t{ \"filename1\": [ { \"old-value1\": \"new-value1\", ...}, \n"
            "\t                 { \"old-value1\": \"diff-value1\", ... }, ...],\n"
            "\t  \"filename2\": ...\n"
        );
        exit(0);
    }

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

int streplace(char **s, const char *old, const char *new)
{
    static char *last_s = NULL;
    char *rp;
    char *result;
    char *sub;
    char *p;
    static size_t len;
    size_t old_len;
    size_t new_len;
    int tmp;

    // input failure
    if (s == NULL || *s == NULL || old == NULL || new == NULL) {
        return 1;
    }

    p = *s;
    if (last_s != *s) {
        len = strlen(*s);
        last_s = *s;
    }
    old_len = strlen(old);
    new_len = strlen(new);

    /* no replacements found */
    if ((sub = strstr(p, old)) == NULL) {
        return 0;
    }

    /* at least one replacement found */
    do {
        len = len - old_len + new_len;
        p = (char *)((size_t)sub + old_len);
    } while ((sub = strstr(p, old)));
    
    rp = result = calloc(len + 1, sizeof(char));
    if (!result) {
        return 1;
    }

    /* re-add the bytes back into the result */
    for (p = *s; *p != '\0'; p++) {
        // starts with
        if (strncmp(p, old, old_len) == 0) {
            // safe because we already checked len
            tmp = sprintf(rp, "%s", new);
            rp += tmp;
            p += old_len - 1;
        }
        else {
            *rp = *p;
            rp++;
        }
    }

    free(*s);
    *s = result;

    return 0;
}
