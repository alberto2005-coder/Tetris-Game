#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdint>

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

int loadHighScore() {
    int highscore = 0;
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&highscore), sizeof(highscore));
        file.close();
    }
    return highscore;
}

void saveHighScore(int highscore) {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&highscore), sizeof(highscore));
        file.close();
    }
}

// Función para dibujar un bloque con efecto de relieve (bevel)
void drawBlock(sf::RenderWindow& window, sf::RectangleShape& shape, int x, int y, sf::Color color, float alpha = 255) {
    shape.setPosition({(float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE});
    
    // Color principal
    sf::Color mainColor = color;
    mainColor.a = static_cast<std::uint8_t>(alpha);
    shape.setFillColor(mainColor);
    shape.setOutlineThickness(0);
    window.draw(shape);

    if (alpha > 200) { // Solo relieve si no es transparente (no ghost)
        // Brillo superior e izquierdo
        sf::RectangleShape highlight(sf::Vector2f{BLOCK_SIZE - 2.0f, 2.0f});
        highlight.setFillColor(sf::Color(255, 255, 255, 100));
        highlight.setPosition({(float)x * BLOCK_SIZE + 1, (float)y * BLOCK_SIZE + 1});
        window.draw(highlight);
        
        highlight.setSize({2.0f, BLOCK_SIZE - 2.0f});
        window.draw(highlight);

        // Sombra inferior y derecha
        sf::RectangleShape shadow(sf::Vector2f{BLOCK_SIZE - 2.0f, 2.0f});
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        shadow.setPosition({(float)x * BLOCK_SIZE + 1, (float)y * BLOCK_SIZE + BLOCK_SIZE - 3});
        window.draw(shadow);

        shadow.setSize({2.0f, BLOCK_SIZE - 2.0f});
        shadow.setPosition({(float)x * BLOCK_SIZE + BLOCK_SIZE - 3, (float)y * BLOCK_SIZE + 1});
        window.draw(shadow);
    }
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode({(unsigned int)(FIELD_W * BLOCK_SIZE + 200), (unsigned int)(FIELD_H * BLOCK_SIZE)}), "Tetris Pro");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = false;
    if (font.openFromFile("arial.ttf")) {
        fontLoaded = true;
    }

    sf::RectangleShape blockShape(sf::Vector2f{BLOCK_SIZE - 2.0f, BLOCK_SIZE - 2.0f});

    std::vector<int> field(FIELD_W * FIELD_H, 0);

    int currentPiece = std::rand() % 7;
    int nextPiece = std::rand() % 7;
    int currentRotation = 0;
    int currentX = FIELD_W / 2 - 2;
    int currentY = 0;

    float speed = 0.5f;
    sf::Clock clock;
    float delay = 0.0f;

    bool forceDown = false;
    bool gameOver = false;

    int score = 0;
    int highScore = loadHighScore();
    int linesClearedTotal = 0;
    int level = 1;

    std::array<sf::Color, 8> colors = {sf::Color::Black,
        sf::Color::Cyan, sf::Color::Blue, sf::Color(255,165,0),
        sf::Color::Yellow, sf::Color::Green, sf::Color::Magenta, sf::Color::Red};

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (!gameOver) {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Left) {
                        if (doesFit(currentPiece, currentRotation, currentX - 1, currentY, field)) currentX--;
                    } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                        if (doesFit(currentPiece, currentRotation, currentX + 1, currentY, field)) currentX++;
                    } else if (keyPressed->code == sf::Keyboard::Key::Up) {
                        if (doesFit(currentPiece, currentRotation + 1, currentX, currentY, field)) currentRotation = (currentRotation + 1) % 4;
                    } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                        if (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) currentY++;
                    } else if (keyPressed->code == sf::Keyboard::Key::Space) {
                        while (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) currentY++;
                        forceDown = true;
                    }
                }
            } else {
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::R) {
                        field.assign(FIELD_W * FIELD_H, 0);
                        currentPiece = std::rand() % 7;
                        nextPiece = std::rand() % 7;
                        currentRotation = 0;
                        currentX = FIELD_W / 2 - 2;
                        currentY = 0;
                        score = 0;
                        linesClearedTotal = 0;
                        level = 1;
                        speed = 0.5f;
                        highScore = loadHighScore();
                        gameOver = false;
                        clock.restart();
                    }
                }
            }
        }

        if (!gameOver) {
            delay = clock.getElapsedTime().asSeconds();
            if (delay >= speed || forceDown) {
                forceDown = false;
                clock.restart();
                if (doesFit(currentPiece, currentRotation, currentX, currentY + 1, field)) {
                    currentY++;
                } else {
                    for (int px = 0; px < 4; px++)
                        for (int py = 0; py < 4; py++) {
                            int pi = rotateIndex(px, py, currentRotation);
                            if (tetromino[currentPiece][pi] == 'X') {
                                int fx = currentX + px;
                                int fy = currentY + py;
                                if (fy >= 0 && fy < FIELD_H && fx >= 0 && fx < FIELD_W)
                                    field[fy * FIELD_W + fx] = currentPiece + 1;
                            }
                        }

                    int linesRemoved = 0;
                    for (int py = 0; py < 4; py++) {
                        if (currentY + py < FIELD_H) {
                            bool line = true;
                            int y = currentY + py;
                            for (int x = 0; x < FIELD_W; x++)
                                if (field[y * FIELD_W + x] == 0) { line = false; break; }
                            if (line) {
                                linesRemoved++;
                                for (int x = 0; x < FIELD_W; x++) field[y * FIELD_W + x] = 0;
                                for (int d = y; d > 0; d--)
                                    for (int x = 0; x < FIELD_W; x++) field[d * FIELD_W + x] = field[(d - 1) * FIELD_W + x];
                                for (int x = 0; x < FIELD_W; x++) field[x] = 0;
                            }
                        }
                    }

                    if (linesRemoved > 0) {
                        score += 100 * linesRemoved * level;
                        linesClearedTotal += linesRemoved;
                        // Subir de nivel cada 10 líneas
                        level = (linesClearedTotal / 10) + 1;
                        speed = 0.5f * std::pow(0.85f, (float)level - 1); // Aumentar velocidad
                    }

                    // Guardado inmediato si supera récord
                    if (score > highScore) {
                        highScore = score;
                        saveHighScore(highScore);
                    }

                    currentPiece = nextPiece;
                    nextPiece = std::rand() % 7;
                    currentRotation = 0;
                    currentX = FIELD_W / 2 - 2;
                    currentY = 0;

                    if (!doesFit(currentPiece, currentRotation, currentX, currentY, field)) {
                        gameOver = true;
                    }
                }
            }
        }

        window.clear(sf::Color(20, 20, 20));

        // Dibujar campo
        for (int y = 0; y < FIELD_H; y++)
            for (int x = 0; x < FIELD_W; x++) {
                int val = field[y * FIELD_W + x];
                if (val == 0) {
                    // Cuadrícula sutil
                    sf::RectangleShape cell({BLOCK_SIZE-1.0f, BLOCK_SIZE-1.0f});
                    cell.setPosition({(float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE});
                    cell.setFillColor(sf::Color(35, 35, 35));
                    window.draw(cell);
                } else {
                    drawBlock(window, blockShape, x, y, colors[val]);
                }
            }

        if (!gameOver) {
            // Calcular posición fantasma
            int ghostY = currentY;
            while (doesFit(currentPiece, currentRotation, currentX, ghostY + 1, field)) ghostY++;

            // Dibujar fantasma
            for (int px = 0; px < 4; px++)
                for (int py = 0; py < 4; py++) {
                    int pi = rotateIndex(px, py, currentRotation);
                    if (tetromino[currentPiece][pi] == 'X') {
                        drawBlock(window, blockShape, currentX + px, ghostY + py, colors[currentPiece + 1], 60); // Alpha 60
                    }
                }

            // Dibujar pieza actual
            for (int px = 0; px < 4; px++)
                for (int py = 0; py < 4; py++) {
                    int pi = rotateIndex(px, py, currentRotation);
                    if (tetromino[currentPiece][pi] == 'X') {
                        drawBlock(window, blockShape, currentX + px, currentY + py, colors[currentPiece + 1]);
                    }
                }
        }

        if (fontLoaded) {
            float uiX = (float)FIELD_W * BLOCK_SIZE + 20.0f;
            
            sf::Text txt(font);
            txt.setCharacterSize(20);
            txt.setFillColor(sf::Color::Yellow);
            txt.setPosition({uiX, 20.0f});
            txt.setString("RECORD\n" + std::to_string(highScore));
            window.draw(txt);

            txt.setFillColor(sf::Color::White);
            txt.setPosition({uiX, 80.0f});
            txt.setString("PUNTOS\n" + std::to_string(score));
            window.draw(txt);

            txt.setPosition({uiX, 140.0f});
            txt.setString("NIVEL: " + std::to_string(level));
            window.draw(txt);

            txt.setCharacterSize(18);
            txt.setPosition({uiX, 190.0f});
            txt.setString("SIGUIENTE:");
            window.draw(txt);

            // Preview siguiente pieza con relieve
            for (int px = 0; px < 4; px++)
                for (int py = 0; py < 4; py++) {
                    int pi = rotateIndex(px, py, 0);
                    if (tetromino[nextPiece][pi] == 'X') {
                        blockShape.setSize({BLOCK_SIZE-5.0f, BLOCK_SIZE-5.0f});
                        // Ajuste manual de posición para centrar preview
                        float px_adj = uiX + 10.0f + (float)px * (BLOCK_SIZE - 4.0f);
                        float py_adj = 230.0f + (float)py * (BLOCK_SIZE - 4.0f);
                        
                        // Versión simplificada de drawBlock para el preview
                        sf::RectangleShape previewRect(sf::Vector2f{BLOCK_SIZE-5.0f, BLOCK_SIZE-5.0f});
                        previewRect.setPosition({px_adj, py_adj});
                        previewRect.setFillColor(colors[nextPiece + 1]);
                        window.draw(previewRect);
                    }
                }
            blockShape.setSize({BLOCK_SIZE-2.0f, BLOCK_SIZE-2.0f}); // reset

            sf::Text controlTxt(font);
            controlTxt.setCharacterSize(14);
            controlTxt.setFillColor(sf::Color(180, 180, 180));
            controlTxt.setPosition({uiX, 400.0f});
            controlTxt.setString("CONTROLES:\nFlechas: Mover\nArriba: Rotar\nEspacio: Caida rapida\nR: Reiniciar");
            window.draw(controlTxt);

            if (gameOver) {
                sf::RectangleShape overlay(sf::Vector2f{(float)FIELD_W * BLOCK_SIZE, (float)FIELD_H * BLOCK_SIZE});
                overlay.setFillColor(sf::Color(0, 0, 0, 150));
                window.draw(overlay);

                sf::Text go(font);
                go.setCharacterSize(36);
                go.setFillColor(sf::Color::Red);
                go.setStyle(sf::Text::Bold);
                go.setString("GAME OVER\nPulsa R");
                go.setPosition({30.0f, (float)FIELD_H * BLOCK_SIZE / 2.0f - 50.0f});
                window.draw(go);
            }
        }

        window.display();
    }

    return 0;
}
