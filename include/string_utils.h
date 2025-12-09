#ifndef STRING_UTILS_H
#define STRING_UTILS_H

char peek_next(const char *text);
char peek_current(const char *text);
void consume_and_append(const char **text, char **out);
int match_and_consume(const char **cursor, char expected);
char *get_string_buffer(int length);
void skip_whitespace(const char **text);
void consume_until_delimiter(const char **cursor, char **out, char delimiter);

#endif
