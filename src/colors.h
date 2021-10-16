#ifndef colors
#define colors

#include "enums.h"

static Color BLACK  {   0,   0,   0 };

static Color RED    { 255,   0,   0 };
static Color GREEN  {   0, 255,   0 };
static Color BLUE   {   0,   0, 255 };

static Color YELLOW  =   RED | GREEN;
static Color CYAN    = GREEN | BLUE;
static Color MAGENTA =   RED | BLUE;
static Color ORANGE { 255, 165,   0 };

static Color WHITE = RED | GREEN | BLUE;

#endif