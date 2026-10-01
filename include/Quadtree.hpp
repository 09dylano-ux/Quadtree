
#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <memory>
#include <cmath>

struct Point {
    float x;
    float y;
};

struct AABB {
    float x; // Center X
    float y; // Center Y
    float halfWidth;
    float halfHeight;

    [[nodiscard]] bool contains(const Point& p) const noexcept {
        return (p.x >= x - halfWidth && p.x <= x + halfWidth &&
                p.y >= y - halfHeight && p.y <= y + halfHeight);
    }

    [[nodiscard]] bool intersects(const AABB& other) const noexcept {
        return (std::abs(x - other.x) <= (halfWidth + other.halfWidth) &&
                std::abs(y - other.y) <= (halfHeight + other.halfHeight));
    }
};

struct Entity {
    int id;
    Point position;
};

class Quadtree {
private:
    static constexpr std::size_t CAPACITY = 4;
    static constexpr int MAX_DEPTH = 6;

    AABB m_boundary;
    int m_depth;
    std::vector<Entity> m_entities;
    bool m_isDivided = false;

    std::unique_ptr<Quadtree> m_northWest;
    std::unique_ptr<Quadtree> m_northEast;
    std::unique_ptr<Quadtree> m_southWest;
    std::unique_ptr<Quadtree> m_southEast;

    void subdivide() {
        float hW = m_boundary.halfWidth / 2.0f;
        float hH = m_boundary.halfHeight / 2.0f;

        m_northWest = std::make_unique<Quadtree>(AABB{m_boundary.x - hW, m_boundary.y - hH, hW, hH}, m_depth + 1);
        m_northEast = std::make_unique<Quadtree>(AABB{m_boundary.x + hW, m_boundary.y - hH, hW, hH}, m_depth + 1);
        m_southWest = std::make_unique<Quadtree>(AABB{m_boundary.x - hW, m_boundary.y + hH, hW, hH}, m_depth + 1);
        m_southEast = std::make_unique<Quadtree>(AABB{m_boundary.x + hW, m_boundary.y + hH, hW, hH}, m_depth + 1);

        m_isDivided = true;
    }

public:
    explicit Quadtree(AABB boundary, int depth = 0)
        : m_boundary(boundary), m_depth(depth) {
        m_entities.reserve(CAPACITY);
    }

    bool insert(const Entity& entity) {
        if (!m_boundary.contains(entity.position)) {
            return false;
        }

        if (m_entities.size() < CAPACITY || m_depth >= MAX_DEPTH) {
            m_entities.push_back(entity);
            return true;
        }

        if (!m_isDivided) {
            subdivide();
        }

        return (m_northWest->insert(entity) ||
                m_northEast->insert(entity) ||
                m_southWest->insert(entity) ||
                m_southEast->insert(entity));
    }

    void queryRange(const AABB& range, std::vector<Entity>& found, int& checksPerformed) const {
        if (!m_boundary.intersects(range)) {
            return;
        }

        for (const auto& entity : m_entities) {
            checksPerformed++;
            if (range.contains(entity.position)) {
                found.push_back(entity);
            }
        }

        if (m_isDivided) {
            m_northWest->queryRange(range, found, checksPerformed);
            m_northEast->queryRange(range, found, checksPerformed);
            m_southWest->queryRange(range, found, checksPerformed);
            m_southEast->queryRange(range, found, checksPerformed);
        }
    }

    void clear() {
        m_entities.clear();
        if (m_isDivided) {
            m_northWest.reset();
            m_northEast.reset();
            m_southWest.reset();
            m_southEast.reset();
            m_isDivided = false;
        }
    }
};

#endif // QUADTREE_HPP
