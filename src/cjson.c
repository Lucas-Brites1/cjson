#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cjson.h"
#include "../include/dynamic_array.h"

t_size count_fields(t_reflect_field *fields);

t_json_model *cjson_create_model(const char *struct_name, t_size struct_size, t_reflect_field *fields, t_json_field_config *configs)
{
  if (struct_name == NULL || struct_size <= 0 || fields == NULL || configs == NULL)
    return NULL;

  t_json_model *model = (t_json_model *)calloc(1, sizeof(t_json_model));
  if (!model)
    return NULL;

  t_reflect_object *r_obj = (t_reflect_object *)calloc(1, sizeof(t_reflect_object));
  if (!r_obj)
    return NULL;

  r_obj->name = struct_name;
  r_obj->size = struct_size;
  r_obj->field_count = count_fields(fields);
  r_obj->fields = fields;

  model->reflect = r_obj;
  model->fields_config = configs;

  return model;
}

t_size count_fields(t_reflect_field *fields)
{
  t_size count = 0;
  while (fields[count].name != NULL)
    count++;

  return count;
}

bool cjson_register_child(t_json_model *parent_model, const char *child_field_name, t_json_model *child_model)
{
  if (!parent_model || !child_field_name || !child_model)
    return false;

  t_reflect_field *fields = parent_model->reflect->fields;
  int i = 0;

  while (fields[i].name != NULL)
  {
    if (strcmp(fields[i].name, child_field_name) == 0)
    {
      fields[i].child_meta = child_model;
      return true;
    }
    i++;
  }

  return false;
}

void cjson_free_instance(void *instance, t_json_model *model)
{
  if (!instance || !model)
    return;

  t_reflect_field *fields = model->reflect->fields;
  int i = 0;

  while (fields[i].name != NULL)
  {
    t_reflect_field *field = &fields[i];
    void *field_ptr = (char *)instance + field->offset;

    switch (field->type)
    {
    case REFLECT_TYPE_STRING:
    {
      char **str_ptr = (char **)field_ptr;
      if (*str_ptr)
      {
        free(*str_ptr);
        *str_ptr = NULL;
      }
      break;
    }
    case REFLECT_TYPE_ARRAY_STRING:
    {
      Array **arr_ptr = (Array **)field_ptr;
      if (*arr_ptr)
      {
        Array *arr = *arr_ptr;

        char **strings = (char **)arr->data;
        for (t_size k = 0; k < arr->count; k++)
        {
          if (strings[k])
          {
            free(strings[k]);
          }
        }
        array_free(arr);
        *arr_ptr = NULL;
      }
      break;
    }
    case REFLECT_TYPE_OBJECT:
    {
      t_json_model *child_model = (t_json_model *)field->child_meta;
      void **child_struct_ptr = (void **)field_ptr;
      if (*child_struct_ptr)
      {
        cjson_free_instance(*child_struct_ptr, child_model);
        free(*child_struct_ptr);
        *child_struct_ptr = NULL;
      }
      break;
    }
    case REFLECT_TYPE_ARRAY_INT:
    case REFLECT_TYPE_ARRAY_DOUBLE:
    {
      Array **arr_ptr = (Array **)field_ptr;
      if (*arr_ptr)
      {
        array_free(*arr_ptr);
        *arr_ptr = NULL;
      }
      break;
    }

    default:
      break;
    }
    i++;
  }
}
