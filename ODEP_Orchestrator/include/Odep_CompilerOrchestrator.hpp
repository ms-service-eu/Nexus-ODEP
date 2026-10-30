/*
Copyright (C) 2026 Michael Schultz, Bremen-Germany

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#pragma once
#include "OdepArchitecture.h"
#include <string>
#include <cstdlib>
#include <iostream>

namespace Odep {
    namespace Management {

        class CompilerOrchestrator {
        public:
            // Nimmt das ODEP-Hardware-Manifest und den Pfad zum generierten Quellcode entgegen
            bool compileForTarget(const OdepHardwareManifest_t& manifest, 
                                  const std::string& source_file, 
                                  const std::string& output_binary) 
            {
                std::string compiler_cmd = "clang -O3 -std=c99 ";
                
                // Dynamische Target-Zuweisung basierend auf den ODEP-Plattform-Informationen
                switch (manifest.cpu_architecture) {
                    case ODEP_ARCH_X86_64:
                        compiler_cmd += "-target x86_64-pc-linux-gnu ";
                        break;
                        
                    case ODEP_ARCH_ARM_CORTEX_M:
                        // Typischer Cross-Compile Aufruf für eingebettete Controller
                        compiler_cmd += "-target arm-none-eabi -mcpu=cortex-m4 -mthumb -nostdlib ";
                        break;
                        
                    case ODEP_ARCH_AVR_8BIT:
                        compiler_cmd += "-target avr -mmcu=atmega328p ";
                        break;
                        
                    default:
                        std::cerr << "[Orchestrator] Unbekannte oder nicht unterstützte Ziel-Architektur!\n";
                        return false;
                }

                // Definition der RAM-Grenzen als Compiler-Makros in das Binary injizieren
                compiler_cmd += "-DODEP_RAM_LIMIT=" + std::to_string(manifest.available_ram_bytes) + " ";

                // Finaler Befehlsaufruf
                compiler_cmd += "-o " + output_binary + " " + source_file;

                std::cout << "[Orchestrator] Rufe Cross-Compiler auf:\n -> " << compiler_cmd << "\n";
                
                // Führt den Compiler-Prozess auf dem Host-System aus
                int result = std::system(compiler_cmd.c_str());
                return (result == 0);
            }
        };
    }
}
