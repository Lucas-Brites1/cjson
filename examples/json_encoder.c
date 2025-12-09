#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/string_utils.h"
#include "../include/cjson.h"
#include "../include/dynamic_array.h"

typedef struct
{
  int number;
  char *street;
  char *city;
  char *country;
} Address;

typedef struct
{
  char *type;
  char *name;
  int age;
} Pets;

typedef struct
{
  int age;
  char *name;
  char *email;
  Address *address;
  Array *pets;
} User;

static t_reflect_field pets_fields[] = {
    {"type", REFLECT_TYPE_STRING, REFLECT_OFFSET(Pets, type), NULL},
    {"name", REFLECT_TYPE_STRING, REFLECT_OFFSET(Pets, name), NULL},
    {"age", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(Pets, age), NULL},
    NO_MORE_FIELDS};

static t_json_field_config pets_json_fields[] = {
    {"type", "pet_type", false},
    {"name", "pet_name", false},
    {"age", "pet_age", false},
    NO_MORE_FIELDS};

static t_reflect_field address_fields[] = {
    {"number", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(Address, number), NULL},
    {"street", REFLECT_TYPE_STRING, REFLECT_OFFSET(Address, street), NULL},
    {"country", REFLECT_TYPE_STRING, REFLECT_OFFSET(Address, country), NULL},
    {"city", REFLECT_TYPE_STRING, REFLECT_OFFSET(Address, city), NULL},
    NO_MORE_FIELDS};

static t_json_field_config address_json_fields[] = {
    {"number", "house_number", true},
    {"street", "user_address_street", false},
    {"country", "user_address_county", false},
    {"city", "user_address_city", false},
    NO_MORE_FIELDS};

static t_reflect_field user_fields[] = {
    {"age", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(User, age), NULL},
    {"name", REFLECT_TYPE_STRING, REFLECT_OFFSET(User, name), NULL},
    {"email", REFLECT_TYPE_STRING, REFLECT_OFFSET(User, email), NULL},
    {"pets", REFLECT_TYPE_ARRAY_OBJECT, REFLECT_OFFSET(User, pets), NULL},
    {"address", REFLECT_TYPE_OBJECT, REFLECT_OFFSET(User, address), NULL},
    NO_MORE_FIELDS};

static t_json_field_config user_json_fields[] = {
    {"age", "user_age", false},
    {"name", "user_name", false},
    {"email", NULL, true},
    {"pets", "user_pets", false},
    {"address", NULL, false},
    NO_MORE_FIELDS};

int main(void)
{
  t_json_model *pets_model = cjson_create_model("Pets", sizeof(Pets), pets_fields, pets_json_fields);
  t_json_model *address_model = cjson_create_model("Address", sizeof(Address), address_fields, address_json_fields);
  t_json_model *user_model = cjson_create_model("User", sizeof(User), user_fields, user_json_fields);
  cjson_register_child(user_model, "address", address_model);
  cjson_register_child(user_model, "pets", pets_model);

  Address my_address = {
      .number = 2002,
      .street = "Street Example",
      .city = "Las Vegas",
      .country = "United States"};

  Pets *dog = malloc(sizeof(Pets));
  dog->type = "Dog";
  dog->name = "Rex";
  dog->age = 5;

  Pets *cat = malloc(sizeof(Pets));
  cat->type = "Cat";
  cat->name = "Felix";
  cat->age = 3;

  Array *pets_array = array_create(sizeof(Pets *));
  array_add(pets_array, &dog);
  array_add(pets_array, &cat);

  User user = {
      .age = 25,
      .name = "John Doe",
      .email = "john@test.com", // will be ignored
      .address = &my_address,
      .pets = pets_array};

  printf("--- JSON Compact ---\n");
  char *json_compact = cjson_encode(&user, user_model, false);
  printf("%s\n\n", json_compact);
  free(json_compact);

  printf("--- JSON Pretty ---\n");
  char *json_pretty = cjson_encode(&user, user_model, true);
  printf("%s\n", json_pretty);
  free(json_pretty);

  free(dog);
  free(cat);

  return 0;
}
