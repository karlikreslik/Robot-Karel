#include "raylib.h"
#include <stdbool.h>

#define GRID_SIZE 10
#define CELL_SIZE 60
#define MAX_BEEPERS 10

typedef struct {
    int x, y;
    int direction; // 0=North, 1=East, 2=South, 3=West
    int beepers;
} Robot;

typedef struct {
    int x, y;
} Beeper;

typedef struct {
    int x1, y1, x2, y2; // Wall between (x1,y1) and (x2,y2)
} Wall;

// Global state
Robot karel = {0, 0, 1, 0}; // Start at (0,0), facing East
Beeper beepers[MAX_BEEPERS] = {{2, 2}, {3, 3}}; // Beepers at positions
int beeperCount = 2;
Wall walls[] = {{1, 0, 1, 1}, {2, 2, 3, 2}}; // Fixed wall initialization
int wallCount = 2;

// Wall collision check
bool isWall(int x, int y, int dir) {
    for (int i = 0; i < wallCount; i++) {
        Wall w = walls[i];
        if ((w.x1 == x && w.y1 == y && w.x2 == x+1 && w.y2 == y && dir == 1) || // East wall
            (w.x1 == x && w.y1 == y && w.x2 == x && w.y2 == y+1 && dir == 2) || // South wall
            (w.x1 == x-1 && w.y1 == y && w.x2 == x && w.y2 == y && dir == 3) || // West wall
            (w.x1 == x && w.y1 == y-1 && w.x2 == x && w.y2 == y && dir == 0)) { // North wall
            return true;
        }
    }
    return false;
}

// Check for beeper at position
int getBeeperAt(int x, int y) {
    for (int i = 0; i < beeperCount; i++) {
        if (beepers[i].x == x && beepers[i].y == y) {
            return i;
        }
    }
    return -1;
}

// Karel's commands
void move() {
    int newX = karel.x, newY = karel.y;
    switch (karel.direction) {
        case 0: newY--; break; // North
        case 1: newX++; break; // East
        case 2: newY++; break; // South
        case 3: newX--; break; // West
    }

    if (newX >= 0 && newX < GRID_SIZE && newY >= 0 && newY < GRID_SIZE && !isWall(karel.x, karel.y, karel.direction)) {
        karel.x = newX;
        karel.y = newY;
    }
}

void turnLeft() { karel.direction = (karel.direction - 1 + 4) % 4; }
void turnRight() { karel.direction = (karel.direction + 1) % 4; }

void pickBeeper() {
    int beeperIdx = getBeeperAt(karel.x, karel.y);
    if (beeperIdx != -1) {
        karel.beepers++;
        // Remove beeper from world
        for (int i = beeperIdx; i < beeperCount - 1; i++) {
            beepers[i] = beepers[i + 1];
        }
        beeperCount--;
    }
}

void putBeeper() {
    if (karel.beepers > 0 && getBeeperAt(karel.x, karel.y) == -1) {
        beepers[beeperCount++] = (Beeper){karel.x, karel.y};
        karel.beepers--;
    }
}

// Draw robot's direction indicator
void drawRobotDirection(int x, int y, int dir) {
    Vector2 center = {x * CELL_SIZE + CELL_SIZE/2, y * CELL_SIZE + CELL_SIZE/2};
    Vector2 end = center;
    switch (dir) {
        case 0: end.y -= CELL_SIZE/3; break; // North
        case 1: end.x += CELL_SIZE/3; break; // East
        case 2: end.y += CELL_SIZE/3; break; // South
        case 3: end.x -= CELL_SIZE/3; break; // West
    }
    DrawLineV(center, end, BLACK);
}

int main() {
    InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE + 50, "Karel the Robot (C + Raylib)");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Keyboard controls
        if (IsKeyPressed(KEY_W)) move();
        if (IsKeyPressed(KEY_A)) turnLeft();
        if (IsKeyPressed(KEY_D)) turnRight();
        if (IsKeyPressed(KEY_P)) pickBeeper();
        if (IsKeyPressed(KEY_T)) putBeeper();

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw grid
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
            }
        }

        // Draw walls
        for (int i = 0; i < wallCount; i++) {
            Wall w = walls[i];
            if (w.x1 == w.x2) { // Vertical wall
                DrawLine(w.x1 * CELL_SIZE, w.y1 * CELL_SIZE, w.x2 * CELL_SIZE, w.y2 * CELL_SIZE, BLACK);
            } else { // Horizontal wall
                DrawLine(w.x1 * CELL_SIZE, w.y1 * CELL_SIZE, w.x2 * CELL_SIZE, w.y2 * CELL_SIZE, BLACK);
            }
        }

        // Draw beepers
        for (int i = 0; i < beeperCount; i++) {
            DrawCircle(beepers[i].x * CELL_SIZE + CELL_SIZE/2, beepers[i].y * CELL_SIZE + CELL_SIZE/2, 10, GREEN);
        }

        // Draw Karel
        DrawRectangle(karel.x * CELL_SIZE, karel.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
        drawRobotDirection(karel.x, karel.y, karel.direction);

        // Status bar
        DrawRectangle(0, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, 50, LIGHTGRAY);
        DrawText(TextFormat("Position: (%d, %d) | Beepers: %d | Facing: %s", 
            karel.x, karel.y, karel.beepers,
            karel.direction == 0 ? "North" : 
            karel.direction == 1 ? "East" : 
            karel.direction == 2 ? "South" : "West"), 
            10, GRID_SIZE * CELL_SIZE + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
