#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "particle.h"
#include "constraint.h"
#include "input_handler.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;
const float PARTICLE_RADIOUS = 8.0f;
const float GRAVITY = 10.0f;
const float TIME_STEP = 0.1f;

const int ROW = 20;
const int COL = 30;
const float REST_DISTANCE = 25.0f;

// Gentle breeze: WIND_STRENGTH = 5.0f, WIND_FREQUENCY = 0.1f
// Strong gusts: WIND_STRENGTH = 30.0f, WIND_FREQUENCY = 0.4f
// default      : WIND_STRENGTH = 15.0f; WIND_FREQUENCY = 0.2f;
const float WIND_STRENGTH = 5.0f;
const float WIND_FREQUENCY = 0.1f; // How fast wind changes

sf::Vector2f calculate_wind(float time) {
    // Creates oscillating wind force
    float wind_x = WIND_STRENGTH * std::sin(time * WIND_FREQUENCY);
    return sf::Vector2f(wind_x, 0);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cloth Simulation");
    window.setFramerateLimit(60);

    sf::Clock clock;

    std::vector<Particle> particles;
    std::vector<Constraint> constraints;

    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            float x = col * REST_DISTANCE + WIDTH/3;
            float y = row * REST_DISTANCE + HEIGHT/3;
            bool pinned = (row == 0);
            particles.emplace_back(x, y, pinned);
        }
    }

    // Initialize constraints
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            if (col < COL - 1) {
                // Horizontal constraint
                constraints.emplace_back(&particles[row * COL + col], &particles[row * COL + col + 1]);
            }
            if (row < ROW - 1) {
                // Vertical constraint
                constraints.emplace_back(&particles[row * COL + col], &particles[(row + 1) * COL + col]);
            }
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // handle mouse clicks
            InputHandler::handle_mouse_click(event, particles, constraints);

        }

        float current_time = clock.getElapsedTime().asSeconds();
        sf::Vector2f wind = calculate_wind(current_time);

        //apply gravity and wind, then update particles
        for (auto& particle : particles) {
            particle.apply_force(sf::Vector2f(0, GRAVITY));
            particle.apply_force(wind);  // Apply wind force
            particle.update(TIME_STEP);
            particle.constrain_to_bounds(WIDTH, HEIGHT);
        }

        for (size_t i = 0; i < 5; i++) {
            for (auto& constraint : constraints) {
                constraint.satisfy();
            }
        }
        
        window.clear(sf::Color(30, 30, 30));  // Dark gray background

        // Draw particles as points
        for (const auto& particle : particles) {
            sf::Vertex point(particle.position, sf::Color::White);
            window.draw(&point, 1, sf::Points);
        }


        // Draw constraints as colored lines
        for (const auto& constraint : constraints) {
            if (!constraint.active) continue;
            
            // Calculate color based on stretch
            sf::Vector2f delta = constraint.p2->position - constraint.p1->position;
            float stretch = std::hypot(delta.x, delta.y) / constraint.initial_length;
            uint8_t r = static_cast<uint8_t>(std::min(255.0f, stretch * 128));
            uint8_t b = static_cast<uint8_t>(std::max(0.0f, 255 - stretch * 128));
            
            sf::Vertex line[] = {
                sf::Vertex(constraint.p1->position, sf::Color(r, 128, b)),
                sf::Vertex(constraint.p2->position, sf::Color(r, 128, b))
            };
            window.draw(line, 2, sf::Lines);
        }

        // Draw particles as small white dots
        for (const auto& particle : particles) {
            sf::CircleShape circle(2.0f);
            circle.setFillColor(sf::Color::White);
            circle.setPosition(particle.position.x - 2.0f, particle.position.y - 2.0f);
            window.draw(circle);
        }

        window.display();
    }
}