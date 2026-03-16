#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <fstream>

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

int main() {
    std::srand((unsigned)std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode({(unsigned int)(FIELD_W * BLOCK_SIZE + 200), (unsigned int)(FIELD_H * BLOCK_SIZE)}), "Tetris");
    window.setFramerateLimit(60);

    // Cargar fuente UNA SOLA VEZ al inicio
    sf::Font font;
    bool fontLoaded = false;
    if (font.openFromFile("arial.ttf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Error: No se pudo cargar arial.ttf. Asegúrate de que el archivo esté en la misma carpeta que el ejecutable." << std::endl;
    }

    sf::RectangleShape blockShape(sf::Vector2f{BLOCK_SIZE - 2.0f, BLOCK_SIZE - 2.0f});

    std::vector<int> field(FIELD_W * FIELD_H, 0);

    int currentPiece = std::rand() % 7;
    int nextPiece = std::rand() % 7;
    int currentRotation = 0;
    int currentX = FIELD_W / 2 - 2;
    int currentY = 0;

    float speed = 0.5f; // segundos por caída
    sf::Clock clock;
    float delay = 0.0f;

    bool forceDown = false;
    bool gameOver = false;

    int score = 0;
    int highScore = loadHighScore();

    // Colores por pieza (7 piezas)
    std::array<sf::Color, 8> colors = {sf::Color::Black,
        sf::Color::Cyan, sf::Color::Blue, sf::Color(255,165,0), // orange
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
                        // reiniciar
                        field.assign(FIELD_W * FIELD_H, 0);
                        currentPiece = std::rand() % 7;
                        nextPiece = std::rand() % 7;
                        currentRotation = 0;
                        currentX = FIELD_W / 2 - 2;
                        currentY = 0;
                        score = 0;
                        highScore = loadHighScore();
                        gameOver = false;
                        clock.restart();
                    }
                }
            }
        }

        if (!gameOver) {
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
                        // Puntaje: 100 por línea
                        score += 100 * (int)lines.size();
                    }

                    // nueva pieza (usar la siguiente)
                    currentPiece = nextPiece;
                    nextPiece = std::rand() % 7;
                    currentRotation = 0;
                    currentX = FIELD_W / 2 - 2;
                    currentY = 0;

                    if (!doesFit(currentPiece, currentRotation, currentX, currentY, field)) {
                        gameOver = true;
                        if (score > highScore) {
                            highScore = score;
                            saveHighScore(highScore);
                        }
                    }
                }
            }
        }

        // Render
        window.clear(sf::Color(30, 30, 30));

        // Dibujar campo
        for (int y = 0; y < FIELD_H; y++)
            for (int x = 0; x < FIELD_W; x++) {
                int val = field[y * FIELD_W + x];
                blockShape.setPosition({(float)x * BLOCK_SIZE + 1.0f, (float)y * BLOCK_SIZE + 1.0f});
                if (val == 0) {
                    blockShape.setFillColor(sf::Color(50,50,50));
                    blockShape.setOutlineColor(sf::Color(25,25,25));
                    blockShape.setOutlineThickness(1.0f);
                } else {
                    blockShape.setFillColor(colors[val]);
                    blockShape.setOutlineColor(sf::Color::Black);
                    blockShape.setOutlineThickness(1.0f);
                }
                window.draw(blockShape);
            }

        // Dibujar pieza actual
        if (!gameOver) {
            for (int px = 0; px < 4; px++)
                for (int py = 0; py < 4; py++) {
                    int pi = rotateIndex(px, py, currentRotation);
                    if (tetromino[currentPiece][pi] == 'X') {
                        int fx = currentX + px;
                        int fy = currentY + py;
                        blockShape.setPosition({(float)fx * BLOCK_SIZE + 1.0f, (float)fy * BLOCK_SIZE + 1.0f});
                        blockShape.setFillColor(colors[currentPiece + 1]);
                        blockShape.setOutlineColor(sf::Color::Black);
                        blockShape.setOutlineThickness(1.0f);
                        window.draw(blockShape);
                    }
                }
        }

        // UI lateral
        if (fontLoaded) {
            sf::Text txt(font);
            txt.setCharacterSize(18);
            txt.setFillColor(sf::Color::White);
            txt.setPosition({(float)FIELD_W * BLOCK_SIZE + 10.0f, 10.0f});
            txt.setString("Puntos: " + std::to_string(score) + "\nRecord: " + std::to_string(highScore));
            window.draw(txt);

            // Siguiente pieza
            sf::Text nextTxt(font);
            nextTxt.setCharacterSize(18);
            nextTxt.setFillColor(sf::Color::White);
            nextTxt.setPosition({(float)FIELD_W * BLOCK_SIZE + 10.0f, 70.0f});
            nextTxt.setString("Siguiente:");
            window.draw(nextTxt);

            for (int px = 0; px < 4; px++)
                for (int py = 0; py < 4; py++) {
                    int pi = rotateIndex(px, py, 0); // rotación 0 para preview
                    if (tetromino[nextPiece][pi] == 'X') {
                        blockShape.setPosition({(float)FIELD_W * BLOCK_SIZE + 30.0f + (float)px * (BLOCK_SIZE - 5), 100.0f + (float)py * (BLOCK_SIZE - 5)});
                        blockShape.setScale({0.8f, 0.8f}); // un poco más pequeña
                        blockShape.setFillColor(colors[nextPiece + 1]);
                        blockShape.setOutlineColor(sf::Color::Black);
                        blockShape.setOutlineThickness(1.0f);
                        window.draw(blockShape);
                    }
                }
            blockShape.setScale({1.0f, 1.0f}); // reset scale

            sf::Text controlTxt(font);
            controlTxt.setCharacterSize(14);
            controlTxt.setFillColor(sf::Color::White);
            controlTxt.setPosition({(float)FIELD_W * BLOCK_SIZE + 10.0f, 220.0f});
            controlTxt.setString("Controles:\nFlechas: Mover/Rotar\nEspacio: Hard Drop\nR: Reiniciar");
            window.draw(controlTxt);

            if (gameOver) {
                sf::Text go(font);
                go.setCharacterSize(36);
                go.setFillColor(sf::Color::Red);
                go.setStyle(sf::Text::Bold);
                go.setString("GAME OVER\nR para reiniciar");
                go.setPosition({20.0f, (float)FIELD_H * BLOCK_SIZE / 2.0f - 50.0f});
                window.draw(go);
            }
        }

        window.display();
    }

    return 0;
}
