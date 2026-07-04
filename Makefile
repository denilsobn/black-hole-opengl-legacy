
ifeq ($(OS),Windows_NT)
    UNAME_S := Windows
else
    UNAME_S := $(shell uname -s)
endif

OS_LABEL := $(UNAME_S)
ifeq ($(UNAME_S),Darwin)
    OS_LABEL := MacOS
endif

.PHONY: all lite clean help check_os run

SRCS_BASE = main.cpp

OBJS_BASE = $(SRCS_BASE:.cpp=.o)

EXEC ?= simulacao

ifeq ($(UNAME_S),Windows)
    OUT = $(EXEC).exe
    LIBS_BASE = -lopengl32 -lglu32 -lfreeglut -lIL
    LIB_ASSIMP = -lassimp
endif

ifeq ($(UNAME_S),Linux)
    OUT = $(EXEC)
    LIBS_BASE = -lGL -lGLU -lglut -lIL
    LIB_ASSIMP = -lassimp
endif

ifeq ($(UNAME_S),Darwin)
    OUT = $(EXEC)
    LIBS_BASE = -L/opt/homebrew/lib -framework OpenGL -framework GLUT -lIL
    LIB_ASSIMP = -lassimp
endif


%.o: %.cpp
	@echo "Compilando arquivo: $<"
	@g++ -c $< -o $@ $(INCLUDES)

all: check_os run

$(OUT): $(OBJS_BASE)
	@echo ""
	@echo "==========   Linkando o executavel final: $@"
	@g++ -o $@ $^ $(LIBS_BASE) 
	@echo "==========   Sucesso! Digite './$(OUT)' para executar."
	@echo ""

lite: check_os $(OBJS_BASE)
	@echo ""
	@echo "==========   Linkando versao LEVE (sem assimp): $(OUT)"
	@g++ -o $(OUT) $(OBJS_BASE) $(LIBS_BASE)
	@echo "==========   Sucesso! Digite './$(OUT)' para executar."
	@echo ""

check_os:
	@echo ""
	@echo "==========   Iniciando compilacao para: $(OS_LABEL) =========="

clean:
	@echo ""
	@echo "==========   Limpando arquivos compilados (.o) e o executavel..."
	@rm -f $(OUT) $(OBJS_BASE)
	@echo "==========   Limpeza concluida."
	@echo ""

run: $(OUT)
	@echo "==========   Executando o programa... =========="
	@./$(OUT)
