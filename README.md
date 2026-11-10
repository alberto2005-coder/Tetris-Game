# Tetris en C++ con SFML

[Español](#español) · [English](#english)

---

## Español

### Índice

- [Descripción](#descripción)
- [Características](#características)
- [Controles](#controles)
- [Requisitos](#requisitos)
- [Instalación de SFML](#instalación-de-sfml)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Sistema de puntuación](#sistema-de-puntuación)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Problemas frecuentes](#problemas-frecuentes)
- [Licencia](#licencia)

### Descripción

Implementación completa del clásico **Tetris** en **C++17** con la biblioteca gráfica **SFML 2.5+**. El tablero es de 10 × 20 bloques de 30 píxeles y el juego corre a 60 FPS.

El repositorio incluye el código fuente (`tetris.cpp`) y un ejecutable ya compilado para Windows (`tetris.exe`).

### Características

- **Los 7 tetrominós clásicos** (I, J, L, O, S, T, Z) con rotación en cuatro posiciones.
- **Pieza fantasma**: previsualización semitransparente de dónde caerá la pieza.
- **Siguiente pieza**: panel lateral con la previsualización del siguiente tetrominó.
- **Hard drop y soft drop**.
- **Sistema de niveles**: se sube un nivel cada 10 líneas y la caída se acelera un 15 % por nivel.
- **Puntuación**: 100 × líneas eliminadas × nivel.
- **Récord persistente** en `save.dat`.
- **Game Over con reinicio** sin cerrar el juego.
- **Efecto de relieve** en los bloques para dar volumen.

### Controles

| Tecla | Acción |
|---|---|
| `←` / `→` | Mover la pieza |
| `↑` | Rotar la pieza |
| `↓` | Bajar suave (soft drop) |
| `Espacio` | Caída instantánea (hard drop) |
| `R` | Reiniciar la partida (en Game Over) |

### Requisitos

- **Compilador con C++17** (g++ o MSYS2/MinGW en Windows).
- **SFML 2.5 o superior**.

### Instalación de SFML

En Windows con **MSYS2 UCRT64**:

```bash
pacman -S mingw-w64-ucrt-x86_64-sfml
```

En Debian/Ubuntu:

```bash
sudo apt install libsfml-dev
```

En macOS con Homebrew:

```bash
brew install sfml
```

### Compilación

Desde la terminal **MSYS2 UCRT64** (Windows):

```bash
g++ tetris.cpp -o tetris.exe -std=c++17 \
   -I"C:/msys64/ucrt64/include" \
   -L"C:/msys64/ucrt64/lib" \
   -lsfml-graphics -lsfml-window -lsfml-system
```

En Linux/macOS:

```bash
g++ tetris.cpp -o tetris -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
```

### Ejecución

```bash
./tetris.exe
```

o, en Linux/macOS:

```bash
./tetris
```

### Sistema de puntuación

- Cada línea eliminada vale `100 × nivel` puntos.
- Al acumular 10 líneas se sube de nivel.
- La velocidad de caída pasa a `0.5 × 0.85^(nivel − 1)` segundos por celda.
- El récord se guarda en `save.dat` en cuanto se supera.

### Estructura del proyecto

```text
Tetris-Game/
├── tetris.cpp      # Código fuente completo
├── tetris.exe      # Ejecutable ya compilado para Windows
├── arial.ttf       # Fuente usada por la interfaz
├── icon.ico        # Icono del ejecutable
├── icon.rc         # Recurso de icono para Windows
├── icon.res        # Recurso compilado
├── package.json
├── LICENSE
└── README.md
```

### Problemas frecuentes

- **Falta `sfml-graphics-2.dll` u otra DLL**: copia las DLL desde `C:/msys64/ucrt64/bin/` a la carpeta del ejecutable.
- **No se ve el texto**: el juego carga `arial.ttf` desde el directorio de trabajo. Ejecútalo desde la carpeta del proyecto o copia la fuente junto al ejecutable.
- **La ventana se abre y se cierra**: revisa que la consola muestre el error de carga de SFML; suele ser un problema de DLL o de arquitectura (compilador de 64 bits con SFML de 32 bits, o al revés).

### Licencia

Este proyecto se distribuye bajo la **licencia MIT**. Consulta [LICENSE](LICENSE).

---

## English

### Table of contents

- [Overview](#overview)
- [Features](#features)
- [Controls](#controls)
- [Requirements](#requirements)
- [Installing SFML](#installing-sfml)
- [Building](#building)
- [Running](#running)
- [Scoring system](#scoring-system)
- [Project structure](#project-structure)
- [Troubleshooting](#troubleshooting)
- [License](#license)

### Overview

A complete **Tetris** implementation in **C++17** using the **SFML 2.5+** graphics library. The board is 10 × 20 blocks of 30 pixels and the game runs at 60 FPS.

The repository ships the source (`tetris.cpp`) and a prebuilt Windows executable (`tetris.exe`).

### Features

- **All 7 classic tetrominoes** (I, J, L, O, S, T, Z) with four rotation states.
- **Ghost piece**: semi-transparent preview of where the piece will land.
- **Next piece** preview in the side panel.
- **Hard drop and soft drop**.
- **Level system**: one level every 10 cleared lines, with a 15 % faster fall per level.
- **Scoring**: 100 × cleared lines × level.
- **Persistent high score** in `save.dat`.
- **Game Over with restart** without closing the game.
- **Bevel effect** on blocks for a 3D look.

### Controls

| Key | Action |
|---|---|
| `←` / `→` | Move the piece |
| `↑` | Rotate the piece |
| `↓` | Soft drop |
| `Space` | Hard drop |
| `R` | Restart (on Game Over) |

### Requirements

- A **C++17 compiler** (g++ or MSYS2/MinGW on Windows).
- **SFML 2.5 or later**.

### Installing SFML

On Windows with **MSYS2 UCRT64**:

```bash
pacman -S mingw-w64-ucrt-x86_64-sfml
```

On Debian/Ubuntu:

```bash
sudo apt install libsfml-dev
```

On macOS with Homebrew:

```bash
brew install sfml
```

### Building

From the **MSYS2 UCRT64** terminal (Windows):

```bash
g++ tetris.cpp -o tetris.exe -std=c++17 \
   -I"C:/msys64/ucrt64/include" \
   -L"C:/msys64/ucrt64/lib" \
   -lsfml-graphics -lsfml-window -lsfml-system
```

On Linux/macOS:

```bash
g++ tetris.cpp -o tetris -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
```

### Running

```bash
./tetris.exe
```

or, on Linux/macOS:

```bash
./tetris
```

### Scoring system

- Each cleared line is worth `100 × level` points.
- Every 10 cleared lines increases the level.
- Fall speed becomes `0.5 × 0.85^(level − 1)` seconds per cell.
- The high score is written to `save.dat` as soon as it is beaten.

### Project structure

```text
Tetris-Game/
├── tetris.cpp      # Full source code
├── tetris.exe      # Prebuilt Windows executable
├── arial.ttf       # Font used by the UI
├── icon.ico        # Executable icon
├── icon.rc         # Windows icon resource
├── icon.res        # Compiled resource
├── package.json
├── LICENSE
└── README.md
```

### Troubleshooting

- **Missing `sfml-graphics-2.dll` or other DLLs**: copy the DLLs from `C:/msys64/ucrt64/bin/` next to the executable.
- **No text visible**: the game loads `arial.ttf` from the working directory. Run it from the project folder or copy the font next to the executable.
- **Window opens and closes immediately**: check the console output; it is usually a DLL or architecture mismatch (32-bit compiler with 64-bit SFML or vice versa).

### License

This project is released under the **MIT License**. See [LICENSE](LICENSE).
