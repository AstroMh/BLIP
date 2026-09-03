// RoboEyesWrapper.h
#ifndef ROBOTEYES_WRAPPER_H
#define ROBOTEYES_WRAPPER_H

// Include the library
#include <FluxGarage_RoboEyes.h>

// Save the macro values as constants
static const int ROBOTEYES_DEFAULT = 0;
static const int ROBOTEYES_N = 1;
static const int ROBOTEYES_NE = 2;
static const int ROBOTEYES_E = 3;
static const int ROBOTEYES_SE = 4;
static const int ROBOTEYES_S = 5;
static const int ROBOTEYES_SW = 6;
static const int ROBOTEYES_W = 7;
static const int ROBOTEYES_NW = 8;

// Undefine ALL macros from the library
#undef DEFAULT
#undef N
#undef NE
#undef E
#undef SE
#undef S
#undef SW
#undef W
#undef NW

// We keep DEFAULT, HAPPY, ANGRY, TIRED as they're needed for moods
// But if they conflict, we'll use numeric values for moods too

#endif