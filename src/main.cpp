
#include "../include/Quadtree.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main() {
    constexpr int ENTITY_COUNT = 2000;
    constexpr float WORLD_SIZE = 1000.0f;

    std::cout << "=== Spatial Partitioning Quadtree Benchmark ===\n";
    std::cout << "Generating " << ENTITY_COUNT << " entities in a " << WORLD_SIZE << "x" << WORLD_SIZE << " world...\n\n";

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> dist(0.0f, WORLD_SIZE);

    std::vector<Entity> allEntities;
    allEntities.reserve(ENTITY_COUNT);

    for (int i = 0; i < ENTITY_COUNT; ++i) {
        allEntities.push_back(Entity{i, Point{dist(rng), dist(rng)}});
    }

    AABB worldBoundary{WORLD_SIZE / 2.0f, WORLD_SIZE / 2.0f, WORLD_SIZE / 2.0f, WORLD_SIZE / 2.0f};
    Quadtree tree(worldBoundary);

    for (const auto& entity : allEntities) {
        tree.insert(entity);
    }

    AABB queryArea{500.0f, 500.0f, 50.0f, 50.0f};

    // 1. Brute-Force Spatial Query
    int bruteForceChecks = 0;
    std::vector<Entity> bruteForceResults;
    auto startBF = std::chrono::high_resolution_clock::now();

    for (const auto& entity : allEntities) {
        bruteForceChecks++;
        if (queryArea.contains(entity.position)) {
            bruteForceResults.push_back(entity);
        }
    }

    auto endBF = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> durationBF = endBF - startBF;

    // 2. Quadtree Optimized Spatial Query
    int quadtreeChecks = 0;
    std::vector<Entity> quadtreeResults;
    auto startQT = std::chrono::high_resolution_clock::now();

    tree.queryRange(queryArea, quadtreeResults, quadtreeChecks);

    auto endQT = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> durationQT = endQT - startQT;

    // Results
    std::cout << "[Brute Force Query]\n";
    std::cout << "  - Entities Found: " << bruteForceResults.size() << "\n";
    std::cout << "  - Checks Performed: " << bruteForceChecks << "\n";
    std::cout << "  - Time Taken: " << durationBF.count() << " ms\n\n";

    std::cout << "[Quadtree Optimized Query]\n";
    std::cout << "  - Entities Found: " << quadtreeResults.size() << "\n";
    std::cout << "  - Checks Performed: " << quadtreeChecks << "\n";
    std::cout << "  - Time Taken: " << durationQT.count() << " ms\n\n";

    float efficiencyFactor = static_cast<float>(bruteForceChecks) / static_cast<float>(quadtreeChecks);
    std::cout << "Quadtree checked " << efficiencyFactor << "x fewer entities!\n";

    return 0;
}
