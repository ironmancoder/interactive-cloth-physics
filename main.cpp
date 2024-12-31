#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include <thread>
#include "particle.h"
#include "constraint.h"
#include "input_handler.h"
#include "spatial_hash.h"

// Window and simulation constants
const int WIDTH = 1920;
const int HEIGHT = 1080;
const float PARTICLE_RADIOUS = 8.0f;
const float GRAVITY = 10.0f;
const float TIME_STEP = 0.1f;
const int ROW = 20;
const int COL = 30;
const float REST_DISTANCE = 25.0f;

// Wind settings
const float WIND_STRENGTH = 15.0f;
const float WIND_FREQUENCY = 0.2f;

// Performance optimization constants 
const float CELL_SIZE = REST_DISTANCE * 2.0f;
const int THREAD_COUNT = std::thread::hardware_concurrency();

// Helper function for wind calculation
sf::Vector2f calculate_wind(float time) {
    float wind_x = WIND_STRENGTH * std::sin(time * WIND_FREQUENCY);
    return sf::Vector2f(wind_x, 0);
}

// Function for parallel particle updates
void updateParticleRange(std::vector<Particle>& particles, 
                        size_t start, size_t end, 
                        const sf::Vector2f& wind,
                        float deltaTime) {
    for (size_t i = start; i < end; i++) {
        particles[i].apply_force(sf::Vector2f(0, GRAVITY));
        particles[i].apply_force(wind);
        particles[i].update(deltaTime);
        particles[i].constrain_to_bounds(WIDTH, HEIGHT);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cloth Simulation");
    window.setFramerateLimit(60);
    
    // Initialize clock for time-based effects
    sf::Clock clock;
    
    // Initialize cloth particles
    std::vector<Particle> particles;
    std::vector<Constraint> constraints;
    
    // Create particle grid
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
                constraints.emplace_back(&particles[row * COL + col], 
                                      &particles[row * COL + col + 1]);
            }
            if (row < ROW - 1) {
                // Vertical constraint
                constraints.emplace_back(&particles[row * COL + col], 
                                      &particles[(row + 1) * COL + col]);
            }
        }
    }

    // Initialize spatial hash
    SpatialHash spatialHash(CELL_SIZE);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle mouse clicks
            InputHandler::handle_mouse_click(event, particles, constraints);
        }

        // Get current time and calculate wind
        float current_time = clock.getElapsedTime().asSeconds();
        sf::Vector2f wind = calculate_wind(current_time);

        // Update spatial hash
        spatialHash.update(particles);

        // Parallel particle updates
        std::vector<std::thread> threads;
        const size_t particlesPerThread = particles.size() / std::max<size_t>(1, THREAD_COUNT);

        for (unsigned int i = 0; i < THREAD_COUNT && i * particlesPerThread < particles.size(); i++) {
            size_t start = i * particlesPerThread;
            size_t end = (i == THREAD_COUNT - 1) ? particles.size() 
                                                : (i + 1) * particlesPerThread;
            
            if (start < end) {
                threads.emplace_back(updateParticleRange, 
                                std::ref(particles), 
                                start, end, 
                                std::ref(wind), 
                                TIME_STEP);
            }
        }
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        // Constraint satisfaction with spatial optimization
        for (size_t i = 0; i < 5; i++) {
            for (auto& constraint : constraints) {
                if (constraint.active) {
                    // Simply call satisfy without the spatial optimization for now
                    constraint.satisfy();
                }
            }
        }

        // Clear window with dark background
        window.clear(sf::Color(30, 30, 30));

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
            circle.setPosition(particle.position.x - 2.0f, 
                             particle.position.y - 2.0f);
            window.draw(circle);
        }

        window.display();
    }
    return 0;
}