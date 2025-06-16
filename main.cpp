#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

struct FallingObject {
    sf::Sprite sprite;
    bool isBad;

    FallingObject(float x, const sf::Texture& texture, bool bad) {
        sprite.setTexture(texture);
        sprite.setScale(0.2, 0.2); 
        sprite.setPosition(x, -sprite.getGlobalBounds().height); 
        isBad =bad;
    }///setting up bad (bomb) object and good (apple object). STarting them above screen and making their size smaller

    void fall(float speed) {
        sprite.move(0, speed);
    }

    bool isOffScreen(int windowHeight) const {
        return sprite.getPosition().y > windowHeight;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Falling Game");
    window.setFramerateLimit(60);

    int score=0;
    int missed=0;
    bool isGameOver=false;
///setup font, score, time and gameover texts
    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
    
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(18);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(18);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(10, 35);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(250, 250);
    gameOverText.setString("GAME OVER :(\nPress R to restart!");

    sf::Texture goodTexture, badTexture;
    goodTexture.loadFromFile("good.png");
    badTexture.loadFromFile("bad.png");
///setup backfground
    int r=255, g= 0, b=0;
    int colourchange=1;
    enum class Phase { RED_TO_GREEN, GREEN_TO_BLUE, BLUE_TO_RED };
    Phase phase=Phase::RED_TO_GREEN;
//setup bombs&apples, timing
    std::vector<FallingObject> objects;
    sf::Clock spawnClock, gameClock;
    float spawnInterval=1.2;
    float gravity=1.0;

    srand(static_cast<unsigned>(time(nullptr)));

    while (window.isOpen()) {
        float elapsedTime=gameClock.getElapsedTime().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type==sf::Event::Closed)
                window.close();
       
///mouse clicks                

        if (!isGameOver && event.type==sf::Event::MouseButtonPressed && event.mouseButton.button==sf::Mouse::Left) {
        sf::Vector2i mousePixel=sf::Mouse::getPosition(window);
        sf::Vector2f mousePos=window.mapPixelToCoords(mousePixel);

        for (int i=0; i<objects.size(); i++) {
            if (objects[i].sprite.getGlobalBounds().contains(mousePos)) {
                if (objects[i].isBad)
                    isGameOver=true;
                else
                    score++;
                objects.erase(objects.begin() + i);
                break;
            }
        }}
///restart feature (there was no restart feature in concept but I think it is very useful for my small game)
        if (isGameOver && event.type== sf::Event::KeyPressed && event.key.code==sf::Keyboard::R) {
        isGameOver=false;
        score=0;
        missed=0;
        gameClock.restart();
        spawnClock.restart();
        objects.clear();


    }

        }

        gravity=1.0+elapsedTime*0.12345;

///RGB colours over time:

        switch (phase) {
            case Phase::RED_TO_GREEN:
                g += colourchange; r -= colourchange;
                if (g>=255) { g=255; phase = Phase::GREEN_TO_BLUE; }
                if (r<0) r=0;
                break;
            case Phase::GREEN_TO_BLUE:
                b += colourchange; g -= colourchange;
                if (b>=255) { b = 255; phase = Phase::BLUE_TO_RED; }
                if (g < 0) g=0;
                break;
            case Phase::BLUE_TO_RED:
                r += colourchange; b -= colourchange;
                if (r >= 255) { r = 255; phase = Phase::RED_TO_GREEN; }
                if (b < 0) b = 0;
                break;
        }

///apples&bombs spawn over seconds

        if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
            float x=rand()%700+50; 
            bool isBad=(rand()% 5==0); 
            if (isBad)
                objects.emplace_back(x, badTexture, true);
            else
                objects.emplace_back(x, goodTexture, false);
            spawnClock.restart();
        }

///Updating objects:

        for (auto& obj : objects)
            obj.fall(gravity);

///Erasing objects which go down the screen (also counting misses):

        for (size_t i=0; i<objects.size(); ) {
        if (objects[i].isOffScreen(window.getSize().y)) {
        if (!objects[i].isBad)
            missed++;
        objects.erase(objects.begin()+i);
        } else {
        ++i;}
    }


    if (missed>=5)
    isGameOver =true;
    
 ///FInally) drawing everything:

    window.clear(sf::Color(r, g, b));
    for (const auto& obj : objects)
    window.draw(obj.sprite);
    scoreText.setString("Score: "+std::to_string(score)+" | Missed: "+std::to_string(missed));
    int seconds=static_cast<int>(elapsedTime);
    timeText.setString("Time: "+std::to_string(seconds)+" s");

    window.draw(scoreText);
    window.draw(timeText);
    if (isGameOver)
    window.draw(gameOverText);
    window.display();
    }

    return 0;
}
