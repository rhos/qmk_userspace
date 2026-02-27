#pragma once
 
#include "layers.h"

enum layers {
#define LAYER_X(LAYER) U_##LAYER,
LAYER_LIST
#undef LAYER_X
};

#define U_MACRO_VA_ARGS(macro, ...) macro(__VA_ARGS__)