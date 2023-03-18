#ifndef __GRID__
#define __GRID__

#include "Ball.hpp"
#include <vector>

struct Cell {
    //NOTE: once ECS is implemented, instead of Ball, this could be Entity
    std::vector<Ball*> balls;
};

// Spacial Partitioning model
// NOTE: this could go to engine at some point
class Grid {
    friend class BallController;
    public:
        /*
         * @brief Spatial Grid constructor
         *
         * @param w Screen width
         * @param h Screen height
         * @param size Cell size
         */
        Grid(int w, int h, int size);
        ~Grid();

        void resize(int w, int h);
        void addBall(Ball* ball);
        void addBall(Ball* ball, Cell* cell);
        void removeBallFromCell(Ball* ball);
        Cell* getCell(int x, int y);
        Cell* getCell(const glm::vec2& position);

    private:
        // Use 1D vector for cache-friendliness
        std::vector<Cell> cells;
        int cellSize;
        int width, height;
        int numXCells, numYCells;
};

#endif // __GRID__
