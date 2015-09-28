#ifndef BTCMD_H
#define BTCMD_H

typedef struct cmd_blk CMDBLK;
typedef int(*FUN)(CMDBLK*);

struct cmd_blk {
    char* cmd;          /* command string */
    char* arg;          /* first argument */
    char* qualifier;    /* argument qualifier */
    int qual_int;       /* qualifier int value */
    char* all;          /* all arguments concatenated */
    int nargs;          /* number of args found (<0 if # args
                           mismatch) */
    int unknown_cmd;    /* TRUE is cmd not in table */
    FUN function;       /* function to call */
};

struct cmd_entry {
    char* cmd;
    char* abbrev;
    FUN function;
    char* args;
    int nargs;
    char* description;
};
typedef struct cmd_entry CMDENTRY;

#define STREMP(s) (s != NULL && (s)[0] == '\0')

int btcmd(char*,CMDENTRY[],void(*)(int));

int btcmd_help(CMDBLK*);

int btcmd_noop(CMDBLK*);
int btcmd_comment(CMDBLK*);
int btcmd_execute(CMDBLK*);
int btcmd_prompt(CMDBLK*);
int btcmd_system(CMDBLK*);
int btcmd_close_execute(CMDBLK*);
int btcmd_echo(CMDBLK*);
int btcmd_error(CMDBLK*);

#endif
