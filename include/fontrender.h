#ifndef FONTRENDER_H
#define FONTRENDER_H

#include "js.h"

i32 renderChar(char c, i32 x, i32 y, i32 size);
i32 renderNumber(char n, i32 x, i32 y, i32 size);
i32 renderString(char *str, i32 x, i32 y, i32 size);

#endif
