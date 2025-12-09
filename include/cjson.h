#ifndef CJSON_H
#define CJSON_H
#include <stdbool.h>
#include "../deps/creflect/reflection.h"

#define NO_MORE_FIELDS {NULL, 0, 0}

typedef struct
{
  const char *field_name;      // reflection field name default
  const char *json_field_name; // provided by annotation like @Json("another_name_here")
  bool ignore;                 // 0 false, 1 true : basically that field is a flag to show or not an information on json
} t_json_field_config;

typedef struct
{
  t_reflect_object *reflect;
  t_json_field_config *fields_config;
} t_json_model;

typedef enum
{
  JSON_TYPE_OBJECT,  // Começa com {
  JSON_TYPE_ARRAY,   // Começa com [
  JSON_TYPE_STRING,  // Começa com "
  JSON_TYPE_NUMBER,  // Começa com 0-9 ou -
  JSON_TYPE_BOOLEAN, // Começa com t ou f
  JSON_TYPE_NULL,    // Começa com n
  JSON_TYPE_UNKNOWN  // Erro ou lixo
} t_json_type;

char *cjson_encode(void *data, t_json_model *model, bool pretty);
int cjson_decode(const char *json, t_json_model *metadata_json, void *output_instance); // string -> object
char *parse_key(const char **cursor);

void cjson_free(char *json_string);
void cjson_free_instance(void *instance, t_json_model *model);

t_json_model *cjson_create_model(const char *struct_name, t_size struct_size, t_reflect_field *fields, t_json_field_config *configs);
bool cjson_register_child(t_json_model *parent_model, const char *child_field_name, t_json_model *child_model);

#endif
