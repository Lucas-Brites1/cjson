# cjson

**cjson** is a lightweight, reflection-based JSON serializer and deserializer for C.

Unlike traditional C JSON parsers where you manually traverse JSON nodes to populate your structs, **cjson** uses metadata maps (built on top of `creflect`) to **automatically map structs to JSON and vice-versa** without repetitive boilerplate code.

## ✨ Features

* **Reflection-Based:** Define your struct layout once, and serialization/deserialization happens automatically.
* **Zero Boilerplate:** No need to write manual parsing logic for every single field.
* **Nested Support:** Handles nested objects and arrays (Strings, Integers, Doubles, Objects).
* **Portable:** Works on **Linux**, **macOS**, and **Windows** (MinGW) with a unified build system.
* **Dependencies:** Built on top of `creflect` (a header-only reflection helper).

---

## 🛠️ Build & Installation

This project uses a **Universal Makefile** that automatically detects your Operating System (Windows or Linux/Unix).

### Prerequisites

* **GCC** (MinGW on Windows, `build-essential` on Linux)
* **Make**

### Compilation Instructions

1.  **Build the static library (`libcjson.a`)**:
    ```bash
    make
    ```

2.  **Build the library and example executables**:
    ```bash
    make examples
    ```

3.  **Clean build artifacts**:
    ```bash
    make clean
    ```

### Running Examples

After running `make examples`, the executables are generated in the root folder.

**Linux/WSL/macOS:**
```bash
./decoder_example
./encoder_example
```

**Windows (CMD/PowerShell):**
```bash
.\decoder_example.exe
.\encoder_example.exe
```

---

## 🚀 Usage Example

cjson works by defining a metadata table that describes your struct. This allows the library to "know" the memory layout of your data.

### 1. Define your Structs (Models)

```c
typedef struct {
    char *city;
    int zip_code;
} Address;

typedef struct {
    char *name;
    int age;
    double height;
    bool is_active;
    Address address; // Nested Object
} User;
```

### 2. Define the Metadata (Reflection)

Using macros from `deps/creflect/reflection.h`:

```c
#include "cjson.h"

// Define Address Metadata
t_reflect_field address_fields[] = {
    {"city", REFLECT_TYPE_STRING, REFLECT_OFFSET(Address, city), NULL},
    {"zip_code", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(Address, zip_code), NULL}
};

t_reflect_object address_meta = {
    .name = "Address",
    .size = sizeof(Address),
    .field_count = 2,
    .fields = address_fields
};

// Define User Metadata
t_reflect_field user_fields[] = {
    {"name", REFLECT_TYPE_STRING, REFLECT_OFFSET(User, name), NULL},
    {"age", REFLECT_TYPE_INTEGER, REFLECT_OFFSET(User, age), NULL},
    {"height", REFLECT_TYPE_DOUBLE, REFLECT_OFFSET(User, height), NULL},
    {"is_active", REFLECT_TYPE_BOOL, REFLECT_OFFSET(User, is_active), NULL},
    {"address", REFLECT_TYPE_OBJECT, REFLECT_OFFSET(User, address), &address_meta}
};

t_reflect_object user_meta = {
    .name = "User",
    .size = sizeof(User),
    .field_count = 5,
    .fields = user_fields
};
```

### 3. Serialize/Encode (Struct -> JSON)

```c
User user = {
    .name = "Lucas",
    .age = 25,
    .height = 1.75,
    .is_active = true,
    .address = { .city = "Campinas", .zip_code = 13000 }
};

char *json_string = cjson_encode(&user, &user_meta);
printf("%s\n", json_string);
// Output: {"name": "Lucas", "age": 25, ...}

free(json_string);
```

### 4. Deserialize/Decoder (JSON -> Struct)

```c
char *input = "{\"name\": \"Lucas\", \"age\": 25, \"address\": {\"city\": \"SP\"}}";
User *new_user = (User*) cjson_decode(input, &user_meta);

printf("Name: %s, City: %s\n", new_user->name, new_user->address.city);

// Clean up
cjson_free(new_user, &user_meta);
```

> 💡 **Tip:** Check the programs in [`examples/`](examples/) to see full demonstrations of
> serialization (`encoder_example`) and deserialization (`decoder_example`) in action.

---

## 📂 Project Structure

```
cjson/
├── deps/              # External dependencies
│   └── creflect/      # Reflection header (reflection.h)
├── examples/          # Usage examples (decoder/encoder)
├── include/           # Public headers (cjson.h, etc.)
├── src/               # Source code
│   ├── utils/         # Helper modules (string, dynamic arrays)
│   ├── cjson.c        # Core logic
│   ├── json_decoder.c # JSON Parser implementation
│   └── json_encoder.c # JSON Stringifier implementation
└── Makefile           # Universal Build System
```

---

## 📄 License

This project is open-source. Please check the LICENSE file for details.
