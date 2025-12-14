#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../include/cjson.h"
#include "../include/dynamic_array.h"

typedef struct
{
  char *buffer;
  t_size length;
  t_size capacity;
} JsonWriter;

static void writer_init(JsonWriter *w)
{
  w->capacity = 1024;
  w->length = 0;
  w->buffer = calloc(1, w->capacity);
}

static void writer_ensure_capacity(JsonWriter *w, t_size len)
{
  if (w->length + len >= w->capacity)
  {
    while (w->length + len >= w->capacity)
    {
      w->capacity *= 2;
    }
    char *new_buff = realloc(w->buffer, w->capacity);
    if (!new_buff)
    {
      perror("Failed to reallocate JSON buffer");
      exit(1);
    }
    w->buffer = new_buff;
  }
}

static void writer_append(JsonWriter *w, const char *str)
{
  if (!str)
    return;
  t_size len = strlen(str);

  writer_ensure_capacity(w, len);

  memcpy(w->buffer + w->length, str, len);
  w->length += len;
  w->buffer[w->length] = '\0';
}

static void writer_append_string_escaped(JsonWriter *w, const char *str)
{
  writer_append(w, "\"");

  if (!str)
  {
    writer_append(w, "\"");
    return;
  }

  const char *p = str;
  while (*p)
  {
    const char *esc = NULL;
    char buf[2];

    switch (*p)
    {
    case '\"':
      esc = "\\\"";
      break;
    case '\\':
      esc = "\\\\";
      break;
    case '\b':
      esc = "\\b";
      break;
    case '\f':
      esc = "\\f";
      break;
    case '\n':
      esc = "\\n";
      break;
    case '\r':
      esc = "\\r";
      break;
    case '\t':
      esc = "\\t";
      break;
    default:
      buf[0] = *p;
      buf[1] = '\0';
      writer_append(w, buf);
      break;
    }

    if (esc)
      writer_append(w, esc);
    p++;
  }

  writer_append(w, "\"");
}

static void writer_printf(JsonWriter *w, const char *format, ...)
{
  va_list args;

  va_start(args, format);
  int needed = vsnprintf(NULL, 0, format, args);
  va_end(args);

  if (needed < 0)
    return;

  writer_ensure_capacity(w, needed + 1);

  va_start(args, format);
  vsnprintf(w->buffer + w->length, needed + 1, format, args);
  va_end(args);

  w->length += needed;
}

static void _cjson_encode_internal(JsonWriter *w, void *instance, t_json_model *model, bool pretty, int depth)
{
  const char *newline = pretty ? "\n" : "";
  const char *spacing = pretty ? "  " : "";

  writer_append(w, "{");
  writer_append(w, newline);

  t_reflect_field *fields = model->reflect->fields;
  int i = 0;
  int printed_count = 0;

  while (fields[i].name != NULL)
  {
    t_reflect_field *field = &fields[i];
    t_json_field_config *config = &model->fields_config[i];

    if (config->ignore)
    {
      i++;
      continue;
    }

    if (printed_count > 0)
    {
      writer_append(w, ",");
      writer_append(w, newline);
    }

    if (pretty)
    {
      for (int d = 0; d < depth + 1; d++)
        writer_append(w, spacing);
    }

    const char *key = config->json_field_name ? config->json_field_name : field->name;
    writer_append(w, "\"");
    writer_append(w, key);
    writer_append(w, "\": ");

    void *ptr = (char *)instance + field->offset;

    switch (field->type)
    {
    case REFLECT_TYPE_INTEGER:
      writer_printf(w, "%d", *(int *)ptr);
      break;

    case REFLECT_TYPE_STRING:
    {
      char *str = *(char **)ptr;
      if (str)
        writer_append_string_escaped(w, str);
      else
        writer_append(w, "null");
      break;
    }

    case REFLECT_TYPE_BOOL:
      writer_append(w, *(bool *)ptr ? "true" : "false");
      break;

    case REFLECT_TYPE_OBJECT:
    {
      void *child_ptr = *(void **)ptr;
      if (child_ptr)
      {
        t_json_model *child_model = (t_json_model *)field->child_meta;
        _cjson_encode_internal(w, child_ptr, child_model, pretty, depth + 1);
      }
      else
      {
        writer_append(w, "null");
      }
      break;
    }

    case REFLECT_TYPE_ARRAY_STRING:
    {
      Array **arr_ptr = (Array **)ptr;
      if (*arr_ptr && (*arr_ptr)->data)
      {
        Array *arr = *arr_ptr;
        writer_append(w, "[");
        char **strings = (char **)arr->data;

        for (t_size k = 0; k < arr->count; k++)
        {
          if (k > 0)
            writer_append(w, ", ");
          writer_append_string_escaped(w, strings[k]);
        }
        writer_append(w, "]");
      }
      else
      {
        writer_append(w, "null");
      }
      break;
    }
    case REFLECT_TYPE_ARRAY_OBJECT:
    {
      Array **arr_ptr = (Array **)ptr;

      if (*arr_ptr && (*arr_ptr)->data)
      {
        Array *arr = *arr_ptr;
        writer_append(w, "[");
        writer_append(w, newline);

        t_json_model *child_model = (t_json_model *)field->child_meta;

        void **items = (void **)arr->data;

        for (t_size k = 0; k < arr->count; k++)
        {
          if (k > 0)
          {
            writer_append(w, ",");
            writer_append(w, newline);
          }

          if (pretty)
          {
            for (int d = 0; d < depth + 1; d++)
              writer_append(w, spacing);
          }

          _cjson_encode_internal(w, items[k], child_model, pretty, depth + 1);
        }

        writer_append(w, newline);
        if (pretty)
        {
          for (int d = 0; d < depth; d++)
            writer_append(w, spacing);
        }
        writer_append(w, "]");
      }
      else
      {
        writer_append(w, "null");
      }
      break;
    }

    default:
      writer_append(w, "\"unsupported_type\"");
    }

    printed_count++;
    i++;
  }

  writer_append(w, newline);
  if (pretty)
  {
    for (int d = 0; d < depth; d++)
      writer_append(w, spacing);
  }
  writer_append(w, "}");
}

char *cjson_encode(void *data, t_json_model *model, bool pretty)
{
  if (!data || !model)
    return NULL;

  JsonWriter w;
  writer_init(&w);

  _cjson_encode_internal(&w, data, model, pretty, 0);

  return w.buffer;
}
