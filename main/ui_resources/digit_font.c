#include "digit_font.h"

const uint8_t DIGIT_0[5][3] = {
    {1, 1, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_1[5][3] = {
    {0, 1, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 1},
};

const uint8_t DIGIT_2[5][3] = {
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
};

const uint8_t DIGIT_3[5][3] = {
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_4[5][3] = {
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1},
};

const uint8_t DIGIT_5[5][3] = {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_6[5][3] = {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_7[5][3] = {
    {1, 1, 1},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
};

const uint8_t DIGIT_8[5][3] = {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_9[5][3] = {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
};

const uint8_t DIGIT_COLON[5][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
};

const uint8_t DIGIT_NONE[5][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
};

const uint8_t (*DIGITS[12])[3] = {
    DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
    DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
    DIGIT_COLON, DIGIT_NONE};