
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arg_parser.h"
#include "cmd_parser.h"
#include "rubik.h"

static const char *rubik_version = "rubik-" Q(GBEMUC_VERSION);

static const char *arg_str = "[Flags] [Game]";

#define SCREEN_WIDTH (640 * 4)
#define SCREEN_HEIGHT (480 * 4)

#define XARGS \
    X(help, "help", 0, 'h', "Display help") \
    X(version, "version", 0, 'v', "Display version information") \
    X(last, NULL, 0, '\0', NULL)

enum arg_index {
    ARG_EXTRA = ARG_PARSER_EXTRA,
    ARG_ERR = ARG_PARSER_ERR,
    ARG_DONE = ARG_PARSER_DONE,
#define X(enu, id, arg, op, help_text) ARG_##enu,
    XARGS
#undef X
};

static const struct arg rubik_args[] = {
#define X(enu, id, arg, op, help_text) [ARG_##enu] = { .lng = id, .shrt = op, .help_txt = help_text, .has_arg = arg },
    XARGS
#undef X
};

static struct rubik_cube *cube;

static void cmd_cw(int argc, char **argv, va_list args)
{
    if (argc != 1) {
        printf("Please supply a face to rotate\n");
        return;
    }

    enum rubik_side side;
    int err = rubik_side_from_char(argv[0][0], &side);

    if (err != 0) {
        printf("Face '%c' not known\n", argv[0][0]);
        return;
    }

    printf("Rotate side: %d\n", side);

    rubik_cube_turn(cube, side, RUBIK_TURN_CW);
}

static void cmd_display_cube(int argc, char **argv, va_list args)
{
    int i;
    for (i = 0; i < 6; i++) {
        printf("%c:\n", rubik_side_char(i));
        printf("    %d %d %d\n",
                (cube->sides[i][0]),
                (cube->sides[i][1]),
                (cube->sides[i][2]));
        printf("    %d %d %d\n",
                (cube->sides[i][3]),
                (cube->sides[i][4]),
                (cube->sides[i][5]));
        printf("    %d %d %d\n",
                (cube->sides[i][6]),
                (cube->sides[i][7]),
                (cube->sides[i][8]));
        printf("\n");
    }
}

static int exit_flag = 0;

static void cmd_exit(int argc, char **argv, va_list args)
{
    exit_flag = 1;
}

struct cmd_desc cmds[] = {
    { 'r', "rotate", cmd_cw, "Rotate face counter clockwise", "face" },
    { 'd', "display", cmd_display_cube, "Display the current state of the cube", NULL },
    { 'e', "exit", cmd_exit, "Exit rubik", NULL },
};

void cmd_loop(void)
{
    int cur_buff = 0;
    ssize_t line_len[2];
    size_t line_max[2] = { 0, 0 };
    char *line[2] = { NULL, NULL };

    while (printf(" > "),
           fflush(stdout),
           line_len[cur_buff] = getline(line + cur_buff, line_max + cur_buff, stdin)) {
        int sel = cur_buff;

        /* We do some double buffering to allow entering a blank line to repeat
         * the last instruction.
         *
         * If a blank line is entered, then we switch the command-buffer to be
         * the last-used buffer, which contains the last instruction.
         *
         * If not, then we switch the current-buffer, so we overwrite our
         * second buffer next time and the current buffer is saved as the
         * 'last' instruction. */
        if (line_len[cur_buff] != 1)
            cur_buff ^= 1;
        else
            sel ^= 1;

        if (line[sel])
            run_cmd(cmds, line[sel]);

        if (exit_flag)
            break;
    }

    free(line[0]);
    free(line[1]);
}

int main(int argc, char **argv)
{
    enum arg_index ret;

    while ((ret = arg_parser(argc, argv, rubik_args)) != ARG_DONE) {
        switch (ret) {
        case ARG_help:
            display_help_text(argv[0], arg_str, "", "", rubik_args);
            return 0;
        case ARG_version:
            printf("%s\n", rubik_version);
            return 0;

        default:
            return 0;
        }
    }

    struct rubik_cube rubik_cube;

    cube = &rubik_cube;
    rubik_cube_init(cube);

    cmd_loop();

    return 0;
}

