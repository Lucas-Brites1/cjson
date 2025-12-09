#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stddef.h>
#include <stdbool.h>
#include "./cjson.h"

typedef struct
{
  t_size element_size;
  t_size count;
  t_size capacity;
  void *data;
} Array;

// Mudei de bool para Array* (Retorna o objeto criado)
Array *array_create(t_size element_size);

void array_add(Array *array, void *item_ptr);
void array_free(Array *array);

#endif
