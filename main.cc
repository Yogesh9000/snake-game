#include <deque>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 22;
int cellCount = 25;
int offset = 22;
double lastUpdateTime = 0;

bool elementInDeque(Vector2 v, std::deque<Vector2> snakeBody) {
  for (size_t i = 0; i < snakeBody.size(); i++) {
    if (Vector2Equals(v, snakeBody[i]))
      return true;
  }
  return false;
}

class Food {
public:
  Vector2 pos;
  Texture2D texture;

  Food(std::deque<Vector2> snakeBody) {
    pos = generateRandomPos(snakeBody);
    Image image = LoadImage("food.png");
    ImageResize(&image, cellSize, cellSize);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
  }

  ~Food() { UnloadTexture(texture); }

  void Draw() {
    DrawTexture(texture, offset - 5 + (int)pos.x * cellSize,
                offset - 5 + (int)pos.y * cellSize, WHITE);
  }

  Vector2 generateRandomPos(std::deque<Vector2> snakeBody) {
    float x = (float)GetRandomValue(0, cellCount - 1);
    float y = (float)GetRandomValue(0, cellCount - 1);
    Vector2 v = {x, y};
    while (elementInDeque(v, snakeBody)) {
      x = (float)GetRandomValue(0, cellCount - 1);
      y = (float)GetRandomValue(0, cellCount - 1);
      v = {x, y};
    }
    return v;
  }
};

class Snake {
public:
  std::deque<Vector2> body = {Vector2{0, 10}};
  Vector2 direction = {1, 0};
  bool running = true;

  void Draw() {
    for (size_t i = 0; i < body.size(); i++) {
      Rectangle rect = {offset - 5 + body[i].x * cellSize,
                        offset - 5 + body[i].y * cellSize, (float)cellSize,
                        (float)cellSize};
      DrawRectangleRounded(rect, 0.5, 6, darkGreen);
    }
  }

  void Update() {
    body.pop_back();
    body.push_front(Vector2Add(body[0], direction));
  }
};

class Game {
public:
  Snake snake;
  Food food;
  bool running;
  int score = 0;

  Game() : food(snake.body), running(true) {}

  void Update() {
    snake.Update();
    checkCollisionWithEdges();
    checkCollisionWithSelf();
    checkCollisionWithFood();
  }

  void Draw() {
    snake.Draw();
    food.Draw();
    DrawRectangleLinesEx({(float)offset - 5, (float)offset - 5,
                          (float)cellSize * cellCount,
                          (float)cellSize * cellCount},
                         5, darkGreen);
  }

  void checkCollisionWithFood() {
    if (Vector2Equals(snake.body[0], food.pos)) {
      Vector2 n = snake.body.front();
      n = Vector2Add(n, snake.direction);
      snake.body.push_front(n);
      food.pos = food.generateRandomPos(snake.body);
      score++;
    }
  }

  void checkCollisionWithEdges() {
    if (snake.body[0].x >= (float)cellCount || snake.body[0].x <= -1)
      GameOver();
    if (snake.body[0].y >= (float)cellCount || snake.body[0].y <= -1)
      GameOver();
  }

  void checkCollisionWithSelf() {
    for (size_t i = 1; i < snake.body.size(); i++) {
      if (Vector2Equals(snake.body[0], snake.body[i]))
        GameOver();
    }
  }

  void GameOver() {
    snake.body = {Vector2{0, 10}};
    food.pos = food.generateRandomPos(snake.body);
    running = false;
    score = 0;
  }
};

bool eventTriggered(double interval) {
  double currentTime = GetTime();
  if (currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    return true;
  }
  return false;
}

int main() {
  InitWindow(offset + 10 + cellSize * cellCount,
             offset + 10 + cellSize * cellCount, "Snake-Game");
  SetTargetFPS(60);
  Game game;
  while (WindowShouldClose() == false) {
    if (eventTriggered(0.2) && game.running) {
      game.Update();
    }
    if (IsKeyPressed(KEY_UP) && (int)game.snake.direction.y != 1) {
      game.snake.direction = {0, -1};
      game.running = true;
    }
    if (IsKeyPressed(KEY_DOWN) && (int)game.snake.direction.y != -1) {
      game.snake.direction = {0, 1};
      game.running = true;
    }
    if (IsKeyPressed(KEY_RIGHT) && (int)game.snake.direction.x != -1) {
      game.snake.direction = {1, 0};
      game.running = true;
    }
    if (IsKeyPressed(KEY_LEFT) && (int)game.snake.direction.x != 1) {
      game.snake.direction = {-1, 0};
      game.running = true;
    }
    BeginDrawing();
    ClearBackground(green);
    DrawText("Snake", offset, 0, 20, darkGreen);
    DrawText(TextFormat("%i", game.score), offset, offset - 6 + 550, 20,
             darkGreen);
    game.Draw();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
