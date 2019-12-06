
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "rubik.h"

void rubik_cube_init(struct rubik_cube *cube)
{
    int i = 0, s;

    for (s = 0; s < 6; s++)
        for (i = 0; i < 9; i++)
            cube->sides[s][i] = s;
}

void rubik_cube_clear(struct rubik_cube *cube)
{

}

enum rubik_side_orientation {
    ORIENT_TOP,
    ORIENT_LEFT,
    ORIENT_RIGHT,
    ORIENT_BOTTOM,
};

/*
 * Describes what three stickers on one side are adjacent to another side
 */
struct rubik_side_adj {
    enum rubik_side side;
    enum rubik_side_orientation ori;
};

struct rubik_side_def {
    struct rubik_side_adj adj[4];
};

struct rubik_side_adj_val {
    enum rubik_sticker stickers[RUBIK_SIZE];
};

#define RUBIK_SIDE_ADJ(a, o) \
    { .side = a, .ori = o }

#define RUBIK_SIDE_DEF(a1, o1, a2, o2, a3, o3, a4, o4)             \
    { .adj = {                                                     \
                 RUBIK_SIDE_ADJ(RUBIK_SIDE_ ## a1, ORIENT_ ## o1), \
                 RUBIK_SIDE_ADJ(RUBIK_SIDE_ ## a2, ORIENT_ ## o2), \
                 RUBIK_SIDE_ADJ(RUBIK_SIDE_ ## a3, ORIENT_ ## o3), \
                 RUBIK_SIDE_ADJ(RUBIK_SIDE_ ## a4, ORIENT_ ## o4)  \
             }                                                     \
    }

/*
 * This table records what parts of other sides are adjacent to any paticular
 * side, listed in clockwise order.
 *
 * IE. The red side touches the blue, yello, white, and greed sides, however a
 * single set of 3 stickers, represented by the orientation. The parts of the
 * sides that it touches are recorded, and the sides themselves are listed in
 * clockwise order. Thus, if you go down the list of adjecent sides and copy
 * one into the next, you'll perform a single clockwise rotation of all the
 * adjacent stickers.
 *
 * The basis for the table below is:
 *
 *    Top:    ORANGE
 *    Bottom: RED
 *    Back:   YELLOW
 *    Front:  WHITE
 *    Right:  BLUE
 *    Left:   GREEN
 */
static const struct rubik_side_def rubik_sides[] = {
    [RUBIK_SIDE_ORANGE] = RUBIK_SIDE_DEF(WHITE,  TOP,
                                         GREEN,  TOP,
                                         YELLOW, TOP,
                                         BLUE,   TOP),

    [RUBIK_SIDE_RED] = RUBIK_SIDE_DEF(WHITE,  BOTTOM,
                                      BLUE,   BOTTOM,
                                      YELLOW, BOTTOM,
                                      GREEN,  BOTTOM),

    [RUBIK_SIDE_YELLOW] = RUBIK_SIDE_DEF(ORANGE, TOP,
                                         GREEN,  LEFT,
                                         RED,    BOTTOM,
                                         BLUE,   RIGHT),

    [RUBIK_SIDE_WHITE] = RUBIK_SIDE_DEF(ORANGE, BOTTOM,
                                        BLUE,   LEFT,
                                        RED,    TOP,
                                        GREEN,  RIGHT),

    [RUBIK_SIDE_BLUE] = RUBIK_SIDE_DEF(ORANGE, RIGHT,
                                       YELLOW, LEFT,
                                       RED,    LEFT,
                                       WHITE,  RIGHT),

    [RUBIK_SIDE_GREEN] = RUBIK_SIDE_DEF(ORANGE, LEFT,
                                        WHITE,  LEFT,
                                        RED,    RIGHT,
                                        YELLOW, RIGHT),
};

static void rubik_cube_read_side_adj(struct rubik_cube *cube, const struct rubik_side_adj *adj, struct rubik_side_adj_val *val)
{
    int i;
    int side_offset = 0;

    switch (adj->ori) {
    case ORIENT_BOTTOM:
        side_offset = RUBIK_SIZE * (RUBIK_SIZE - 1);
        /* fallthrough */

    case ORIENT_TOP:
        for (i = 0; i < RUBIK_SIZE; i++)
            val->stickers[i] = cube->sides[adj->side][side_offset + i];
        break;

    case ORIENT_LEFT:
        side_offset = RUBIK_SIZE - 1;
        /* fallthrough */

    case ORIENT_RIGHT:
        for (i = 0; i < RUBIK_SIZE; i++)
            val->stickers[i] = cube->sides[adj->side][side_offset + i * RUBIK_SIZE];
        break;
    }
}

static void rubik_cube_write_side_adj(struct rubik_cube *cube, const struct rubik_side_adj *adj, const struct rubik_side_adj_val *val)
{
    int i;
    int side_offset = 0;

    switch (adj->ori) {
    case ORIENT_BOTTOM:
        side_offset = RUBIK_SIZE * (RUBIK_SIZE - 1);
        /* fallthrough */

    case ORIENT_TOP:
        for (i = 0; i < RUBIK_SIZE; i++)
            cube->sides[adj->side][side_offset + i] = val->stickers[i];
        break;

    case ORIENT_LEFT:
        side_offset = RUBIK_SIZE - 1;
        /* fallthrough */

    case ORIENT_RIGHT:
        for (i = 0; i < RUBIK_SIZE; i++)
            cube->sides[adj->side][side_offset + i * RUBIK_SIZE] = val->stickers[i];
        break;
    }
}

static void rubik_cube_turn_cw(struct rubik_cube *cube, enum rubik_side rside, const struct rubik_side_def *def)
{
    struct rubik_side_adj_val tmp, cur;
    enum rubik_sticker tmp_stick;

    rubik_cube_read_side_adj(cube, def->adj + 3, &tmp);

    int i;
    for (i = 3; i > 0; i--) {
        rubik_cube_read_side_adj(cube, def->adj + i - 1, &cur);
        rubik_cube_write_side_adj(cube, def->adj + i, &cur);
    }

    rubik_cube_write_side_adj(cube, def->adj, &tmp);

    enum rubik_sticker (*side)[RUBIK_SIZE * RUBIK_SIZE] = &cube->sides[rside];

    tmp_stick = (*side)[0];
    (*side)[0] = (*side)[6];
    (*side)[6] = (*side)[8];
    (*side)[8] = (*side)[2];
    (*side)[2] = tmp_stick;

    tmp_stick = (*side)[1];
    (*side)[1] = (*side)[3];
    (*side)[3] = (*side)[7];
    (*side)[7] = (*side)[5];
    (*side)[5] = tmp_stick;
}

static void rubik_cube_turn_ccw(struct rubik_cube *cube, enum rubik_side rside, const struct rubik_side_def *def)
{
    struct rubik_side_adj_val tmp, cur;
    enum rubik_sticker tmp_stick;

    rubik_cube_read_side_adj(cube, def->adj, &tmp);

    int i;
    for (i = 1; i < 4; i++) {
        rubik_cube_read_side_adj(cube, def->adj + i, &cur);
        rubik_cube_write_side_adj(cube, def->adj + i - 1, &cur);
    }

    rubik_cube_write_side_adj(cube, def->adj + 3, &tmp);

    enum rubik_sticker (*side)[RUBIK_SIZE * RUBIK_SIZE] = &cube->sides[rside];

    tmp_stick = (*side)[6];
    (*side)[6] = (*side)[0];
    (*side)[0] = (*side)[2];
    (*side)[2] = (*side)[8];
    (*side)[8] = tmp_stick;

    tmp_stick = (*side)[3];
    (*side)[3] = (*side)[1];
    (*side)[1] = (*side)[5];
    (*side)[5] = (*side)[7];
    (*side)[7] = tmp_stick;
}

void rubik_cube_turn(struct rubik_cube *cube, enum rubik_side side, enum rubik_turn turn)
{
    const struct rubik_side_def *def = rubik_sides + side;

    switch (turn) {
    case RUBIK_TURN_CW:
        rubik_cube_turn_cw(cube, side, def);
        break;

    case RUBIK_TURN_CCW:
        rubik_cube_turn_ccw(cube, side, def);
        break;

    case RUBIK_TURN_DOUBLE:
        printf("NOT Implemented!\n");
        break;
    }
}

