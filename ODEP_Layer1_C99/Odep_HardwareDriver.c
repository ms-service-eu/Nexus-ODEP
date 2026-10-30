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


#include "odep_l1.h"

void odep_hw_raw_output(const uint8_t* data, uint16_t size) {
    /* Unterdrückt die Unused-Parameter Warnung für den leeren Hardware-Mock */
    (void)data; 
    (void)size;

    /* 
     * Sobald du echten Hardware-Code einfügst, kannst du diese Schleife nutzen:
     *
     * for (uint16_t i = 0; i < size; i++) {
     *     // UDR0 = data[i]; // Beispiel für AVR UART
     * }
     */
}
