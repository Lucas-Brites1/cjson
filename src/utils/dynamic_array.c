#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/dynamic_array.h"

Array *array_create(t_size element_size)
{
  if (element_size <= 0)
    return NULL;

  Array *arr = (Array *)malloc(sizeof(Array));
  if (!arr)
    return NULL;

  arr->capacity = 8;
  arr->count = 0;
  arr->element_size = element_size;

  arr->data = malloc(arr->capacity * element_size);

  if (!arr->data)
  {
    free(arr);
    return NULL;
  }

  return arr;
}

void array_add(Array *array, void *item_ptr)
{
  if (array == NULL || item_ptr == NULL)
    return;

  if (array->count >= array->capacity)
  {
    t_size new_capacity = array->capacity * 2;

    void *temp = realloc(array->data, new_capacity * array->element_size);

    if (!temp)
    {
      return;
    }

    array->data = temp;
    array->capacity = new_capacity;
  }

  void *destination = (char *)array->data + (array->count * array->element_size);

  memcpy(destination, item_ptr, array->element_size);

  array->count++;
}

void array_free(Array *array)
{
  if (array == NULL)
    return;

  if (array->data)
    free(array->data);

  free(array);
}
