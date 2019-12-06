
#include "common.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cmd_parser.h"

char **parse_line(char *line, int *argc)
{
    char **lines = NULL;
    int sflag = 0, i, len, last = 0;

    len = strlen(line);
    *argc = 0;

    for (i = 0; i < len; i++) {
         if (!isspace(line[i]) && !sflag) {
             (*argc)++;
             lines = realloc(lines, *argc * sizeof(*lines));
             last = i;
             sflag = 1;
         } else if (isspace(line[i]) && sflag) {
             line[i] = '\0';
             lines[*argc - 1] = line + last;
             sflag = 0;
         }
    }

    if (sflag)
        lines[*argc - 1] = line + last;

    return lines;
}

void run_cmd(struct cmd_desc *cmds, char *line, ...)
{
    struct cmd_desc *cmd;
    char **lines;
    int args;
    va_list cmd_args;
    size_t len;

    lines = parse_line(line, &args);

    va_start(cmd_args, line);

    if (lines == NULL)
        return ;

    len = strlen(lines[0]);

    for (cmd = cmds; cmd->cmd_id != NULL; cmd++) {
        if (len > 2) {
            if (strcmp(lines[0], cmd->cmd_id) == 0) {
                (cmd->cmd) (args - 1, lines + 1, cmd_args);
                break;
            }
        } else {
            if (cmd->simple == lines[0][0]) {
                (cmd->cmd) (args - 1, lines + 1, cmd_args);
                break;
            }
        }
    }

    va_end(cmd_args);

    if (cmd->cmd_id == NULL)
        printf("Unknown command '%s'\n", lines[0]);

    free(lines);
}

