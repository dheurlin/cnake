typedef signed int i32;

void print(i32);
void makeWindow(i32 width, i32 height);
void clearWindow();
void drawRect(i32 x, i32 y, i32 width, i32 height, i32 r, i32 g, i32 b);
i32 getHeldKey();
float rand();
i32 roundFloat(float);

// Constants ------------------------------------------------------------------

#define bool short
#define false 0
#define true 1

#define LEFT_ARR 37
#define UP_ARR 38
#define RIGHT_ARR 39
#define DOWN_ARR 40

#define CELL_SIDE 70
#define CELL_GAP 10

#define GRID_COLS 18
#define GRID_ROWS 12

#define SCREEN_WIDTH (GRID_COLS * CELL_SIDE)
#define SCREEN_HEIGHT (GRID_ROWS * CELL_SIDE)

#define PAUSE_THRESHOLD 10
#define BASE_SPEED 10
#define UPDATE_INTERVAL 10

#define SNAKE_COLOR 46, 84, 37
#define APPLE_COLOR 219, 53, 53

// Data types -----------------------------------------------------------------

typedef struct Point {
  i32 x;
  i32 y;
} Point;

typedef Point Vec2;

typedef struct Player {
  Point pos;
  Vec2 speed;
} Player;

typedef struct Tail {
  Point prevPositions[GRID_COLS * GRID_ROWS];
  i32 length;
} Tail;

// Game state -----------------------------------------------------------------

bool paused = false;
bool dead = false;
short framesSincePauseChanged = 0;
short framesSinceUpdated = 0;

Player player = {
  .pos = {
    .x = (GRID_COLS / 2) * CELL_SIDE,
    .y = (GRID_ROWS / 2) * CELL_SIDE,
  },
  .speed = {
    .x = 1,
    .y = 0,
  }
};

// Keeps track of the last direction we moved in. This is different from the player
// speed, since this variable is updated every frame, and lastDirection is only
// updated when the snake moves.
Vec2 lastDirection = {
  .x = 0,
  .y = 0,
};

Tail tail = {
  .length = 0
};

Point apple = {
  .x = 7 * CELL_SIDE,
  .y = 7 * CELL_SIDE,
};

// Game logic -----------------------------------------------------------------

void drawSnakeCell(i32 x, i32 y) {
  drawRect(x + CELL_GAP, y + CELL_GAP, CELL_SIDE - CELL_GAP, CELL_SIDE - CELL_GAP, SNAKE_COLOR);
}

void drawApple(i32 x, i32 y) {
  drawRect(x + CELL_GAP, y + CELL_GAP, CELL_SIDE - CELL_GAP, CELL_SIDE - CELL_GAP, APPLE_COLOR);
}

void drawTail() {
  for (i32 i = 0; i < tail.length; i++) {
    drawSnakeCell(tail.prevPositions[i].x, tail.prevPositions[i].y);
  }
}

void updateTail(i32 newX, i32 newY) {
  for (i32 i = tail.length; i > 0; i--) {
    tail.prevPositions[i] = tail.prevPositions[i-1];
  }
  Point newPoint = { .x = newX, .y = newY  };
  tail.prevPositions[0] = newPoint;
}

void extendTail() {
  tail.prevPositions[tail.length] = tail.prevPositions[tail.length - 1];
  tail.length++;
}

void spawnApple() {
  i32 newX, newY;
  // Look for places to spawn until we find one that doesn't overlap
  // with the snake
  while (true) {
    newX = roundFloat(rand() * (GRID_COLS - 1)) * CELL_SIDE;
    newY = roundFloat(rand() * (GRID_ROWS - 1)) * CELL_SIDE;

    // Check if we have spawned on the player
    if (newX == player.pos.x && newY == player.pos.y) {
      goto cont;
    }
    // Check if we have spawned on the tail
    for (i32 i = 0; i < tail.length; i++) {
      Point p = tail.prevPositions[i];
      if (newX == p.x && newY == p.y) {
        goto cont;
      }
    }

    break;
    // Used to continue from inner loop
    cont:;
  }
  apple.x = newX;
  apple.y = newY;
}

void setIsDead() {
  for (i32 i = 0; i < tail.length; i++) {
    Point p = tail.prevPositions[i];
    if (player.pos.x == p.x && player.pos.y == p.y) {
      dead = true;
      return;
    }
  }
}

void init() {
  makeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
  drawSnakeCell(player.pos.x, player.pos.y);
  spawnApple();
  drawApple(apple.x, apple.y);

  for (int i = 0; i < 10; i++) {
    extendTail();
  }

  drawTail();
}

void update() {

  if (dead) {
    drawRect(player.pos.x + CELL_GAP, player.pos.y + CELL_GAP, CELL_SIDE - CELL_GAP, CELL_SIDE - CELL_GAP, 0, 0, 0);
    return;
  }

  // Get input    ----------------------------------------------

  i32 key = getHeldKey();

  if (key == 'P' && framesSincePauseChanged >= PAUSE_THRESHOLD) {
    paused = !paused;
    framesSincePauseChanged = 0;
  }
  framesSincePauseChanged++;

  // Update state ----------------------------------------------

  if (paused) {
    return;
  }

  switch (key) {
    case 'W':
    case UP_ARR:
      if (lastDirection.y != 1) {
        player.speed.y = -1;
        player.speed.x = 0;
      }
      break;
    case 'S':
    case DOWN_ARR:
      if (lastDirection.y != -1) {
        player.speed.y = 1;
        player.speed.x = 0;
      }
      break;
    case 'A':
    case LEFT_ARR:
      if (lastDirection.x != 1) {
        player.speed.y = 0;
        player.speed.x = -1;
      }
      break;
    case 'D':
    case RIGHT_ARR:
      if (lastDirection.x != -1) {
        player.speed.y = 0;
        player.speed.x = 1;
      }
      break;
  }

  // We only want to move the snake every X frames (to get the 'tick' effect),
  // but we still want to listen for input every frame
  if (!(framesSinceUpdated++ >= UPDATE_INTERVAL)) {
    return;
  }

  lastDirection.x = player.speed.x;
  lastDirection.y = player.speed.y;

  i32 oldX = player.pos.x;
  i32 oldY = player.pos.y;
  i32 newX = oldX + (player.speed.x * CELL_SIDE);
  i32 newY = oldY + (player.speed.y * CELL_SIDE);

  // Wrap around if negative coordinates
  if (newY <= 0) {
    newY = SCREEN_HEIGHT + newY;
  }
  if (newX <= 0) {
    newX = SCREEN_WIDTH + newX;
  }

  newX = newX % SCREEN_WIDTH;
  newY = newY % SCREEN_HEIGHT;

  player.pos.x = newX;
  player.pos.y = newY;

  updateTail(oldX, oldY);

  // Touching apple?
  if (newX == apple.x && newY == apple.y) {
    extendTail();
    spawnApple();
  }

  setIsDead();

  // Render ----------------------------------------------------

  clearWindow();
  drawApple(apple.x, apple.y);
  drawSnakeCell(player.pos.x, player.pos.y);
  drawTail();
  framesSinceUpdated = 0;

}
