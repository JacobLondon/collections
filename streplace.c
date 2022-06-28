#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "streplace.h"

int streplace_trivial(char **s, const char *old, const char *new)
{
	char *result;
	char *sub;
	char *p;
	size_t len;
	size_t old_len;
	size_t new_len;

	assert(s != NULL);
	assert(*s != NULL);
	assert(old != NULL);
	assert(new != NULL);

	p = *s;
	len = strlen(*s);
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
	
	result = malloc((len + 1) * sizeof(char));
	if (!result) return -ENOMEM;
	
	/* re-add the bytes back into the result */
	for (p = *s; *p != '\0'; p++) {
		if (p == strstr(p, old)) {
			strcat(result, new);
			p += old_len - 1;
		}
		else {
			strncat(result, p, 1);
		}
	}

	free(*s);
	result[len] = '\0';
	*s = result;
	return 0;
}

int streplace_remember(char **s, const char *old, const char *new)
{
	static char *last_s = NULL;
	char *rp;
	char *result;
	char *sub;
	char *p;
	static size_t len;
	size_t old_len;
	size_t new_len;

	assert(s != NULL);
	assert(*s != NULL);
	assert(old != NULL);
	assert(new != NULL);

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
	
	rp = result = malloc((len + 1) * sizeof(char));
	if (!result) {
		return 1;
	}

	/* re-add the bytes back into the result */
	for (p = *s; *p != '\0'; p++) {
		// starts with
		if (strncmp(p, old, old_len) == 0) {
			// safe because we already checked len
			//tmp = sprintf(rp, "%s", new);
			memcpy(rp, new, new_len);
			rp += new_len;
			p += old_len - 1;
		}
		else {
			*rp = *p;
			rp++;
		}
	}

	free(*s);
	result[len] = '\0';
	*s = result;
	return 0;
}

int streplace_cache(char **s, const char *original, const char *replacement)
{
	static char **cache = NULL;

	static const char
		*s_last = NULL,
		*original_last = NULL,
		*replacement_last = NULL;
	static size_t
		s_len = 0,
		original_len = 0,
		replacement_len = 0;

	char *p, *sub, *rp;
	char *result;
	size_t len, i, copy_len;
	size_t cache_ndx = 0;

	assert(s != NULL);
	assert(*s != NULL);
	assert(original != NULL);
	assert(replacement != NULL);

	if (!cache) {
		cache = malloc(STREPLACE_CACHE_SIZE * sizeof(*cache));
		if (!cache) return -ENOMEM;
	}

	p = *s;
	if (s_last != p) {
		s_len = strlen(p);
		s_last = p;
	}

	if (original_last != original) {
		original_len = strlen(original);
		original_last = original;
	}

	if (replacement_last != replacement) {
		replacement_len = strlen(replacement);
		replacement_last = replacement;
	}

	if ((sub = strstr(p, original)) == NULL) {
		/* success, nothing to replace */
		return 0;
	}

	/* at least one replacement found */
	len = s_len;
	do {
		cache[cache_ndx++] = sub;
		if (cache_ndx >= STREPLACE_CACHE_SIZE) return -ENOMEM;

		len = len - original_len + replacement_len;
		p = &sub[original_len];
	} while ((sub = strstr(p, original)));

	result = malloc((len + 1) * sizeof(char));
	if (!result) return -ENOMEM;

	p = *s;
	rp = result;
	for (i = 0; i < cache_ndx; i++) {
		copy_len = cache[i] - p;
		memcpy(rp, p, copy_len);

		/* p must pass the copy and the original substring */
		p = &p[copy_len + original_len];

		/* rp must pass the copy */
		rp = &rp[copy_len];

		/* copy replacement into result */
		memcpy(rp, replacement, replacement_len);
		rp = &rp[replacement_len];
	}

	if (p < &(*s)[s_len]) {
		copy_len = &(*s)[s_len] - p;
		memcpy(rp, p, copy_len);
	}

	free(*s);
	result[len] = '\0';
	*s = result;
	return 0;
}

//#define STREPLACE_TEST
#ifdef STREPLACE_TEST

int main(void)
{
	int rv;
	char *a = strdup("$T is a different way of $T with a $F involved");

	char *t = "sleeping";
	char *f = "bed";

	(void)streplace_fast(&a, "$T", t);
	printf("Result10: \"%s\"\n", a);
	(void)streplace_fast(&a, "$F", f);
	printf("Result11: \"%s\"\n", a);
	free(a);

	a = strdup("$T is a different way of $T with a $F involved");
	t = "running";
	f = "steps";
	(void)streplace_fast(&a, "$T", t);
	printf("Result20: \"%s\"\n", a);
	(void)streplace_fast(&a, "$F", f);
	printf("Result21: \"%s\"\n", a);
	free(a);

	return 0;
}

#endif
