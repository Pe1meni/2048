#include <SFML/Graphics.hpp>
#include "PlayingField.h"

#include <array>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

// ================== КОНСТАНТЫ ==================
const int RES_X = 1080;
const int RES_Y = 720;
const int GRID_SIZE = 4;
const float ANIM_DURATION = 0.18f;
const float SPAWN_DURATION = 0.12f;

// ================== ЦВЕТА ПЛИТОК ==================
sf::Color getTileColor(int value) {
    switch (value) {
    case 2:    return { 238, 228, 218 };
    case 4:    return { 237, 224, 200 };
    case 8:    return { 242, 177, 121 };
    case 16:   return { 245, 149, 99 };
    case 32:   return { 246, 124, 95 };
    case 64:   return { 246, 94, 59 };
    case 128:  return { 237, 207, 114 };
    case 256:  return { 237, 204, 97 };
    case 512:  return { 237, 200, 80 };
    case 1024: return { 237, 197, 63 };
    case 2048: return { 237, 194, 46 };
    default:   return { 60, 58, 50 };
    }
}

sf::Color getTextColor(int value) {
    return (value == 2 || value == 4) ? sf::Color::Black : sf::Color::White;
}

int getFontSize(int value) {
    if (value >= 1024) return 26;
    if (value >= 128)  return 34;
    if (value >= 16)   return 42;
    return 48;
}

// ================== КЛАСС ==================
class SFML2048 {
private:
    sf::RenderWindow window;
    PlayingField pf;
    sf::Font font;
    enum class State { Menu, Playing, Won, Lost };
    State currentState = State::Menu;

    int field[4][4];
    int oldField[4][4];
    int moveField[4][4];

    bool isAnimating = false;
    bool isSpawning = false;
    float animProgress = 0.f;
    float spawnProgress = 0.f;
    char lastDir = ' ';
    int newTileRow = -1;
    int newTileCol = -1;

    bool won = false;
    bool lost = false;

    sf::Clock clock;

    float getTileSize() const {
        float minSide = std::min(RES_X, RES_Y) * 0.78f;
        return minSide / 4.f;
    }

    float getPadding() const { return getTileSize() * 0.12f; }

    sf::Vector2f getGridOffset() const {
        float ts = getTileSize();
        float pad = getPadding();
        float gridW = 4 * (ts + pad) - pad;
        return { (RES_X - gridW) / 2.f, 80.f };
    }

    bool loadFont() {
        std::vector<std::string> paths = {
            "C:/Windows/Fonts/segoeui.ttf",   // основной шрифт Windows 10/11
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/seguisb.ttf",
            "C:/Windows/Fonts/Arial.ttf"
        };
        for (const auto& p : paths) {
            if (font.loadFromFile(p)) {
                std::cout << "font uploaded: " << p << std::endl;
                return true;
            }
        }
        std::cerr << "No fonts found!\n";
        return false;
    }

    void copyField(int dest[4][4], const int src[4][4]) {
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) dest[i][j] = src[i][j];
    }

    void findNewTile() {
        newTileRow = newTileCol = -1;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (field[i][j] == 2 && oldField[i][j] == 0) {
                    newTileRow = i; newTileCol = j; return;
                }
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && currentState == State::Playing) {
            currentState = State::Menu;
            return;
        }

        if (isAnimating || won || lost) return;

        char dir = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) dir = 'w';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dir = 'a';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) dir = 's';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dir = 'd';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) { restart(); return; }

        if (dir == 0) return;

        copyField(oldField, field);
        bool cont = pf.move(dir);
        pf.getField(field);
        pf.getMoveField(moveField);
        lastDir = dir;

        isAnimating = true;
        animProgress = 0.f;
        isSpawning = false;
        spawnProgress = 0.f;

        if (!cont) {
            if (pf.find2048()) won = true;
            else lost = true;
        }
        findNewTile();
    }

    void restart() {
        pf = PlayingField();
        pf.getField(field);
        copyField(oldField, field);
        std::fill(&moveField[0][0], &moveField[0][0] + 16, 0);
        isAnimating = isSpawning = won = lost = false;
        animProgress = spawnProgress = 0.f;
        newTileRow = newTileCol = -1;
    }

    void update(float dt) {
        if (isAnimating) {
            animProgress += dt;
            if (animProgress >= ANIM_DURATION) {
                animProgress = ANIM_DURATION;
                isAnimating = false;
                if (newTileRow != -1 && newTileCol != -1) {
                    isSpawning = true;
                    spawnProgress = 0.f;
                }
            }
        }
        if (isSpawning) {
            spawnProgress += dt;
            if (spawnProgress >= SPAWN_DURATION) {
                spawnProgress = SPAWN_DURATION;
                isSpawning = false;
                newTileRow = newTileCol = -1;
            }
        }
    }

    void drawMenu() {
        window.clear(sf::Color(250, 248, 239));

        // Заголовок
        sf::Text title("2048", font, 120);
        title.setFillColor({ 119, 110, 101 });
        title.setStyle(sf::Text::Bold);
        auto tb = title.getLocalBounds();
        title.setPosition((RES_X - tb.width) / 2.f, 120.f);
        window.draw(title);

        // Кнопка "Начать игру"
        sf::RectangleShape startBtn({ 300, 70 });
        startBtn.setPosition((RES_X - 300) / 2.f, 320.f);
        startBtn.setFillColor({ 187, 173, 160 });
        startBtn.setOutlineThickness(3);
        startBtn.setOutlineColor({ 119, 110, 101 });
        window.draw(startBtn);

        sf::Text startText("Start", font, 36);
        startText.setFillColor(sf::Color::White);
        auto stb = startText.getLocalBounds();
        startText.setPosition(startBtn.getPosition().x + (300 - stb.width) / 2.f,
            startBtn.getPosition().y + (70 - stb.height) / 2.f - 5);
        window.draw(startText);

        // Кнопка "Выход"
        sf::RectangleShape exitBtn({ 300, 70 });
        exitBtn.setPosition((RES_X - 300) / 2.f, 420.f);
        exitBtn.setFillColor({ 187, 173, 160 });
        exitBtn.setOutlineThickness(3);
        exitBtn.setOutlineColor({ 119, 110, 101 });
        window.draw(exitBtn);

        sf::Text exitText("Exit", font, 36);
        exitText.setFillColor(sf::Color::White);
        auto etb = exitText.getLocalBounds();
        exitText.setPosition(exitBtn.getPosition().x + (300 - etb.width) / 2.f,
            exitBtn.getPosition().y + (70 - etb.height) / 2.f - 5);
        window.draw(exitText);

        window.display();
    }

    void render() {
        window.clear(sf::Color(250, 248, 239));

        // ТЕСТ: если увидишь красный квадрат — отрисовка работает
        /*sf::RectangleShape test({ 80, 80 });
        test.setPosition(50, 50);
        test.setFillColor(sf::Color::Red);
        window.draw(test);*/

        // === ЗАГОЛОВОК ===
        sf::Text title("2048", font, 68);
        title.setFillColor({ 119, 110, 101 });
        title.setStyle(sf::Text::Bold);
        auto tb = title.getLocalBounds();
        title.setPosition((RES_X - tb.width) / 2.f, 5.f);
        window.draw(title);

        // === СЕТКА ===
        float ts = getTileSize();
        float pad = getPadding();
        auto offset = getGridOffset();

        sf::RectangleShape gridBg({ 4 * (ts + pad) - pad + 4, 4 * (ts + pad) - pad + 4 });  
        gridBg.setPosition(offset.x - 1, offset.y - 1);
        gridBg.setFillColor({ 187, 173, 160 });
        window.draw(gridBg);

        // === ЛИНИИ РАЗДЕЛИТЕЛЕЙ ===
        sf::RectangleShape line(sf::Vector2f(1.f, 4 * (ts + pad) - pad));
        line.setFillColor(sf::Color(160, 150, 140)); // можно сделать чуть светлее, например {187, 173, 160}

        for (int i = 1; i < 4; ++i) {
            // вертикальные линии
            line.setSize({ 5.f, 4 * (ts + pad) - pad });
            line.setPosition(offset.x + i * (ts + pad) - pad / 2.f, offset.y);
            window.draw(line);

            // горизонтальные линии
            line.setSize({ 4 * (ts + pad) - pad, 5.f });
            line.setPosition(offset.x, offset.y + i * (ts + pad) - pad / 2.f);
            window.draw(line);
        }

        // === ПЛИТКИ ===
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float bx = offset.x + j * (ts + pad);
                float by = offset.y + i * (ts + pad);

                int val = field[i][j];
                sf::Vector2f pos = { bx, by };

                if (isAnimating && oldField[i][j] > 0) {
                    int d = moveField[i][j];
                    float p = std::min(animProgress / ANIM_DURATION, 1.f);
                    float dr = (lastDir == 's' ? 1.f : lastDir == 'w' ? -1.f : 0.f);
                    float dc = (lastDir == 'd' ? 1.f : lastDir == 'a' ? -1.f : 0.f);
                    float lr = i + dr * d * p;
                    float lc = j + dc * d * p;
                    pos = { offset.x + lc * (ts + pad), offset.y + lr * (ts + pad) };
                    val = oldField[i][j];
                }

                if (val == 0 && !(isSpawning && i == newTileRow && j == newTileCol)) continue;

                sf::RectangleShape tile({ ts, ts });
                tile.setPosition(pos);
                tile.setFillColor(getTileColor(val));
                tile.setOutlineThickness(4.f);
                tile.setOutlineColor({ 187, 173, 160 });
                window.draw(tile);

                if (val > 0) {
                    sf::Text txt(std::to_string(val), font, getFontSize(val));
                    txt.setFillColor(getTextColor(val));
                    auto b = txt.getLocalBounds();
                    txt.setPosition(pos.x + (ts - b.width) / 2.f - b.left,
                        pos.y + (ts - b.height) / 2.f - b.top);
                    window.draw(txt);
                }
            }
        }

        // === НОВАЯ ПЛИТКА (spawn) ===
        if ((isSpawning || (!isAnimating && newTileRow != -1)) && newTileRow >= 0) {
            float p = isSpawning ? (spawnProgress / SPAWN_DURATION) : 1.f;
            float sc = 0.3f + 0.7f * std::sin(p * 1.57f);
            float ts2 = ts * sc;
            float px = offset.x + newTileCol * (ts + pad) + (ts - ts2) / 2.f;
            float py = offset.y + newTileRow * (ts + pad) + (ts - ts2) / 2.f;

            sf::RectangleShape st({ ts2, ts2 });
            st.setPosition(px, py);
            st.setFillColor(getTileColor(2));
            st.setOutlineThickness(4.f * sc);
            st.setOutlineColor({ 187, 173, 160 });
            window.draw(st);

            sf::Text stxt("2", font, static_cast<int>(getFontSize(2) * sc));
            stxt.setFillColor(getTextColor(2));
            auto b = stxt.getLocalBounds();
            stxt.setPosition(px + (ts2 - b.width) / 2.f - b.left, py + (ts2 - b.height) / 2.f - b.top);
            window.draw(stxt);
        }

        // === ОВЕРЛЕЙ ===
        if (won || lost) {
            sf::RectangleShape ov({ (float)RES_X, (float)RES_Y });
            ov.setFillColor({ 0, 0, 0, 160 });
            window.draw(ov);

            sf::Text msg(won ? "You Win!" : "Game Over", font, 72);
            msg.setFillColor(won ? sf::Color(237, 194, 46) : sf::Color(246, 94, 59));
            msg.setStyle(sf::Text::Bold);
            auto mb = msg.getLocalBounds();
            msg.setPosition((RES_X - mb.width) / 2.f, RES_Y / 2.f - 80.f);
            window.draw(msg);

            sf::Text h("Press R to restart", font, 28);
            h.setFillColor(sf::Color::White);
            auto hb = h.getLocalBounds();
            h.setPosition((RES_X - hb.width) / 2.f, RES_Y / 2.f + 20.f);
            window.draw(h);
        }

        window.display();
    }

public:
    SFML2048() : window(sf::VideoMode(RES_X, RES_Y), "2048 - SFML Edition", sf::Style::Close) {
        window.setFramerateLimit(60);
        loadFont();

        pf = PlayingField();
        pf.getField(field);
        copyField(oldField, field);
        std::fill(&moveField[0][0], &moveField[0][0] + 16, 0);

        std::cout << "Game started! WASD - move, R - restart\n";
    }

    void run() {
        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) window.close();
                if (e.type == sf::Event::KeyPressed && (won || lost) && e.key.code == sf::Keyboard::R)
                    restart();
            }

            // Обработка меню
            if (currentState == State::Menu) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Кнопка "Начать игру"
                    if (mousePos.x > (RES_X - 300) / 2.f && mousePos.x < (RES_X + 300) / 2.f &&
                        mousePos.y > 320 && mousePos.y < 390) {
                        currentState = State::Playing;
                        restart();           // сбрасываем поле
                        sf::sleep(sf::milliseconds(200)); // чтобы не было двойного клика
                    }

                    // Кнопка "Выход"
                    if (mousePos.x > (RES_X - 300) / 2.f && mousePos.x < (RES_X + 300) / 2.f &&
                        mousePos.y > 420 && mousePos.y < 490) {
                        window.close();
                    }
                }
            }

            handleInput();
            update(clock.restart().asSeconds());
            if (currentState == State::Menu) {
                drawMenu();
            }
            else {
                render();
            }
        }
    }
};

int main() {
    SFML2048 game;
    game.run();
    return 0;
}
