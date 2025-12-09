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

  const char *json_payload =
      "{\n"
      "  \"user_age\": 25,\n"
      "  \"user_name\": \"John Doe\",\n"
      "  \"user_pets\": [\n"
      "    {\n"
      "      \"pet_type\": \"Dog\",\n"
      "      \"pet_name\": \"Rex\",\n"
      "      \"pet_age\": 5\n"
      "    },\n"
      "    {\n"
      "      \"pet_type\": \"Cat\",\n"
      "      \"pet_name\": \"Felix\",\n"
      "      \"pet_age\": 3\n"
      "    }\n"
      "  ],\n"
      "  \"address\": {\n"
      "    \"user_address_street\": \"Street Example\",\n"
      "    \"user_address_county\": \"United States\",\n"
      "    \"user_address_city\": \"Las Vegas\"\n"
      "  }\n"
      "}";

  printf("--- JSON Input ---\n%s\n\n", json_payload);

  User *decoded_user = calloc(1, sizeof(User));

  cjson_decode(json_payload, user_model, decoded_user);

  printf("--- Decoded Struct Data ---\n");

  printf("Name: %s\n", decoded_user->name ? decoded_user->name : "NULL");
  printf("Age: %d\n", decoded_user->age);
  printf("Email: %s (Should be NULL/Empty because of logic)\n", decoded_user->email ? decoded_user->email : "NULL");

  if (decoded_user->address)
  {
    printf("Address Street: %s\n", decoded_user->address->street);
    printf("Address City: %s\n", decoded_user->address->city);
    printf("Address Country: %s\n", decoded_user->address->country);
  }
  else
  {
    printf("Address: NULL (Error)\n");
  }

  if (decoded_user->pets)
  {
    printf("Pets Count: %lu\n", decoded_user->pets->count);

    Pets **pets_list = (Pets **)decoded_user->pets->data;

    for (size_t i = 0; i < decoded_user->pets->count; i++)
    {
      Pets *p = pets_list[i];
      printf("  [%lu] %s is a %s (%d years old)\n", i, p->name, p->type, p->age);
    }
  }
  else
  {
    printf("Pets: NULL (Error)\n");
  }

  return 0;
}
