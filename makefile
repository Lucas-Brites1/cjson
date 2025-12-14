# ==========================================
#  Makefile Universal para CJson
# ==========================================

CC = gcc
# Adicionei -Ideps/creflect caso seu código precise do reflection.h
CFLAGS = -Wall -Wextra -Iinclude -Ideps/creflect

# --- DETECÇÃO DE SISTEMA OPERACIONAL ---
ifdef OS
   # Windows
   RM = del /Q
   FixPath = $(subst /,\,$1)
   EXEC_EXT = .exe
   LIB_EXT = .a
   # No Windows, mkdir -p é chato, então compilamos os objetos junto com os fontes
else
   # Linux / Unix
   RM = rm -f
   FixPath = $1
   EXEC_EXT =
   LIB_EXT = .a
endif

# --- DEFINIÇÃO DE ARQUIVOS ---

# Pega todos os .c da raiz src
SRC_CORE = $(wildcard src/*.c)

# Pega todos os .c da pasta utils
SRC_UTILS = $(wildcard src/utils/*.c)

# Junta tudo na lista de fontes da biblioteca
LIB_SRC = $(SRC_CORE) $(SRC_UTILS)

# Gera a lista de objetos (.o) trocando .c por .o
LIB_OBJ = $(LIB_SRC:.c=.o)

# Nome da Biblioteca Final
TARGET_LIB = libcjson$(LIB_EXT)

# --- DEFINIÇÃO DOS EXEMPLOS ---
# Aqui configuramos para compilar os exemplos da pasta 'examples'
EX_DEC_SRC = examples/json_decoder.c
EX_DEC_BIN = decoder_example$(EXEC_EXT)

EX_ENC_SRC = examples/json_encoder.c
EX_ENC_BIN = encoder_example$(EXEC_EXT)

# --- REGRAS DE COMPILAÇÃO ---

# Regra padrão: cria apenas a biblioteca
all: $(TARGET_LIB)

# 1. Cria a Biblioteca Estática (.a)
# Compila tudo de src/ e src/utils/ e empacota
$(TARGET_LIB): $(LIB_OBJ)
	ar rcs $@ $^

# Regra genérica para transformar qualquer .c em .o
# Isso funciona tanto para src/ quanto para src/utils/
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# 2. Compila os Exemplos
# Linka com a biblioteca que acabamos de criar (-L. -lcjson)
examples: $(TARGET_LIB) $(EX_DEC_BIN) $(EX_ENC_BIN)

$(EX_DEC_BIN): $(EX_DEC_SRC)
	$(CC) $(EX_DEC_SRC) -o $@ -Iinclude -L. -lcjson

$(EX_ENC_BIN): $(EX_ENC_SRC)
	$(CC) $(EX_ENC_SRC) -o $@ -Iinclude -L. -lcjson

# --- LIMPEZA ---
clean:
	$(RM) $(call FixPath,$(TARGET_LIB))
	$(RM) $(call FixPath,$(EX_DEC_BIN))
	$(RM) $(call FixPath,$(EX_ENC_BIN))
	$(RM) $(call FixPath,src/*.o)
	$(RM) $(call FixPath,src/utils/*.o)

# Atalho para limpar e compilar tudo do zero
rebuild: clean all
