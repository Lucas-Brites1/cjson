# CJSON-Reflect

A lightweight, reflection-based JSON serializer and deserializer for C.

Unlike traditional C JSON libraries that require manual field mapping for every operation, **CJSON-Reflect** uses a unified metadata system to automatically map C structs to JSON objects — and vice-versa — significantly reducing boilerplate and maintenance effort.

## Features

- **Reflection-Based** — Define struct metadata once, reuse for both encoding and decoding
- **Bi-directional** — Struct → JSON (compact or pretty-printed) and JSON → Struct
- **Rich Type Support** — Handles `int`, `string`, `bool`, nested objects, and arrays (primitives or objects)
- **Zero External Dependencies** — Only relies on the bundled `creflect` module
- **Flexible Field Mapping** — Rename fields (`user_id` → `id`) or ignore them entirely

## Installation

Clone with submodules:

```bash
git clone --recursive https://github.com/YOUR_USERNAME/cjson-reflect.git
```

Already cloned without submodules?

```bash
git submodule update --init --recursive
```

## Quick Start

### 1. Define Structs and Metadata

```c
typedef struct {
    char *name;
    int age;
} User;

// Field reflection metadata
static t_reflect_field user_fields[] = {
    {"name", REFLECT_TYPE_STRING,  REFLECT_OFFSET(User, name), NULL},
    {"age",  REFLECT_TYPE_INTEGER, REFLECT_OFFSET(User, age),  NULL},
    NO_MORE_FIELDS
};

// Optional: JSON field configuration (renaming/ignoring)
static t_json_field_config user_json_config[] = {
    {"name", "full_name", false},  // C "name" → JSON "full_name"
    {"age",  NULL,        false},  // Keep as "age"
    NO_MORE_FIELDS
};

// Create the model
t_json_model *model = cjson_create_model(
    "User", sizeof(User), user_fields, user_json_config
);
```

### 2. Encoding (Struct → JSON)

```c
User user = {.name = "Lucas", .age = 25};

char *json = cjson_encode(&user, model, true);  // true = pretty print
printf("%s\n", json);
// Output:
// {
//     "full_name": "Lucas",
//     "age": 25
// }

free(json);
```

### 3. Decoding (JSON → Struct)

```c
const char *json_input = "{\"full_name\": \"Lucas\", \"age\": 25}";

User *new_user = calloc(1, sizeof(User));
cjson_decode(json_input, new_user, model);

printf("Name: %s, Age: %d\n", new_user->name, new_user->age);
// Output: Name: Lucas, Age: 25

// Cleanup (strings are heap-allocated during decode)
free(new_user->name);
free(new_user);
```

## Project Structure

```
cjson-reflect/
├── src/          # Core implementation (encoder, decoder, utils)
├── include/      # Public headers (cjson.h, dynamic_array.h)
├── deps/         # Dependencies (creflect submodule)
└── examples/     # Ready-to-run examples
```

## Contributing

Issues and pull requests are welcome!

## License

MIT License
