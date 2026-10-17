# ==============================================================================
# ODP (Object-Driven Edge Protocol) - Modulares GNU Makefile
# ==============================================================================

# 1. COMPILER & FLAGS DEFINIEREN
CXX      := g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17

# Name der fertigen ausführbaren Datei
TARGET   := odp_device_node

# 2. MODUL-KONFIGURATION (Hier aktivieren/deaktivieren)
# Setze die Variablen auf 1 zum Einschalten oder 0 zum Ausschalten.
# Dies korrespondiert mit den Defines in der ODP_Config.h.
ENABLE_SCHEDULER       := 1
ENABLE_VIRTUALIZATION  := 1

# 3. QUELLDATEIEN (MINDSTENS DER KERNEL)
SRCS := main.cpp ODP_Core.cpp

# 4. CONDITIONALS: MODULE DYNAMISCH HINZUFÜGEN
ifeq ($(ENABLE_SCHEDULER), 1)
    SRCS     += ODP_Scheduler.cpp
    CXXFLAGS += -DODP_ENABLE_SCHEDULER
endif

ifeq ($(ENABLE_VIRTUALIZATION), 1)
    SRCS     += ODP_VirtualDevice.cpp
    CXXFLAGS += -DODP_ENABLE_VIRTUALIZATION
endif

# Automatische Generierung der Object-Dateien (.cpp -> .o)
OBJS := $(SRCS:.cpp=.o)

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
%.o: %.cpp ODP_Config.h ODP_Binary.h
	@echo "🔨 Kompiliere $< ..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Status-Informationen im Terminal ausgeben
info:
	@echo "=============================================================================="
	@echo "🏗️  ODP Modulare Kompilation Gestartet"
	@echo "⚙️  Scheduler Module:     $(if $(filter 1,$(ENABLE_SCHEDULER)),AKTIV,DEAKTIVIERT)"
	@echo "⚙️  Virtualization Mode:  $(if $(filter 1,$(ENABLE_VIRTUALIZATION)),PROXY-ACTIVE,STANDALONE)"
	@echo "📂 Quelldateien:         $(SRCS)"
	@echo "=============================================================================="

# Startet das Programm direkt nach dem Kompilieren ('make run')
run: all
	@echo "🚀 Starte ODP-Instanz..."
	./$(TARGET)

# Säubert das Verzeichnis von Kompilaten ('make clean')
clean:
	@echo "🧹 Räume alte Object-Dateien und Binaries auf..."
	rm -f $(OBJS) $(TARGET)
