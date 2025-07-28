#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include "sqlite3.h"

using namespace std;
using namespace sf;

class Background {
    public:
    vector<RectangleShape> objects;
    vector<int> delays;

    Background(vector<Color>* colors, RectangleShape* blick){
        for (int i=0; i<20; i++){
            (*blick).setFillColor((*colors)[rand()%4]);
            (*blick).setPosition(Vector2f(rand()%820, 30*i));
            this->objects.push_back(*blick);
            this->delays.push_back(rand()%20);
        }
    }

    void update(RenderWindow* window){
        for (int i=0; i<(this->objects).size(); i++){
            if ((this->objects)[i].getPosition().y + 2 < 820){ // Если не достиг низа экрана
                (this->objects)[i].move(Vector2f(0.f, 1.4f)); // то двигается
            } else { // иначе
                (this->objects)[i].setPosition(Vector2f((this->objects)[i].getPosition().x, 0)); // y = 0
            }
            if ((this->delays)[i] < 60){
                (this->delays)[i] += 1;
            } else {
                (this->delays)[i] = 0;
            }
            
            if ((this->delays)[i] < 30){
                (*window).draw((this->objects)[i]);
            }
        }
    }
};

class Alien {
    public:

    RectangleShape object;
    string type;
    bool isFlying = false;
    int flyAnimation;
    bool isFlyingLeft;
    double prevX, prevY;
    vector<Vector2f> speeds;


    Alien(RectangleShape* alienObj, string type){
        this->object = *alienObj;
        this->type = type;
    }

    void update(RenderWindow* window, vector<Alien>* Aliens, bool* isLeft, time_t* currTime){
        if (!(this->isFlying)){
            if (this->object.getPosition().x == 1 && (*isLeft)){
                (*isLeft) = false;
            } else if (this->object.getPosition().x == 781 && not(*isLeft)){
                (*isLeft) = true;
                this->object.move(Vector2f(4.f, 0.f));
            }
            if (*isLeft){
                this->object.move(Vector2f(-2.f, 0.f));
            } else {
                this->object.move(Vector2f(2.f, 0.f));
            }
        } else {
            if (*isLeft){
                this->prevX -= 2;
            } else {
                this->prevX += 2;
            }
            this->flyAnimation += 1;
            if (this->flyAnimation <= 60){
                this->object.move(this->speeds[0]);
            } else if (this->flyAnimation <= 120){
                this->object.move(this->speeds[1]);
            } else if (this->flyAnimation <= 180){
                // this->object.move();
            } else {
                this->object.setPosition(Vector2f(this->prevX, this->prevY));
                this->isFlying = false;
            }
        }
        (*window).draw(this->object);
    }
};

class Rocket {
    public:

    RectangleShape object;
    float speedX = 0;
    vector<RectangleShape> bullets;
    time_t lastShot = time(0);
    const int delayShot = 2; 

    Rocket(RectangleShape* rocketObj){
        this->object = *rocketObj;
    }

    void update(RenderWindow* window, vector<Alien>* Aliens, FloatRect* bulletCollide, FloatRect* alienCollide){
        this->object.move(Vector2f(this->speedX, 0.f));
        if (this->object.getPosition().x > 825-50){
            (this->object).setPosition(Vector2f(825-50, this->object.getPosition().y));
        } else if (this->object.getPosition().x < 0){
            (this->object).setPosition(Vector2f(0.f, this->object.getPosition().y));
        }
        for (int j=0; j<(this->bullets.size()); j++){
            this->bullets[j].move(Vector2f(0.f, -10.f));
            (*window).draw(this->bullets[j]);
            if (this->bullets[j].getPosition().y < 0){
                this->bullets.erase(this->bullets.begin() + j);
            }
            (*bulletCollide) = FloatRect(this->bullets[j].getPosition(), this->bullets[j].getSize());
            for (int k=0; k<(*Aliens).size(); k++){
                (*alienCollide) = FloatRect((*Aliens)[k].object.getPosition(), (*Aliens)[k].object.getSize());
                if ((*bulletCollide).intersects((*alienCollide))){
                    this->bullets.erase(this->bullets.begin()+j);
                    (*Aliens).erase((*Aliens).begin()+k);
                }
            }
        }
        (*window).draw(this->object);
    }
};

void createAliens(RectangleShape* alien, Texture* alien_sm, Texture* alien_mid, Texture* alien_big, RectangleShape* alien_biggy, vector<Alien>* Aliens){
    // small
    (*alien).setTexture(alien_sm);
    for (int i=0; i<3; i++){ // 3 строки инопланетян(small)
        for (int j=0; j<10; j++){ // 10 инопланетян в строке(small)
            (*alien).setPosition(Vector2f((175+46*j), (205+35*i)));
            (*Aliens).push_back(Alien(alien, "small"));
        } 
    }

    // mid
    (*alien).setTexture(alien_mid);
    for (int i=0; i<8; i++){ // 8 инопланетян(mid)
        (*alien).setPosition(Vector2f(221+46*i, 170));
        (*Aliens).push_back(Alien(alien, "mid"));
    }
    
    // big
    (*alien).setTexture(alien_big);
    for (int i=0; i<6; i++){ // 6 инопланетян(big)
        (*alien).setPosition(Vector2f(267+46*i, 135));
        (*Aliens).push_back(Alien(alien, "big"));
    }

    // biggest
    (*alien_biggy).setPosition(Vector2f(311.f, 95.f)); // 2 инопланетянина(biggest)
    (*Aliens).push_back(Alien(alien_biggy, "biggy"));
    (*alien_biggy).setPosition(Vector2f(449.f, 95.f));
    (*Aliens).push_back(Alien(alien_biggy, "biggy"));
}

void checkForFlying(vector<Alien>* Aliens, time_t* lastFlyTime, time_t* currTime, vector<Rocket>* Rockets){
    if ((*lastFlyTime) + 3 <= (*currTime)){
        double min = 825;
        double max = 0;
        for (int i=0; i<(*Aliens).size(); i++){
            if (!((*Aliens)[i].isFlying) && ((*Aliens)[i].object.getPosition().x < min)){
                min = (*Aliens)[i].object.getPosition().x;
            } else if (!((*Aliens)[i].isFlying) && ((*Aliens)[i].object.getPosition().x > max)){
                max = (*Aliens)[i].object.getPosition().x;
            }
        }
        vector<int> indexesFly;

        for (int i=0; i<(*Aliens).size(); i++){
            if (!((*Aliens)[i].isFlying) && (((*Aliens)[i].object.getPosition().x == min) || ((*Aliens)[i].object.getPosition().x == max))){
                indexesFly.push_back(i);
            }
        }

        int randIndex = indexesFly[rand()%(indexesFly.size())];
        (*Aliens)[randIndex].isFlying = true;
        (*Aliens)[randIndex].flyAnimation = 0;
        (*Aliens)[randIndex].prevX = (*Aliens)[randIndex].object.getPosition().x;
        (*Aliens)[randIndex].prevY = (*Aliens)[randIndex].object.getPosition().y;
        if ((*Aliens)[randIndex].object.getPosition().x < 413){
            (*Aliens)[randIndex].isFlyingLeft = true;
        } else {
            (*Aliens)[randIndex].isFlyingLeft = false;
        }
        (*Aliens)[randIndex].speeds.clear();
        (*Aliens)[randIndex].speeds.push_back(Vector2f(-1.0 * (*Aliens)[randIndex].object.getPosition().x/60, (625-(*Aliens)[randIndex].object.getPosition().y)/2/60));
        min = 825;
        for (int i=0; i<(*Rockets).size(); i++){
            if (min > (*Rockets)[i].object.getPosition().x) {
                min = (*Rockets)[i].object.getPosition().x;
            }
        }
        (*Aliens)[randIndex].speeds.push_back(Vector2f(1.0 * min / 60, (625-(*Aliens)[randIndex].object.getPosition().y)/2/60 ));

        (*lastFlyTime) = (*currTime);
    }
}


int main(){
    RenderWindow window(VideoMode(825, 625), "2 player");
    time_t currTime = time(nullptr); // Текущее время
    time_t lastFlyTime = time(nullptr); // Текущее время
    
    
    // Ракеты
    vector<Rocket> RocketShips;
    
    Texture rocketTexture1;
    rocketTexture1.loadFromFile("resources/textures/rocketShip.png");

    Texture rocketTexture2;
    rocketTexture2.loadFromFile("resources/textures/rocketShip2.png");

    RectangleShape rocket(Vector2f(50.f, 70.f));

    rocket.setTexture(&rocketTexture1);
    rocket.setPosition(Vector2f(225.f, 550.f));
    RocketShips.push_back(Rocket(&rocket));

    rocket.setTexture(&rocketTexture2);
    rocket.setPosition(Vector2f(600.f, 550.f));
    RocketShips.push_back(Rocket(&rocket));

    RectangleShape bullet(Vector2f(3.f, 15.f));
    bullet.setFillColor(Color(255, 154, 57));


    vector<Color> colors = {Color::White, Color::Green, Color::Red, Color(206, 138, 255)};
    RectangleShape blick(Vector2f(5.f, 5.f));
    Background Back(&colors, &blick);


    // Инопланетяне
    RectangleShape alien(Vector2f(42.f, 25.f));
    RectangleShape alienBiggest(Vector2f(42.f, 42.f));
    Texture alien_sm, alien_mid, alien_big, alien_biggy;
    alien_sm.loadFromFile("resources/textures/alien_small.png");
    alien_mid.loadFromFile("resources/textures/alien_mid.png"); 
    alien_big.loadFromFile("resources/textures/alien_big.png");
    alien_biggy.loadFromFile("resources/textures/alien_biggest.png"); 
    alienBiggest.setTexture(&alien_biggy);
    vector<Alien> Aliens;
    createAliens(&alien, &alien_sm, &alien_mid, &alien_big, &alienBiggest, &Aliens);
    FloatRect bulletCollide;
    FloatRect alienCollide;
    bool isLeft = true;


    // Игровой цикл
    window.setVerticalSyncEnabled(true); // VSync 
    while (window.isOpen()){
        Event event; // Текущее событие
        currTime = time(nullptr); // Текущее время
        
        while (window.pollEvent(event)){
            if (event.type == Event::Closed){
                window.close(); // Кнопка закрытия окна
            }

            if (event.type == Event::KeyPressed){
                if (Keyboard::isKeyPressed(Keyboard::A)){
                    RocketShips[0].speedX = -4.5f;
                }
                if (Keyboard::isKeyPressed(Keyboard::D)){
                    RocketShips[0].speedX = 4.5f;
                }
                if (Keyboard::isKeyPressed(Keyboard::Left)){
                    RocketShips[1].speedX = -4.5f;
                }
                if (Keyboard::isKeyPressed(Keyboard::Right)){
                    RocketShips[1].speedX = 4.5f;
                }
                if (Keyboard::isKeyPressed(Keyboard::Z) && (currTime - RocketShips[0].lastShot) >= RocketShips[0].delayShot && RocketShips[0].bullets.size() == 0){
                    bullet.setPosition(Vector2f(RocketShips[0].object.getPosition().x + RocketShips[0].object.getSize().x/2, RocketShips[0].object.getPosition().y));
                    RocketShips[0].bullets.push_back(bullet);
                    RocketShips[0].lastShot = currTime;
                }
                if (Keyboard::isKeyPressed(Keyboard::Up) && (currTime - RocketShips[1].lastShot) >= RocketShips[1].delayShot && RocketShips[1].bullets.size() == 0){
                    bullet.setPosition(Vector2f(RocketShips[1].object.getPosition().x + RocketShips[1].object.getSize().x/2, RocketShips[1].object.getPosition().y));
                    RocketShips[1].bullets.push_back(bullet);
                    RocketShips[1].lastShot = currTime;
                }
            }

            if (event.type == Event::KeyReleased){ // Кнопка отпущена
                if (event.key.code == Keyboard::A || event.key.code == Keyboard::D){ // Если отпущена кнопка управления
                    RocketShips[0].speedX = 0; // Зануляем скорость
                }
                if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right){ // Если отпущена кнопка управления
                    RocketShips[1].speedX = 0; // Зануляем скорость
                }
            }
        }

        window.clear(Color::Black);

        Back.update(&window);

        for (int i=0; i<RocketShips.size(); i++){
            RocketShips[i].update(&window, &Aliens, &bulletCollide, &alienCollide);
        }

        for (int i=0; i<Aliens.size(); i++){
            Aliens[i].update(&window, &Aliens, &isLeft, &currTime);
        }
        checkForFlying(&Aliens, &lastFlyTime, &currTime, &RocketShips);

        window.display();
    }

}