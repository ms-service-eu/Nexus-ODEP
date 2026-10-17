# ==============================================================================
# ODEP (Object-Driven Edge Protocol) - Modulares GNU Makefile
# ==============================================================================

# 1. COMPILER & FLAGS DEFINIEREN
CXX      := g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17

# Name der fertigen ausführbaren Datei
TARGET   := odep_device_node

# Verzeichnis für Object-Dateien
OBJ_DIR  := obj

# 2. MODUL-KONFIGURATION (Hier aktivieren/deaktivieren)
ENABLE_SCHEDULER       := 1
ENABLE_VIRTUALIZATION  := 1

# 3. QUELLDATEIEN (MINDSTENS DER KERNEL)
SRCS := main.cpp ODEP_Core.cpp

# 4. CONDITIONALS: MODULE DYNAMISCH HINZUFÜGEN
ifeq ($(ENABLE_SCHEDULER), 1)
    SRCS     += ODEP_Scheduler.cpp
    CXXFLAGS += -DODEP_ENABLE_SCHEDULER
endif

ifeq ($(ENABLE_VIRTUALIZATION), 1)
    SRCS     += ODEP_VirtualDevice.cpp
    CXXFLAGS += -DODEP_ENABLE_VIRTUALIZATION
endif

# KORREKTUR: Erzeugt sauber aus "main.cpp" -> "obj/main.o"
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# ==============================================================================
# PROJEKT-RULES
# ==============================================================================

.PHONY: all clean info run

# Standard-Ziel beim Aufruf von 'make'
all: info $(TARGET)
	@echo "=============================================================================="
	@echo "🎉 Bootloader-Image / Binary [$(TARGET)] erfolgreich erstellt!"
	@echo "=============================================================================="

# Linken des fertigen Binaries
$(TARGET): $(OBJS)
	@echo "🔗 Linke Object-Dateien zu: $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Kompilieren der einzelnen Quellcodedateien
$(OBJ_DIR)/%.o: %.cpp ODEP_Config.h ODEP_Binary.h
	@echo "🔨 Kompiliere $< ..."
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Status-Informationen im Terminal ausgeben
info:
	@echo "=============================================================================="
	@echo "🏗️  ODEP Modulare Kompilation Gestartet"
	@echo "⚙️  Scheduler Module:     $(if $(filter 1,$(ENABLE_SCHEDULER)),AKTIV,DEAKTIVIERT)"
	@echo "⚙️  Virtualization Mode:  $(if $(filter 1,$(ENABLE_VIRTUALIZATION)),PROXY-ACTIVE,STANDALONE)"
	@echo "📂 Quelldateien:         $(SRCS)"
	@echo "📂 Object-Pfad:          $(OBJ_DIR)/"
	@echo "=============================================================================="

# Startet das Programm direkt nach dem Kompilieren ('make run')
run: all
	@echo "🚀 Starte ODEP-Instanz..."
	./$(TARGET)

# Säubert das Verzeichnis von Kompilaten ('make clean')
clean:
	@echo "🧹 Räume alte Object-Dateien und Binaries auf..."
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)
