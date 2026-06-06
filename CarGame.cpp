#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include <iostream>

struct EnemyCar
{
    sf::RectangleShape body;
    float speed;
};

// ================= LOAD HIGHSCORE =================
int loadHighScore()
{
    std::ifstream file("highscore.txt");
    int score = 0;
    if (file.is_open())
        file >> score;
    return score;
}

// ================= SAVE HIGHSCORE =================
void saveHighScore(int score)
{
    std::ofstream file("highscore.txt");
    if (file.is_open())
        file << score;
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({ 500, 700 }), "Modern Car Game");
    window.setFramerateLimit(60);

    // ================= ROAD =================
    sf::RectangleShape road({ 300.f, 700.f });
    road.setFillColor(sf::Color(50, 50, 50));
    road.setPosition({ 100.f, 0.f });

    sf::RectangleShape leftBorder({ 8.f, 700.f });
    leftBorder.setFillColor(sf::Color::Yellow);
    leftBorder.setPosition({ 100.f, 0.f });

    sf::RectangleShape rightBorder({ 8.f, 700.f });
    rightBorder.setFillColor(sf::Color::Yellow);
    rightBorder.setPosition({ 392.f, 0.f });

    // ================= PLAYER CAR =================
    sf::RectangleShape carBody({ 55.f, 110.f });
    carBody.setFillColor(sf::Color(30, 144, 255));
    carBody.setPosition({ 220.f, 560.f });

    sf::RectangleShape windshield({ 35.f, 30.f });
    windshield.setFillColor(sf::Color(180, 220, 255));

    sf::CircleShape frontLight1(5.f);
    sf::CircleShape frontLight2(5.f);
    frontLight1.setFillColor(sf::Color::Yellow);
    frontLight2.setFillColor(sf::Color::Yellow);

    sf::CircleShape tailLight1(5.f);
    sf::CircleShape tailLight2(5.f);
    tailLight1.setFillColor(sf::Color::Red);
    tailLight2.setFillColor(sf::Color::Red);

    // ================= ROAD LINES =================
    std::vector<sf::RectangleShape> lines;
    for (int i = 0; i < 12; i++)
    {
        sf::RectangleShape line({ 10.f, 50.f });
        line.setFillColor(sf::Color::White);
        line.setPosition({ 245.f, float(i * 80) });
        lines.push_back(line);
    }

    // ================= TREES =================
    std::vector<sf::CircleShape> trees;
    for (int i = 0; i < 12; i++)
    {
        sf::CircleShape tree(20.f);
        tree.setFillColor(sf::Color(0, 180, 0));

        if (i % 2 == 0)
            tree.setPosition({ 30.f, float(i * 90) });
        else
            tree.setPosition({ 440.f, float(i * 90) });

        trees.push_back(tree);
    }

    // ================= ENEMIES =================
    std::vector<EnemyCar> enemies;
    sf::Clock spawnClock;

    // ================= FONT =================
    sf::Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\arialbd.ttf"))
    {
        std::cout << "Font load failed!\n";
        return -1;
    }

    // ================= TEXT =================
    sf::Text scoreText(font);
    scoreText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({ 10.f, 10.f });

    sf::Text highText(font);
    highText.setCharacterSize(24);
    highText.setFillColor(sf::Color::Yellow);
    highText.setPosition({ 10.f, 45.f });

    sf::Text gameOverText(font);
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER\nPress R");
    gameOverText.setPosition({ 110.f, 280.f });

    // 🟡 NEW PAUSE TEXT
    sf::Text pauseText(font);
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::Cyan);
    pauseText.setString("PAUSED\nPress P");
    pauseText.setPosition({ 140.f, 280.f });

    // ================= GAME VARIABLES =================
    bool gameOver = false;
    bool paused = false;

    int score = 0;
    int highScore = loadHighScore();

    float worldSpeed = 7.f;
    float speedTimer = 0.f;

    // ================= LOOP =================
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // ================= PAUSE TOGGLE =================
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
        {
            static bool canToggle = true;

            if (canToggle)
            {
                paused = !paused;
                canToggle = false;
            }
        }
        else
        {
            static bool canToggle = true;
            canToggle = true;
        }

        // ================= RESTART =================
        if (gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
        {
            enemies.clear();
            carBody.setPosition({ 220.f, 560.f });

            score = 0;
            worldSpeed = 7.f;
            gameOver = false;
            paused = false;
        }

        // ================= UPDATE =================
        if (!gameOver && !paused)
        {
            speedTimer += 0.016f;
            if (speedTimer > 5.f)
            {
                worldSpeed += 0.2f;
                speedTimer = 0.f;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) &&
                carBody.getPosition().x > 110)
            {
                carBody.move({ -8.f, 0.f });
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) &&
                carBody.getPosition().x < 335)
            {
                carBody.move({ 8.f, 0.f });
            }

            for (auto& line : lines)
            {
                line.move({ 0.f, worldSpeed });
                if (line.getPosition().y > 720)
                    line.setPosition({ 245.f, -60.f });
            }

            for (auto& tree : trees)
            {
                tree.move({ 0.f, worldSpeed });

                if (tree.getPosition().y > 740)
                    tree.setPosition(tree.getPosition().x < 100 ?
                        sf::Vector2f(30.f, -40.f) :
                        sf::Vector2f(440.f, -40.f));
            }

            if (spawnClock.getElapsedTime().asSeconds() > 1.f)
            {
                EnemyCar enemy;
                enemy.body.setSize({ 50.f, 110.f });
                enemy.body.setFillColor(sf::Color(
                    80 + std::rand() % 175,
                    80 + std::rand() % 175,
                    80 + std::rand() % 175
                ));

                float lanes[3] = { 130.f, 220.f, 310.f };
                enemy.body.setPosition({ lanes[std::rand() % 3], -120.f });
                enemy.speed = worldSpeed + (std::rand() % 3);

                enemies.push_back(enemy);
                spawnClock.restart();
            }

            for (auto& enemy : enemies)
            {
                enemy.body.move({ 0.f, enemy.speed });

                if (carBody.getGlobalBounds()
                    .findIntersection(enemy.body.getGlobalBounds()))
                {
                    gameOver = true;

                    if (score > highScore)
                    {
                        highScore = score;
                        saveHighScore(highScore);
                    }
                }
            }

            score++;
        }

        // ================= UI =================
        scoreText.setString("Score: " + std::to_string(score / 10));
        highText.setString("High Score: " + std::to_string(highScore / 10));

        // ================= POSITION CAR PARTS =================
        windshield.setPosition({ carBody.getPosition().x + 10.f,
                                  carBody.getPosition().y + 20.f });

        frontLight1.setPosition({ carBody.getPosition().x + 8.f,
                                  carBody.getPosition().y + 100.f });

        frontLight2.setPosition({ carBody.getPosition().x + 42.f,
                                  carBody.getPosition().y + 100.f });

        tailLight1.setPosition({ carBody.getPosition().x + 8.f,
                                 carBody.getPosition().y + 5.f });

        tailLight2.setPosition({ carBody.getPosition().x + 42.f,
                                 carBody.getPosition().y + 5.f });

        // ================= DRAW =================
        window.clear(sf::Color(20, 120, 20));

        window.draw(road);
        window.draw(leftBorder);
        window.draw(rightBorder);

        for (auto& t : trees) window.draw(t);
        for (auto& l : lines) window.draw(l);
        for (auto& e : enemies) window.draw(e.body);

        window.draw(carBody);
        window.draw(windshield);
        window.draw(frontLight1);
        window.draw(frontLight2);
        window.draw(tailLight1);
        window.draw(tailLight2);

        window.draw(scoreText);
        window.draw(highText);

        // ================= GAME STATES =================
        if (paused)
        {
            sf::RectangleShape overlay({ 500.f, 700.f });
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);
            window.draw(pauseText);
        }

        if (gameOver)
        {
            sf::RectangleShape overlay({ 500.f, 700.f });
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}