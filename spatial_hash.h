#ifndef SPATIAL_HASH_H
#define SPATIAL_HASH_H

#include <vector>
#include <unordered_map>
#include <memory>
#include "particle.h"

class SpatialHash {
private:
    float cellSize;
    std::unordered_map<size_t, std::vector<std::reference_wrapper<Particle>>> grid;

    size_t hashFunction(int x, int y) const {
        const size_t p1 = 73856093;
        const size_t p2 = 19349669;
        return (size_t)(x * p1 ^ y * p2);
    }

public:
    SpatialHash(float size) : cellSize(size) {}

    void update(std::vector<Particle>& particles) {
        grid.clear();
        for (auto& particle : particles) {
            int x = static_cast<int>(particle.position.x / cellSize);
            int y = static_cast<int>(particle.position.y / cellSize);
            size_t hash = hashFunction(x, y);
            grid[hash].push_back(std::ref(particle));
        }
    }

    std::vector<std::reference_wrapper<Particle>> getNearbyParticles(const sf::Vector2f& position) {
        std::vector<std::reference_wrapper<Particle>> nearby;
        int x = static_cast<int>(position.x / cellSize);
        int y = static_cast<int>(position.y / cellSize);

        // Check neighboring cells
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                size_t hash = hashFunction(x + i, y + j);
                auto it = grid.find(hash);
                if (it != grid.end()) {
                    nearby.insert(nearby.end(), it->second.begin(), it->second.end());
                }
            }
        }
        return nearby;
    }
};
#endif