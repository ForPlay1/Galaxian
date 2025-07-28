#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

// g++ -c main.cpp -I"G:\SFML-2.6.2\include"
// g++ main.o -o main -L"G:\SFML-2.6.2\lib" -lsfml-graphics -lsfml-window -lsfml-system
// ./main

int main()
{
    RenderWindow window(VideoMode(500, 500), "SFML works!");
    CircleShape shape(100.f); // Можно дополнительно указать кол-во сторон для правильного n-угольника
    shape.setFillColor(Color(100, 100, 100));
    shape.setOutlineThickness(5);
    shape.setOutlineColor(Color(100, 100, 255));
    shape.setPosition(100, 100);
    shape.rotate(45);
    RectangleShape rect(Vector2f(300, 300));
    
    Texture texture;
    texture.loadFromFile("img.jpg");
    
    ConvexShape star(5);
    star.setPoint(0, Vector2f(100, 200));
    star.setPoint(1, Vector2f(161, 10));
    star.setPoint(2, Vector2f(222, 200));
    star.setPoint(3, Vector2f(60, 82));
    star.setPoint(4, Vector2f(262, 82));
    star.setTexture(&texture);
    // star.setTextureRect(IntRect(250, 0, 100, 100));
    // window.setVerticalSyncEnabled(true); VSync
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            
            if (event.type == Event::KeyPressed){
                if (Keyboard::isKeyPressed(Keyboard::Enter)){
                    std::cout << 1;
                    star.move(Vector2f(1, 1));
                }
            }
        
        }

        window.clear(Color(100, 100, 100)); // Залить цветом
        // window.draw(rect);
        // window.draw(shape);
        window.draw(star);
        window.display();
    }

    return 0;
}