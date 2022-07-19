#include "fontrender.h"
#include "font.h"
#include "js.h"

typedef signed int i32;
#define CHAR_SPACING 1

i32 renderChar(char c, i32 x, i32 y, i32 size) {
  char *bitmap = font8x8_basic[c];
  int set;
  int mask;
  for (int col=0; col < 8; col++) {
    for (int row=0; row < 8; row++) {
      set = bitmap[row] & 1 << col;
      if (set) {
        drawRect(x + col * size, y + row * size, size, size, 0, 0, 0);
      }
    }
  }
  return x + size * (8 + CHAR_SPACING);
}

i32 renderString(char *n, i32 x, i32 y, i32 size) {
  i32 nextX = x;
  while (*n) {
    nextX = renderChar(*n++, nextX, y, size);
  }

  return nextX;
}

i32 renderNumber(char n, i32 x, i32 y, i32 size) {
  // Extract the digits. Will be layed out backwards
  char digits[4];
  for (int i = 0; i < 4; i++) {
    digits[i] = n % 10;
    n /= 10;
  }

  // Now draw them, iterating through the digits array backwards
  i32 digit = -1;
  i32 nextX = x;
  for (int i = 0; i < 4; i++) {

    // the line below skips leading zeros
    if (digits[3 - i] == 0 && digit == -1 && i != 3) continue;
    digit++;

    nextX = renderChar(
      digits[3 - i] + 0x30,
      nextX,
      y,
      size
    );
  }
  return nextX;
}
