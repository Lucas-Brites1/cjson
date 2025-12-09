#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char peek_current(const char *text)
{
  if (text == NULL)
    return '\0';

  return *text;
}

char peek_next(const char *text)
{
  if (text == NULL)
    return '\0';
  return *(text + 1);
}

int match_and_consume(const char **cursor, char expected)
{
  if (**cursor == expected)
  {
    (*cursor)++;
    return 1;
  }
  return 0;
}

void consume_and_append(const char **source, char **out)
{
  if (*source == NULL || **source == '\0')
    return;

  **out = **source;
  (*out)++;
  (*source)++;
  **out = '\0';
}

char *get_string_buffer(int length)
{
  char *buffer = (char *)malloc(sizeof(char) * (length + 1));
  if (buffer == NULL)
    return NULL;

  buffer[0] = '\0';
  return buffer;
}

void skip_whitespace(const char **text)
{
  while (*text != NULL && (**text == ' ' || **text == '\t' || **text == '\n' || **text == '\r'))
    (*text)++;
}

void consume_until_delimiter(const char **cursor, char **out, char delimiter)
{
  if (*cursor == NULL || **cursor == '\0')
    return;

  while (**cursor != '\0' && !match_and_consume(cursor, delimiter))
  {
    **out = **cursor;
    (*out)++;
    (*cursor)++;
  }
  **out = '\0';
}
