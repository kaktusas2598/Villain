#include "Grid.hpp"

Grid::Grid(int w, int h, int size) : width(w), height(h), cellSize(size) {
    numXCells = ceil((float)width / cellSize);
    numYCells = ceil((float)height / cellSize);

    // Allocate memory for cells
    const int BALLS_TO_RESERVE = 20;
    cells.resize(numXCells * numYCells);
    for (int i = 0; i < cells.size(); i++) {
        cells[i].balls.reserve(BALLS_TO_RESERVE);
    }

}

Grid::~Grid() {}

void Grid::resize(int w, int h) {
    width = w;
    height = h;

    numXCells = ceil((float)width / cellSize);
    numYCells = ceil((float)height / cellSize);

    //cells.clear();
    // Allocate memory for cells
    cells.resize(numXCells * numYCells);
}

void Grid::addBall(Ball* ball) {
    Cell* cell = getCell(ball->position);
    // NOTE: kept duplicated code here for performance to avoid too many function calls
    cell->balls.push_back(ball);
    ball->ownerCell = cell;
    ball->cellVectorIndex = cell->balls.size() - 1;
}

void Grid::addBall(Ball* ball, Cell* cell) {
    cell->balls.push_back(ball);
    ball->ownerCell = cell;
    ball->cellVectorIndex = cell->balls.size() - 1;
}

void Grid::removeBallFromCell(Ball* ball) {
    std::vector<Ball*>& balls = ball->ownerCell->balls;
    balls[ball->cellVectorIndex] = balls.back();
    balls.pop_back();
    // Update vector index
    if ((size_t)ball->cellVectorIndex < balls.size()) {
        balls[ball->cellVectorIndex]->cellVectorIndex = ball->cellVectorIndex;
    }
    ball->cellVectorIndex = -1;
    ball->ownerCell = nullptr;
}

Cell* Grid::getCell(int x, int y) {
    if (x < 0) x = 0;
    if (x >= numXCells) x = numXCells - 1;
    if (y < 0) y = 0;
    if (y >= numYCells) y = numYCells - 1;

    return &cells[y * numXCells + x];
}

Cell* Grid::getCell(const glm::vec2& position) {
    int celX = (int)(position.x / cellSize);
    int celY = (int)(position.y / cellSize);

    return getCell(celX, celY);
}
