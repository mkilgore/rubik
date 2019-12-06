#ifndef INCLUDE_RUBIK_H
#define INCLUDE_RUBIK_H

#define RUBIK_SIZE 3

enum rubik_turn {
    RUBIK_TURN_CW,
    RUBIK_TURN_CCW,
    RUBIK_TURN_DOUBLE,
};

enum rubik_side {
    RUBIK_SIDE_RED = 0,
    RUBIK_SIDE_ORANGE = 1,
    RUBIK_SIDE_BLUE = 2,
    RUBIK_SIDE_GREEN = 3,
    RUBIK_SIDE_WHITE = 4,
    RUBIK_SIDE_YELLOW = 5,
};

static inline char rubik_side_char(enum rubik_side side) {
    switch (side) {
    case RUBIK_SIDE_RED:
        return 'R';

    case RUBIK_SIDE_ORANGE:
        return 'O';

    case RUBIK_SIDE_BLUE:
        return 'B';

    case RUBIK_SIDE_GREEN:
        return 'G';

    case RUBIK_SIDE_WHITE:
        return 'W';

    case RUBIK_SIDE_YELLOW:
        return 'Y';
    }
    return ' ';
}

static inline int rubik_side_from_char(char c, enum rubik_side *side) {
    switch (c) {
    case 'r':
    case 'R':
        *side = RUBIK_SIDE_RED;
        return 0;

    case 'o':
    case 'O':
        *side = RUBIK_SIDE_ORANGE;
        return 0;

    case 'b':
    case 'B':
        *side = RUBIK_SIDE_BLUE;
        return 0;

    case 'g':
    case 'G':
        *side = RUBIK_SIDE_GREEN;
        return 0;

    case 'w':
    case 'W':
        *side = RUBIK_SIDE_WHITE;
        return 0;

    case 'y':
    case 'Y':
        *side = RUBIK_SIDE_YELLOW;
        return 0;
    }

    return 1;
}

enum rubik_sticker {
    RUBIK_RED,
    RUBIK_ORANGE,
    RUBIK_BLUE,
    RUBIK_GREEN,
    RUBIK_WHITE,
    RUBIK_YELLOW,
};

static inline char rubik_sticker_char(enum rubik_sticker sticker) {
    switch (sticker) {
    case RUBIK_RED:
        return 'R';

    case RUBIK_ORANGE:
        return 'O';

    case RUBIK_BLUE:
        return 'B';

    case RUBIK_GREEN:
        return 'G';

    case RUBIK_WHITE:
        return 'W';

    case RUBIK_YELLOW:
        return 'Y';
    }
    return ' ';
}

struct rubik_cube {
    enum rubik_sticker sides[6][RUBIK_SIZE * RUBIK_SIZE];
};

void rubik_cube_init(struct rubik_cube *);
void rubik_cube_clear(struct rubik_cube *);

void rubik_cube_turn(struct rubik_cube *, enum rubik_side, enum rubik_turn);

#endif
