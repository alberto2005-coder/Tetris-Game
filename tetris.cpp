#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

// Tetris simple en C++ usando SFML
// Compilar:
// g++ Tetris.cpp -o tetris -lsfml-graphics -lsfml-window -lsfml-system

const int FIELD_W = 10;
const int FIELD_H = 20;
const int BLOCK_SIZE = 30; // tamaño de cada bloque en pixeles

std::array<std::string, 7> tetromino = {
    "..X...X...X...X.", // I
    ".X..X..XX.......", // J
    "..X..X..XX......", // L
    ".XX..XX.........", // O
    ".XX.XX..........", // S 
    ".X..XX..X.......", // T
    "XX...XX........."  // Z
};

int rotateIndex(int px, int py, int r) {
    
    switch (r % 4) {
        case 0: return py * 4 + px;            // 0 degrees
        case 1: return 12 + py - (px * 4);     // 90 degrees
        case 2: return 15 - (py * 4) - px;     // 180 degrees
        case 3: return 3 - py + (px * 4);      // 270 degrees
    }
    return 0;
}

bool doesFit(int tetrominoIndex, int rotation, int posX, int posY, const std::vector<int>& field) {
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++) {
            int pi = rotateIndex(px, py, rotation);
            char piece = tetromino[tetrominoIndex][pi];
            if (piece == 'X') {
                int fx = posX + px;
                int fy = posY + py;
                if (fx < 0 || fx >= FIELD_W || fy < 0 || fy >= FIELD_H) return false;
                if (field[fy * FIELD_W + fx] != 0) return false;
            }
        }
    return true;
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode(FIELD_W * BLOCK_SIZE + 200, FIELD_H * BLOCK_SIZE), "Tetris");
    window.setFramerateLimit(60);

    sf::RectangleShape blockShape(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));

    std::vector<int> field(FIELD_W * FIELD_H, 0);

    int currentPiece = std::rand() % 7;
    int currentRotation = 0;
    int currentX = FIELD_W / 2 - 2;
    int currentY = 0;

    float speed = 0.5f; // segundos por caída
    sf::Clock clock;
    float delay = 0.0f;

    bool forceDown = false;
    bool gameOver = false;

    int score = 0;

    // Colores por pieza (7 piezas)
    std::array<sf::Color, 8> colors = {sf::Color::Black,
        sf::Color::Cyan, sf::Color::Blue, sf::Color(255,165,0), // orange
        sf::Color::Yellow, sf::Color::Green, sf::Color::Magenta, sf::Color::Red};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && !gameOver) {
                if (event.key.code == sf::Keyboard::Left) {
                    if (doesFit(currentPiece, currentRotation, currentX - 1, currentY, field)) currentX--;
                } else if (event.key.code == sf::Keyboard::Right) {
                    if (doesFit(currentPiece, currentRotation, currentX + 1, currentY, field)) currentX++;
                } else if (event.key.code == sf::Keyboard::Up) {
                    if (doesFit(currentPiece, currentRotation + 1, currentX, currentY, field)) currentRotation = (currentRotation + 1) % 4;
                } else if (event.key.code == sf::Keyboard::Down) {
                    
                    if (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) currentY++;
                } else if (event.key.code == sf::Keyboard::Space) {
                    
                    while (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) currentY++;
                    forceDown = true;
                }
            }
            if (event.type == sf::Event::KeyPressed && gameOver) {
                if (event.key.code == sf::Keyboard::R) {
                    // reiniciar
                    field.assign(FIELD_W * FIELD_H, 0);
                    currentPiece = std::rand() % 7;
                    currentRotation = 0;
                    currentX = FIELD_W / 2 - 2;
                    currentY = 0;
                    score = 0;
                    gameOver = false;
                    clock.restart();
                }
            }
        }

        // temporizador de caída
        delay = clock.getElapsedTime().asSeconds();
        if (delay >= speed || forceDown) {
            forceDown = false;
            clock.restart();
            if (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) {
                currentY++;
            } else {
                // fijar pieza al campo
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++) {
                        int pi = rotateIndex(px, py, currentRotation);
                        if (tetromino[currentPiece][pi] == 'X') {
                            int fx = currentX + px;
                            int fy = currentY + py;
                            if (fy >= 0 && fy < FIELD_H && fx >= 0 && fx < FIELD_W)
                                field[fy * FIELD_W + fx] = currentPiece + 1; // color index
                        }
                    }

                // comprobar líneas completas
                std::vector<int> lines;
                for (int py = 0; py < 4; py++) {
                    if (currentY + py < FIELD_H) {
                        bool line = true;
                        int y = currentY + py;
                        for (int x = 0; x < FIELD_W; x++)
                            if (field[y * FIELD_W + x] == 0) { line = false; break; }
                        if (line) lines.push_back(y);
                    }
                }

                if (!lines.empty()) {
                    // eliminar líneas y desplazar hacia abajo
                    for (int l : lines) {
                        for (int x = 0; x < FIELD_W; x++) field[l * FIELD_W + x] = 0;
                        for (int y = l; y > 0; y--)
                            for (int x = 0; x < FIELD_W; x++) field[y * FIELD_W + x] = field[(y - 1) * FIELD_W + x];
                        for (int x = 0; x < FIELD_W; x++) field[x] = 0;
                    }
                    // Puntaje: 100 por línea, +50 por cada línea extra encadenada
                    score += 100 * (int)lines.size();
                }

                // nueva pieza
                currentPiece = std::rand() % 7;
                currentRotation = 0;
                currentX = FIELD_W / 2 - 2;
                currentY = 0;

                if (!doesFit(currentPiece, currentRotation, currentX, currentY, field)) {
                    gameOver = true;
                }
            }
        }

        // Render
        window.clear(sf::Color(30, 30, 30));

        // Dibujar campo
        for (int y = 0; y < FIELD_H; y++)
            for (int x = 0; x < FIELD_W; x++) {
                int val = field[y * FIELD_W + x];
                blockShape.setPosition(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1);
                if (val == 0) {
                    blockShape.setFillColor(sf::Color(50,50,50));
                    blockShape.setOutlineColor(sf::Color(25,25,25));
                    blockShape.setOutlineThickness(1);
                } else {
                    blockShape.setFillColor(colors[val]);
                    blockShape.setOutlineColor(sf::Color::Black);
                    blockShape.setOutlineThickness(1);
                }
                window.draw(blockShape);
            }

        // Dibujar pieza actual
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++) {
                int pi = rotateIndex(px, py, currentRotation);
                if (tetromino[currentPiece][pi] == 'X') {
                    int fx = currentX + px;
                    int fy = currentY + py;
                    blockShape.setPosition(fx * BLOCK_SIZE + 1, fy * BLOCK_SIZE + 1);
                    blockShape.setFillColor(colors[currentPiece + 1]);
                    blockShape.setOutlineColor(sf::Color::Black);
                    window.draw(blockShape);
                }
            }

        // UI lateral: puntuación y controles
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            // fallback: usar fuente por defecto (si no carga, no muestra texto)
        }
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(18);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(FIELD_W * BLOCK_SIZE + 10, 10);
        txt.setString("Puntos: " + std::to_string(score));
        window.draw(txt);

        sf::Text controls;
        controls.setFont(font);
        controls.setCharacterSize(14);
        controls.setFillColor(sf::Color::White);
        controls.setPosition(FIELD_W * BLOCK_SIZE + 10, 40);
        controls.setString("Controles:\nFlecha Izq/Dcha: Mover\nFlecha Arriba: Rotar\nFlecha Abajo: Soft Drop\nSpace: Hard Drop\nR: Reiniciar (game over)");
        window.draw(controls);

        if (gameOver) {
            sf::Text go;
            go.setFont(font);
            go.setCharacterSize(36);
            go.setFillColor(sf::Color::Red);
            go.setStyle(sf::Text::Bold);
            go.setString("GAME OVER\nR para reiniciar");
            go.setPosition(20, FIELD_H * BLOCK_SIZE / 2 - 50);
            window.draw(go);
        }

        window.display();
    }

    return 0;
}
