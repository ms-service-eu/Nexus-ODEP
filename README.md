# Nexus Object-Driven Edge Protocol (Nexus-ODEP)

![DOI](https://zenodo.org/records/23427131)(https://doi.org/10.5281/zenodo.23427130)]



## ⚠️ LEGAL NOTICE: OFFICIAL PRIOR ART SPECIFICATION

**This document and the associated source code constitute an official, dated, and public first publication (State of the Art / Prior Art) under international patent examination guidelines.**

* **Cryptographic Timestamp:** This framework is registered across immutable, tamper-proof repositories ([GitHub](https://github.com) / [Zenodo DOI](https://zenodo.org)) securing a globally recognized and legally binding timestamp.
* **Patent Protection for the Public Domain:** Any future attempts by third-party manufacturers or corporations to patent the core methodologies described herein—specifically the decentralized resource-safety engine, time-slotted future edge allocation, or device-level hardware virtualization via Virtual Devices—are hereby legally blocked worldwide.
* **Intellectual Property \& Trademark Rights:** The designation **"Nexus-ODEP"** (Nexus Object-Driven Edge Protocol) and its structural protocol architecture are inherently tied to this open-source project and its original author from the exact date of this publication. Any bad-faith trademark registrations of this name by third parties within the fields of network control, automation, or entertainment technology will be legally contested based on documented first authorship and established common-law trade dress.

*Usage, modification, forks, and commercial deployment are explicitly encouraged and permitted under the terms of the Apache License 2.0.*

\---

## 🚀 Was ist Nexus-ODEP?

**Nexus-ODEP** ist ein universelles, quelloffenes Meta- und Rahmenprotokoll für eingebettete Systeme, das Internet der Dinge (IoT), Industrie 4.0 und die professionelle Veranstaltungstechnik.

Es bricht radikal mit dem traditionellen Prinzip des zentralisierten, kontinuierlichen Daten-Streamings (wie DMX-512, Art-Net, sACN) oder reiner Cloud-Abhängigkeit. Durch konsequentes **Edge Computing** verlagert das Protokoll die funktionale Logik, die zeitbasierte Ressourcen-Planung und die hardwareseitige Konfliktlösung direkt auf die dezentralen Knotenpunkte (**Peer-to-Peer**).

Nexus-ODEP ist vollständig **payload-agnostisch**: Es definiert keine branchenspezifischen Anwendungsbefehle, sondern das administrative, zeitgesteuerte Rahmenwerk für deren konfliktfreie Ausführung.

\---

## 🧠 Hauptmerkmale

* **🔒 Dezentrales Ressourcen-Management:** Hardware-Fähigkeiten werden als Ressourcen definiert. Controller können diese via `EXCLUSIVE` oder `SHARED` (mit integrierten Prioritäten-Queues) direkt auf dem Gerät buchen – ohne zentralen Master-Server.
* **📅 Zeitgesteuerte Reservierungen (Time-Slotted Allocation):** Dank hochgenauer Zeitsynchronisation (IEEE 1588 PTP / NTP) können kritische Steuerungsfenster nanosekundengenau für die Zukunft reserviert werden, inklusive automatischer Kollisionsprüfung und Verdrängung (`Eviction`).
* **🌐 Virtuelle Geräte (Virtual Devices):** Rechenstarke Edge-Knoten können als intelligente Proxys agieren. Sie aggregieren einfache Sub-Geräte (z. B. serielle Sensoren, Aktuatoren) zu logischen, virtuellen Objekten und kapseln deren Komplexität vor dem Netzwerk.
* **☁️ Native Internet-Resilienz (Buffer-Ahead):** Eine integrierte *Buffer-Ahead-Engine* erlaubt die zeitlich entkoppelte Übertragung von Kontroll-Rahmen. Das Gerät puffert zukünftige Befehle vorab, wodurch unvorhersehbarer Internet-Jitter, Paketverluste oder Mobilfunk-Schwankungen vollständig kompensiert werden.

\---

## 📂 Paketstruktur (Vorschlag)

```text

├── ODEP\\\\\\\\\\\\\\\_Config.h           # Die Schaltzentrale (Funktionsumfang wählen)

├── ODEP\\\\\\\\\\\\\\\_Core.h / .cpp      # Schicht 1 Basis (Ressourcen \\\\\\\\\\\\\\\& Direkt-Rechte)

├── ODEP\\\\\\\\\\\\\\\_Scheduler.h / .cpp # Optionale Zeitsteuerung (Zukunftsbuchung)

├── ODEP\\\\\\\\\\\\\\\_VirtualDevice.h    # Optionale Proxy-Schicht (Virtual Devices)

├── NODEP\\\\\\\\\\\\\\\_Binary.h         # Hocheffiziente Binärstruktur (z.B. für DMX/Serial)

├── Makefile               # Modulares GNU Makefile für GCC/G++

└── README.md              # Diese Dokumentation

```

\---

## 💻 Binärer Netzwerk-Header (C++)

Für den ressourcenschonenden Transport über klassische serielle Busse oder DMX-Kabel verwendet Nexus-ODEP eine hocheffiziente, bit-gepackte Binärstruktur (exakt 12 Bytes Header) anstelle von sperrigem JSON:

```cpp

\\\\\\\\#pragma pack(push, 1)



struct NODEP\\\\\\\\\\\\\\\_BinaryHeader {

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t start\\\\\\\\\\\\\\\_code;       // Immer 0x4F (Identifikator)

\\\\\\\&#x20;   

\\\\\\\&#x20;   // Bit-Packing für administrative Flags (Zusammen exakt 1 Byte)

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t cmd\\\\\\\\\\\\\\\_id       : 4; // 16 administrative Nexus-ODEP-Befehle

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t resource\\\\\\\\\\\\\\\_mode: 2; // 0=FREE, 1=EXCLUSIVE, 2=SHARED, 3=RESERVED

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t reserved     : 2; 

\\\\\\\&#x20;   

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t priority;         // Priorität (0 - 255) zur Konfliktlösung

\\\\\\\&#x20;   uint16\\\\\\\\\\\\\\\_t controller\\\\\\\\\\\\\\\_id;   // Numerische ID des Senders

\\\\\\\&#x20;   uint16\\\\\\\\\\\\\\\_t target\\\\\\\\\\\\\\\_id;       // Numerische ID des Ziel-Knotens/VD

\\\\\\\&#x20;   uint8\\\\\\\\\\\\\\\_t  resource\\\\\\\\\\\\\\\_id;     // Lokaler Funktions-Index auf dem Gerät

\\\\\\\&#x20;   

\\\\\\\&#x20;   uint32\\\\\\\\\\\\\\\_t duration\\\\\\\\\\\\\\\_ms;     // Ablaufzeit der Reservierung

};



\\\\\\\\#pragma pack(pop)

```

\---

## ⚙️ Kompilierung (Modul-Auswahl via Make)

Das Framework ist so aufgebaut, dass du über einfache Parameter beim Aufruf von GNU Make bestimmen kannst, welche Module mitkompiliert werden. So erzeugst du maßgeschneiderte Binaries für jede Hardware-Größe:

**Vollständiger Build (mit Scheduler \& Virtualisierung):**

```bash

make

```

**Minimaler Build (nur Schicht 1, perfekt für kleine Mikrocontroller):**

```bash

make ENABLE\\\\\\\\\\\\\\\_SCHEDULER=0 ENABLE\\\\\\\\\\\\\\\_VIRTUALIZATION=0

```

\---

## 🤝 Mitmachen / Contribution

Wir glauben an ein freies, demokratisches und dezentrales Internet der Dinge. Wenn du Erweiterungen für das Rahmenprotokoll vorschlagen oder branchenspezifische Applikations-Profile beisteuern möchtest, eröffne einfach einen Pull Request!

