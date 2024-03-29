#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

class Ball
{
public:
    sf::Vector2f _pos;
    sf::Vector2f _vel;
    sf::CircleShape _ball;
    float _radius { rand() % 5 + 15 };
    float _speed { rand() % 100 + 75 };

    Ball();
    sf::CircleShape getBall();
    void update(float dt, sf::RenderWindow& window);
};

Ball::Ball()
{
    _ball.setOutlineThickness(4);
    _ball.setOutlineColor(sf::Color::Black);
    _ball.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
    _ball.setRadius(_radius);
}

sf::CircleShape Ball::getBall()
{
    // Return the ball shape
    return _ball;
}

void Ball::update(float dt, sf::RenderWindow& window)
{
    // Move the ball
    _ball.move(_vel * dt);

    // Collisions between screen
    sf::Vector2f getPos = _ball.getPosition();
    sf::Vector2u winSize = window.getSize();
    // Collide with Left and Right screen
    if (getPos.x < 0 || getPos.x + _radius * 2 > winSize.x)
    {
        _vel.x = -_vel.x;
    }

    // Collide with Top and Bottom screen
    if (getPos.y < 0 || getPos.y + _radius * 2 > winSize.y)
    {
        _vel.y = -_vel.y;
    }
}

bool checkCollision(const Ball& b1, const Ball& b2)
{
    // Calculate the distance between the centers of the two balls
    sf::Vector2f dis = b1._ball.getPosition() - b2._ball.getPosition();

    // Calculate the Euclidean norm of the distance
    float n { std::sqrt(dis.x * dis.x + dis.y * dis.y) };

    // Calculate the sum of the radius of the two balls
    float minDis { b1._radius + b2._radius };

    // Check if the distance is less than or equal to the sum of the radius
    // indicating a collision
    return n <= minDis;
}

void collide(Ball& b1, Ball& b2)
{
    // Determine the direction vector of the collision
    // and normalize the direction vector
    sf::Vector2f delta = b1._ball.getPosition() - b2._ball.getPosition();
    delta /= std::sqrt(delta.x * delta.x + delta.y * delta.y);

    // Calculate the relative velocity between the balls
    // and calculate the dot product between the relative velocity
    // and the collision direction
    sf::Vector2f rV = b1._vel - b2._vel;
    float dot { rV.x * delta.x + rV.y * delta.y };

    // Apply collision response only if the
    // balls are moving towards each other
    if (dot < 0)
    {
        sf::Vector2f colVector = delta * dot;
        b1._vel -= colVector;
        b2._vel += colVector;
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    using kb = sf::Keyboard;

    // Set the size of the screen
    unsigned const int WIDTH { 1200 };
    unsigned const int HEIGHT { 900 };
    sf::VideoMode vm(WIDTH, HEIGHT);
    sf::RenderWindow window(vm, "SFML Bouncing Balls AABB");
    window.setFramerateLimit(75);

    std::vector<Ball> balls;

    sf::Clock clock;
    // Timer for spawn interval
    sf::Time spawnInterval = sf::seconds(.5f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close the window if press X
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // ---- INPUT ----

        // Press Esc key to close the window
        if (kb::isKeyPressed(kb::Escape))
        {
            window.close();
        }

        // Spawn balls if press Space
        if (kb::isKeyPressed(kb::Space) && clock.getElapsedTime() >= spawnInterval)
        {
            Ball ball;
            // Spawn the balls at the center of the screen
            ball._ball.setPosition(window.getSize().x / 2, window.getSize().y / 2);
            // Set the angle of the spawn ball
            float angle { static_cast<float>(rand() % 360) * 3.141 / 180.f };
            // Set the velocity
            ball._vel = sf::Vector2f(ball._speed * std::cos(angle), ball._speed * std::sin(angle));
            balls.push_back(ball);
            clock.restart();
        }

        // ---- UPDATE ----

        sf::Time dt = sf::seconds(1.f / 75.f);
        float dtAsSeconds = dt.asSeconds();

        // Update balls and update the collision
        for (std::size_t i = 0; i < balls.size(); ++i)
        {
            balls[i].update(dtAsSeconds, window);

            // Check collisions between balls
            for (std::size_t j = i + 1; j < balls.size(); ++j)
            {
                if (checkCollision(balls[i], balls[j]))
                {
                    collide(balls[i], balls[j]);
                }
            }
        }

        // ---- DRAW ----

        window.clear(sf::Color(40, 40, 100));
        // Draw the balls
        for (auto& ball : balls)
        {
            window.draw(ball.getBall());
        }
        window.display();
    }

    return 0;
}
