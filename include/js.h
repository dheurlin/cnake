#ifndef JS_H
#define JS_H

typedef signed int i32;

void print(i32);
void makeWindow(i32 width, i32 height);
void clearWindow();
void drawRect(i32 x, i32 y, i32 width, i32 height, i32 r, i32 g, i32 b);
i32 getHeldKey();
float rand();
i32 roundFloat(float);

#endif
