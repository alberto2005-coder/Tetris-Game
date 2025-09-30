# Tetris en C++ con SFML

¡Bienvenido a **Tetris en C++**! 🟦🟩🟥🟨 Este proyecto implementa el clásico juego de Tetris usando la librería gráfica **SFML**.

---

## 🎮 Características
- Piezas clásicas de Tetris (7 tetrominós)
- Rotación, colisiones y fijación de piezas
- Eliminación de líneas y puntuación
- Controles de movimiento, soft drop y hard drop
- Pantalla de *Game Over* con opción de reiniciar

---

## ⚙️ Requisitos
- **MSYS2 + MinGW UCRT64** (Windows) o compilador g++ en Linux/Mac
- **SFML 2.5+** instalada

### Instalación de SFML en MSYS2 (Windows)
1. Abrir la terminal **MSYS2 UCRT64**
2. Instalar SFML:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-sfml
   ```

---

## 🚀 Compilación
Desde la terminal **UCRT64**:
```bash
cd /c/Users/<TU_USUARIO>/Downloads/tetris

# Compilar con SFML
g++ tetris.cpp -o tetris.exe \
   -I"C:/msys64/ucrt64/include" \
   -L"C:/msys64/ucrt64/lib" \
   -lsfml-graphics -lsfml-window -lsfml-system
```

---

## ▶️ Ejecución
```bash
./tetris.exe
```
Si aparecen errores de **DLL faltante**, copia los `.dll` desde:
```
C:/msys64/ucrt64/bin/
```
a la carpeta del ejecutable.

---

## ⌨️ Controles
| Tecla      | Acción            |
|------------|-------------------|
| ← / →      | Mover pieza       |
| ↑          | Rotar pieza       |
| ↓          | Soft drop         |
| Espacio    | Hard drop         |
| R          | Reiniciar juego   |

---

## 📝 Fuente (TTF)
El juego requiere una fuente TrueType (`.ttf`) para mostrar texto:
- **Linux:** `/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf`
- **Windows:** coloca `Arial.ttf` o `DejaVuSans.ttf` en la carpeta del ejecutable y cambia la ruta en el código:
```cpp
font.loadFromFile("arial.ttf");
```

---

## 📜 Licencia
Este proyecto está bajo la **Licencia MIT**. Consulta el archivo [LICENSE](LICENSE) para más detalles.

---

¡Disfruta programando y jugando tu propio Tetris! 🎮