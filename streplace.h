#ifndef STREPLACE_H
#define STREPLACE_H

#ifndef STREPLACE_CACHE_SIZE
#define STREPLACE_CACHE_SIZE 256
#endif

int streplace_trivial(char **s, const char *old, const char *new);
int streplace_remember(char **s, const char *old, const char *new);
int streplace_cache(char **s, const char *original, const char *replacement);

#endif /* STREPLACE_H */
