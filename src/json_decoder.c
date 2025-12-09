#include <stdio.h>
#include <stdlib.h>
#include "../include/string_utils.h"
#include "../include/cjson.h"
#include "../include/dynamic_array.h"
#include <string.h>

int parse_int(const char **cursor);
double parse_double(const char **cursor);
char *parse_string(const char **cursor);
int parse_boolean(const char **cursor);

void skip_json_value(const char **cursor);
void parse_value(t_json_model *model, const char *json_key, const char **cursor, void *output_instance);
t_json_type detect_json_type(const char *cursor);
t_reflect_field *find_field_by_jsonkey(t_json_model *model, const char *json_key);
int _cjson_decode_internal(const char **cursor, t_json_model *model, void *instance);

int get_json_string_length(const char *cursor)
{
  int length = 0;

  while (*cursor != '\0' && *cursor != '"')
  {
    length++;
    cursor++;
  }

  if (*cursor == '"')
    return length;
  return -1;
}

t_json_type detect_json_type(const char *cursor)
{
  char c = peek_current(cursor);

  if (c == '{')
    return JSON_TYPE_OBJECT;
  if (c == '[')
    return JSON_TYPE_ARRAY;
  if (c == '"')
    return JSON_TYPE_STRING;
  if (c == 't' || c == 'f')
    return JSON_TYPE_BOOLEAN;
  if (c == 'n')
    return JSON_TYPE_NULL;

  if ((c >= '0' && c <= '9') || c == '-')
  {
    return JSON_TYPE_NUMBER;
  }

  return JSON_TYPE_UNKNOWN;
}

t_reflect_field *find_field_by_jsonkey(t_json_model *model, const char *json_key)
{
  if (model == NULL || json_key == NULL)
    return NULL;

  for (int i = 0; i < model->reflect->field_count; i++)
  {
    t_json_field_config *config = &model->fields_config[i];
    if (config->ignore)
    {
      continue;
    }

    const char *target_name = config->field_name;

    if (config->json_field_name != NULL)
    {
      target_name = config->json_field_name;
    }

    if (strcmp(json_key, target_name) == 0)
    {
      return &model->reflect->fields[i];
    }
  }

  return NULL;
}

void skip_json_value(const char **cursor)
{
  if (peek_current(*cursor) == '"')
  {
    char *trash = parse_string(cursor);
    if (trash)
      free(trash);
    return;
  }

  while (**cursor != '\0' && **cursor != ',' && **cursor != '}' && **cursor != ']' &&
         **cursor != ' ' && **cursor != '\n' && **cursor != '\t')
  {
    (*cursor)++;
  }
}

int cjson_decode(const char *json, t_json_model *model, void *instance)
{
  const char *cursor = json;
  return _cjson_decode_internal(&cursor, model, instance);
}

int _cjson_decode_internal(const char **cursor, t_json_model *model, void *instance)
{
  skip_whitespace(cursor);
  if (!match_and_consume(cursor, '{'))
    return -1;

  while (peek_current(*cursor) != '}' && **cursor != '\0')
  {
    skip_whitespace(cursor);
    char *key = parse_key(cursor);
    if (!key)
      return -1;

    skip_whitespace(cursor);
    match_and_consume(cursor, ':');
    skip_whitespace(cursor);

    parse_value(model, key, cursor, instance);

    free(key);
    skip_whitespace(cursor);
    match_and_consume(cursor, ',');
  }

  if (!match_and_consume(cursor, '}'))
    return -1;
  return 0;
}

char *parse_key(const char **cursor)
{
  skip_whitespace(cursor);

  if (!match_and_consume(cursor, '"'))
    return NULL;

  char *key_name = get_string_buffer(100);
  char *writer = key_name;

  consume_until_delimiter(cursor, &writer, '"');
  return key_name;
}

void parse_value(t_json_model *model, const char *json_key, const char **cursor, void *output_instance)
{
  t_reflect_field *field = find_field_by_jsonkey(model, json_key);
  t_json_type json_type = detect_json_type(*cursor);

  if (field == NULL)
  {
    skip_json_value(cursor);
    return;
  }

  switch (field->type)
  {
  case REFLECT_TYPE_OBJECT:
    if (json_type == JSON_TYPE_OBJECT)
    {
      t_json_model *child_model = (t_json_model *)field->child_meta;
      if (!child_model)
      {
        skip_json_value(cursor);
        return;
      }

      void **ptr_to_child_ptr = (void **)((char *)output_instance + field->offset);
      t_size child_size = child_model->reflect->size;
      void *child_instance = calloc(1, child_size);

      *ptr_to_child_ptr = child_instance;
      if (_cjson_decode_internal(cursor, child_model, child_instance) != 0)
      {
      }
      break;
    }
    else
    {
      skip_json_value(cursor);
      return;
    }
  case REFLECT_TYPE_ARRAY_INT:
  {
    if (json_type == JSON_TYPE_ARRAY)
    {
      if (!match_and_consume(cursor, '['))
      {
        skip_json_value(cursor);
        return;
      }

      Array *list = array_create(sizeof(int));

      while (**cursor != '\0' && peek_current(*cursor) != ']')
      {
        skip_whitespace(cursor);

        int val = parse_int(cursor);
        array_add(list, &val);

        skip_whitespace(cursor);
        match_and_consume(cursor, ',');
      }

      match_and_consume(cursor, ']');

      Array **target_ptr = (Array **)((char *)output_instance + field->offset);
      *target_ptr = list;

      break;
    }
    else
    {
      skip_json_value(cursor);
    }
    break;
  }
  case REFLECT_TYPE_ARRAY_DOUBLE:
    if (json_type == JSON_TYPE_ARRAY)
    {
      if (!match_and_consume(cursor, '['))
      {
        skip_json_value(cursor);
        return;
      }

      Array *list = array_create(sizeof(double));

      while (**cursor != '\0' && peek_current(*cursor) != ']')
      {
        skip_whitespace(cursor);

        double val = parse_double(cursor);
        array_add(list, &val);

        skip_whitespace(cursor);
        match_and_consume(cursor, ',');
      }

      match_and_consume(cursor, ']');
      Array **target_ptr = (Array **)((char *)output_instance + field->offset);
      *target_ptr = list;

      break;
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  case REFLECT_TYPE_ARRAY_STRING:
    if (json_type == JSON_TYPE_ARRAY)
    {
      if (!match_and_consume(cursor, '['))
      {
        skip_json_value(cursor);
        return;
      }

      Array *list = array_create(sizeof(char *));

      while (**cursor != '\0' && peek_current(*cursor) != ']')
      {
        skip_whitespace(cursor);

        char *value = parse_string(cursor);
        array_add(list, &value);

        skip_whitespace(cursor);
        match_and_consume(cursor, ',');
      }

      match_and_consume(cursor, ']');

      Array **target_ptr = (Array **)((char *)output_instance + field->offset);
      *target_ptr = list;
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  case REFLECT_TYPE_ARRAY_OBJECT:
    if (json_type == JSON_TYPE_ARRAY)
    {
      if (!match_and_consume(cursor, '['))
      {
        skip_json_value(cursor);
        return;
      }

      t_json_model *child_model = (t_json_model *)field->child_meta;
      if (!child_model)
      {
        skip_json_value(cursor);
        return;
      }

      Array *list = array_create(sizeof(void *));

      while (**cursor != '\0' && peek_current(*cursor) != ']')
      {
        skip_whitespace(cursor);
        void *item_instance = calloc(1, child_model->reflect->size);

        if (peek_current(*cursor) == '{')
        {
          _cjson_decode_internal(cursor, child_model, item_instance);
        }

        array_add(list, &item_instance);

        skip_whitespace(cursor);
        match_and_consume(cursor, ',');
      }

      match_and_consume(cursor, ']');

      Array **target_ptr = (Array **)((char *)output_instance + field->offset);
      *target_ptr = list;
    }
    else
    {
      skip_json_value(cursor);
    }
    break;
  case REFLECT_TYPE_INTEGER:
    if (json_type == JSON_TYPE_NUMBER)
    {
      int val = parse_int(cursor);
      REFLECT_SET(output_instance, field->offset, int, val);
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  case REFLECT_TYPE_DOUBLE:
    if (json_type == JSON_TYPE_NUMBER)
    {
      double val = parse_double(cursor);
      REFLECT_SET(output_instance, field->offset, double, val);
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  case REFLECT_TYPE_STRING:
    if (json_type == JSON_TYPE_STRING)
    {
      char *val = parse_string(cursor);
      REFLECT_SET(output_instance, field->offset, char *, val);
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  case REFLECT_TYPE_BOOL:
    if (json_type == JSON_TYPE_BOOLEAN)
    {
      int val = parse_boolean(cursor);
      if (val != -1)
      {
        REFLECT_SET(output_instance, field->offset, bool, val == 1);
      }
      else
      {
        skip_json_value(cursor);
      }
    }
    else
    {
      skip_json_value(cursor);
    }
    break;

  default:
    skip_json_value(cursor);
    break;
  }
}

double parse_double(const char **cursor)
{
  char *endptr;
  double val = strtod(*cursor, &endptr);

  *cursor = endptr;
  return val;
}

int parse_int(const char **cursor)
{
  char *endptr;
  long val = strtol(*cursor, &endptr, 10);

  if (*endptr == '.')
  {
    double temp = strtod(*cursor, &endptr);
    val = (long)temp;
  }

  *cursor = endptr;
  return (int)val;
}

char *parse_string(const char **cursor)
{
  if (!match_and_consume(cursor, '"'))
    return NULL;

  int len = get_json_string_length(*cursor);
  if (len < 0)
    return NULL;

  char *str = get_string_buffer(len + 1);
  if (!str)
    return NULL;

  char *writer = str;

  consume_until_delimiter(cursor, &writer, '"');

  return str;
}

int parse_boolean(const char **cursor)
{
  if (peek_current(*cursor) == 't')
  {
    if (match_and_consume(cursor, 't') &&
        match_and_consume(cursor, 'r') &&
        match_and_consume(cursor, 'u') &&
        match_and_consume(cursor, 'e'))
    {
      return 1;
    }
    return -1;
  }

  if (peek_current(*cursor) == 'f')
  {
    if (match_and_consume(cursor, 'f') &&
        match_and_consume(cursor, 'a') &&
        match_and_consume(cursor, 'l') &&
        match_and_consume(cursor, 's') &&
        match_and_consume(cursor, 'e'))
    {
      return 0;
    }
    return -1;
  }

  return -1;
}
