# CJSON-Reflect

**CJSON-Reflect** is a lightweight, reflection-based JSON serializer and deserializer for C.

Unlike traditional C JSON libraries that require manual mapping of every field for every operation, **CJSON-Reflect** uses a unified metadata system to automatically map C `structs` to JSON objects and vice-versa, significantly reducing boilerplate code and maintenance effort.

## 🚀 Features

- **Reflection-Based:** Define your struct metadata once, use it for both encoding and decoding.
- **Bi-directional:**
  - **Encoder:** Struct -> JSON String (supports Compact & Pretty Print).
  - **Decoder:** JSON String -> Struct (with automatic memory allocation for strings, arrays, and nested objects).
- **Type Support:** Handles `int`, `string`, `bool`, nested `objects`, and `arrays` (of primitives or objects).
- **Zero-Dependency:** Core logic depends only on the internal `creflect` module (included).
- **Customizable:** easy to ignore fields or map C field names (e.g., `user_id`) to different JSON keys (e.g., `id`).

## 📦 Installation

This project uses git submodules. To clone it with all dependencies:

```bash
git clone --recursive [https://github.com/YOUR_USERNAME/cjson-reflect.git](https://github.com/YOUR_USERNAME/cjson-reflect.git)
If you have already cloned it without submodules, run:

Bash

git submodule update --init --recursive
🛠️ Usage
1. Define your Structs and Metadata
You need to define your C structs and the corresponding reflection metadata.

C

typedef struct {
    char *name;
    int age;
} User;

// 1. Define field mapping (Reflection)
static t_reflect_field user_fields[] = {
    {"name", REFLECT_TYPE_STRING, REFLECT_OFFSET(User, name), NULL},
    {"age", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(User, age), NULL},
    NO_MORE_FIELDS
};

// 2. Define JSON configuration (Renaming or Ignoring fields)
static t_json_field_config user_json_config[] = {
    {"name", "full_name", false}, // Maps C "name" -> JSON "full_name"
    {"age", NULL, false},         // Keeps "age" -> JSON "age"
    NO_MORE_FIELDS
};
2. Encoding (Struct to JSON)
C

// Initialize model
t_json_model *model = cjson_create_model("User", sizeof(User), user_fields, user_json_config);

// Create instance
User user = {.name = "Lucas", .age = 25};

// Encode
char *json = cjson_encode(&user, model, true); // true = Pretty Print
printf("%s\n", json);

free(json);
3. Decoding (JSON to Struct)
C

const char *json_input = "{\"full_name\": \"Lucas\", \"age\": 25}";

// Prepare empty struct
User *new_user = calloc(1, sizeof(User));

// Decode
cjson_decode(json_input, new_user, model);

printf("Name: %s, Age: %d\n", new_user->name, new_user->age);

// Cleanup
free(new_user->name); // Strings are auto-allocated
free(new_user);
📂 Project Structure
src/: Core implementation (Encoder, Decoder, Utils).

include/: Header files (cjson.h, dynamic_array.h).

deps/: Dependencies (creflect submodule).

examples/: Ready-to-run examples for encoding and decoding.

🤝 Contributing
Feel free to submit issues and enhancement requests.

📄 License
This project is licensed under the MIT License.
