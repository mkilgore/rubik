#ifndef CMIPS_CMD_PARSER_H
#define CMIPS_CMD_PARSER_H

#include <stdarg.h>

struct cmd_desc {
    char simple;
    const char *cmd_id;
    void (*cmd) (int argc, char **argv, va_list args);

    const char *help;
    const char *args;
};

/* 'Line' will be modified.
 * Returned array needs to be free'd */
char **parse_line(char *line, int *argc);

void run_cmd(struct cmd_desc[], char *line, ...);

#endif
