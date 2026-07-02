# 1. Detecta o Sistema Operacional
ifeq ($(OS),Windows_NT)
    UNAME_S := Windows
else
    UNAME_S := $(shell uname -s)
endif

# Nome amigável do SO para exibição
OS_LABEL := $(UNAME_S)
ifeq ($(UNAME_S),Darwin)
    OS_LABEL := MacOS
endif

.PHONY: all lite clean help check_os run

# 2. Separação dos arquivos fonte (.cpp)
SRCS_BASE = main.cpp bib/Camera.cpp bib/CameraDistante.cpp bib/CameraJogo.cpp bib/Desenha.cpp bib/Vetor3D.cpp bib/Transform.cpp gui_glut/extra.cpp gui_glut/gui.cpp gui_glut/OpenTextures.cpp
SRC_ASSIMP = bib/Model3D.cpp

# Transforma a lista de .cpp em uma lista de .o (Arquivos Objeto)
OBJS_BASE = $(SRCS_BASE:.cpp=.o)
OBJ_ASSIMP = $(SRC_ASSIMP:.cpp=.o)

# Nome do executável
EXEC ?= GLUTdoZero20261

# 3. Configurações específicas por Sistema Operacional
ifeq ($(UNAME_S),Windows)
    OUT = run/$(EXEC).exe
    INCLUDES = -I bib -I gui_glut
    LIBS_BASE = -lopengl32 -lglu32 -lfreeglut -lIL
    LIB_ASSIMP = -lassimp
endif

ifeq ($(UNAME_S),Linux)
    OUT = run/$(EXEC)
    INCLUDES = -I bib -I gui_glut
    LIBS_BASE = -lGL -lGLU -lglut -lIL
    LIB_ASSIMP = -lassimp
endif

ifeq ($(UNAME_S),Darwin)
    OUT = run/$(EXEC)
    INCLUDES = -I bib -I gui_glut -I/opt/homebrew/include -I mac -Wno-deprecated-declarations -Wno-macro-redefined
    LIBS_BASE = -L/opt/homebrew/lib -framework OpenGL -framework GLUT -lIL
    LIB_ASSIMP = -lassimp
endif


# 4. Regras de compilação

# A regra que ensina o Makefile a compilar cada .cpp em um .o de forma isolada
%.o: %.cpp
	@echo "Compilando arquivo: $<"
	@g++ -c $< -o $@ $(INCLUDES)

# Regra padrão (Basta digitar 'make')
all: check_os run

# Regra para construir o executável final juntando todos os .o
$(OUT): $(OBJS_BASE) $(OBJ_ASSIMP)
	@echo ""
	@echo "==========   Linkando o executavel final: $@"
	@g++ -o $@ $^ $(LIBS_BASE) $(LIB_ASSIMP)
	@echo "==========   Sucesso! Digite './$(OUT)' para executar."
	@echo ""

# Regra rápida: Compila SEM Assimp
lite: check_os $(OBJS_BASE)
	@echo ""
	@echo "==========   Linkando versao LEVE (sem assimp): $(OUT)"
	@g++ -o $(OUT) $(OBJS_BASE) $(LIBS_BASE)
	@echo "==========   Sucesso! Digite './$(OUT)' para executar."
	@echo ""

# Mensagem inicial padronizada
check_os:
	@echo ""
	@echo "==========   Iniciando compilacao para: $(OS_LABEL) =========="

# Regra de limpeza: Agora apaga o executavel E os arquivos residuais .o
clean:
	@echo ""
	@echo "==========   Limpando arquivos compilados (.o) e o executavel..."
	@rm -f $(OUT) $(OBJS_BASE) $(OBJ_ASSIMP)
	@echo "==========   Limpeza concluida."
	@echo ""

# Regra para compilar (se necessario) e executar o programa logo em seguida
run: $(OUT)
	@echo "==========   Executando o programa... =========="
	@./$(OUT)

# Ajuda
help:
	@echo ""
	@echo "==========   Ajuda para o Makefile =========="
	@echo "  make             -> Compila de forma incremental tudo (com Assimp)"
	@echo "  make lite        -> Compila de forma incremental a versao leve"
	@echo "  make clean       -> Remove o executavel e os arquivos objeto (.o)"
	@echo "  make EXEC=meu_app -> Personaliza o nome da saida"
	@echo ""