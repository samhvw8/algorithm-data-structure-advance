/*
 * $Id: btcmd.c,v 1.14 2012/11/15 12:19:46 mark Exp $
 * 
 * =====================================================================
 * Simple parser for BT test harness
 * =====================================================================
 *
 * Copyright (C) 2010 Mark Willson.
 *
 * This file is part of the B Tree library.
 *
 * The B Tree library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The B Tree library  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the B Tree library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	NAME
 *      btcmd - parses commands for bt lib test harness
 *
 * 	SYNOPSIS
 *      void btcmd(char*,CMDENTRY[],void(*)(int));
 *
 *      int btcmd_help(CMDBLK*);
 *      int btcmd_noop(CMDBLK*);
 *      int btcmd_comment(CMDBLK*);
 *      int btcmd_execute(CMDBLK*);
 *      int btcmd_prompt(CMDBLK*);
 *      int btcmd_system(CMDBLK*);
 *      int btcmd_echo(CMDBLK*);
 *      int btcmd_error(CMDBLK*);
 *
 * 	DESCRIPTION
 *      Provide command handling for the BT test harness aka the app.
 *      The app calls btcmd, passing in the command prompt string, an
 *      array of command definitions (see btcmd.h) and an error
 *      function.
 *
 *      The CMDENTRY array should have, as its last entry, a command
 *      with a zero length string, and a command function, which will
 *      be invoked if no valid command has been entered by the user.
 *
 *      btcmd will return on either EOF on tty input or when an app
 *      function returns a negative value.  App error function is
 *      called when a command function returns a value greater than
 *      zero.
 *
 *      This module provides a number of 'free' commands that the app
 *      can make available to the end user; these are commands related
 *      to scripting, help and access to system commands.  An example
 *      of the appropriate CMDENTRY setup may be found below.
 *      
 * 	NOTES
 *      This module was introduced when I became fed-up with looking
 *      at the mess of bt command handling.
 *
 */

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "btcmd.h"

#define MAXBUFSZ 132
#define MAXDESCSZ 512

int echo = FALSE;
int stop_on_error = FALSE;

int issue_prompt = TRUE;
int svp;
FILE* input = NULL;

/* Storage locations for command and argument tokens */
CMDBLK cblk;
char svcmd[MAXBUFSZ+1];
char cmd[MAXBUFSZ+1];
char arg[MAXBUFSZ+1];
char qual[MAXBUFSZ+1];
char all[MAXBUFSZ+1];

/* Setup for handling interrupts */
jmp_buf env;

void break_handler (int sig)
{
    longjmp(env,1);
}

/* forward declaration of local_cmds (for help) */
CMDENTRY local_cmds[];
/* Pointer to current applicaton command set (for help) */
CMDENTRY* current_app_cmds;

/* Nested command file handling variables and functions */
#define CMDFILEMAX 5

FILE* cmdinput[CMDFILEMAX];
int cmdtop = 0;

int pushcf(FILE* cf)
{
    if (cmdtop < CMDFILEMAX) {
        cmdinput[cmdtop++] = cf;
        return TRUE;
    }
    else {
        return FALSE;
    }
}

FILE* pullcf()
{
    if (cmdtop <= 0) {
        return NULL;
    }
    else {
        return cmdinput[--cmdtop];
    }
}

/* Dump cmdblk contents to stdout */

void dump_cmdblk()
{
    fprintf(stdout,"cmd: %s\n",cblk.cmd);
}

/* Return char pointer to first space or tab character before
 * the character position max in string s */
char* strbrk(char* s, int max)
{
    char* p = s;
    int len = strlen(s);

    if (len < max) {
        return s+len;
    }
    else {
        p += max;
        while (p != s && *p != ' ' && *p != '\t') p--;
        return (p==s?s+max:p);
    }
}

void display_help(CMDENTRY cmds[],char* cmd)
{
    const int maxdescwidth = 47;
    int i;
    char desc[MAXDESCSZ+1];
    char* cp;
    char* ed;
    
    if (STREMP(cmd)) {
        fprintf(stdout,"%-20s %-10s %s\n","Command,Abbrev","Args",
                "Description");
    }
    for ( i=0 ; !STREMP(cmds[i].cmd); i++ ) {
        char s[MAXBUFSZ+1];
        /* ignore commands with no description (probably hidden) */
        if (strlen(cmds[i].description) == 0) continue;
        if (STREMP(cmd) || (strncmp(cmds[i].cmd,cmd,MAXBUFSZ) == 0) ||
            (strncmp(cmds[i].abbrev,cmd,MAXBUFSZ) == 0)) {
            snprintf(s,MAXBUFSZ,"%s,%s",cmds[i].cmd,cmds[i].abbrev);
            strncpy(desc,cmds[i].description,MAXDESCSZ);
            ed = desc+strlen(desc);
            cp = strbrk(desc,maxdescwidth);
            *cp = '\0';cp++;
            fprintf(stdout,"%-20s %-10s %s\n",s,cmds[i].args,desc);
            while (cp < ed) {
                char* scp = cp;
                cp = strbrk(scp,maxdescwidth);
                *cp = '\0'; cp++;
                fprintf(stdout,"%20s %10s %s\n","","",scp);
            }
            if (!STREMP(cmd)) return;
        }
    }
    if (!STREMP(cmd)) {
        strncpy(cblk.cmd,cmd,MAXBUFSZ);  /* copy cmd to correct loc in
                                          * cblk */
        cblk.cmd[MAXBUFSZ-1] = '\0';
        (cmds[i].function)(&cblk);
    }
}

/* Define btcmd 'free' commands */

/* does nothing */
int btcmd_noop(CMDBLK* c) {
    return 0;
}

int btcmd_prompt(CMDBLK* c)
{
    issue_prompt = !issue_prompt;
    return 0;
}

int btcmd_execute(CMDBLK* c)
{
    if (!pushcf(input)) {
        fprintf(stderr,"command file stack exhausted at: %s\n",
                c->arg);
    }
    else {
        if (input == stdin) {
            svp = issue_prompt;
            issue_prompt = FALSE;
        }
        input = fopen(c->arg,"rt");
        if (input == NULL) {
            printf("unable to open execute file: %s\n",c->arg);
            input = pullcf();
            if (input == stdin) {
                issue_prompt = svp;
            }
        }
    }
    return 0;
}

int btcmd_close_execute(CMDBLK* c)
{
    fclose(input);
    input = pullcf();
    if (input == NULL) {
        fprintf(stderr,"command file stack underflow\n");
        input = stdin;
    }
    else if (input == stdin) {
        issue_prompt = svp;
    }
    return 0;
}

int btcmd_system(CMDBLK* c)
{
    system(c->all);
    return 0;
}

int btcmd_comment(CMDBLK* c)
{
    return 0;
}

int btcmd_help(CMDBLK* c)
{
    display_help(current_app_cmds,c->arg);
    return 0;
}

int btcmd_echo(CMDBLK* c)
{
    if (strncmp(c->arg,"on",2) == 0) {
        echo = TRUE;
    }
    else if (strncmp(c->arg,"off",3) == 0) {
        echo = FALSE;
    }
    else {
        fprintf(stderr,"echo: %s\n",(echo?"on":"off"));
    }
    return 0;
}

int btcmd_error(CMDBLK* c)
{
    if (strncmp(c->arg,"on",2) == 0) {
        stop_on_error = TRUE;
    }
    else if (strncmp(c->arg,"off",3) == 0) {
        stop_on_error = FALSE;
    }
    else {
        fprintf(stderr,"error: %s\n",(stop_on_error?"on":"off"));
    }
    return 0;
}

/* Sample CMDENTRY array for free commands.  App can include these as
 * necessary */
CMDENTRY local_cmds[] = {
    { "comment","#",btcmd_comment,"string",0,"Following text will be ignored."},
    { "execute","e",btcmd_execute,"filename",1,"Commence reading commands from "
      "file. execute commands may be nested."},
    { "echo","ec",btcmd_echo,"[on|off]",0,
      "Echo commands when on and reading from file." },         
    { "error","er",btcmd_error,"[on|off]",0,
      "Stop processing command files on error." },
    { "help","?",btcmd_help,"",0,"Provide help on supported commands."},
    { "prompt","p",btcmd_prompt,"",0,
      "Toggle prompting before reading command."},
    { "system","!",btcmd_system,"string",0,"Run shell command."},
    { "","",btcmd_noop,"END OF COMMANDS"}
};

char* non_ws(char* str){
    char* cp = str;

    while (*cp == ' ' || *cp == '\t') cp++;
    return cp;
}


int tty_input(FILE* input)
{
    struct stat statbuf;
    
    if (fstat(fileno(input),&statbuf) == 0) {
        return ((statbuf.st_mode & S_IFMT) == S_IFCHR); /* character
                                                           special */
     }
    else {
        fprintf(stderr,"unable to fstat file descriptor: %d\n",
                fileno(input));
        exit(EXIT_FAILURE);
    }
}

void rl_history(FILE* input,char* cmd)
{
#ifdef READLINE
    if (tty_input(input)) {
        add_history(cmd);
    }
#endif            
    return;
}

int tokenise(char* cmdbuf, char* cmd, char* arg, char* qual, char* all)
{
    char copybuf[MAXBUFSZ+1];
    char* cp;
    int nargs = 0;
    
    strncpy(copybuf,cmdbuf,MAXBUFSZ);
    cmd[0] = '\0';
    arg[0] = '\0';
    qual[0] = '\0';
    all[0] = '\0';
    
    if ((cp = strtok(copybuf," \n")) != NULL) {
        strncpy(cmd,cp,MAXBUFSZ);
        if ((cp = strtok(NULL,"\n")) != NULL) {
            strncpy(all,cp,MAXBUFSZ);
            /* now we've got all the command args, retokenise for arg
             * and qualifier */
            strncpy(copybuf,all,MAXBUFSZ);
            if ((cp = strtok(copybuf," \n")) != NULL) {
                strncpy(arg,cp,MAXBUFSZ);
                nargs++;
                if ((cp = strtok(NULL," \n")) != NULL) {
                    strncpy(qual,cp,MAXBUFSZ);
                    nargs++;
                }
            }
        }
    }
    return nargs;
}
void bad_args(char* s)
{
    fprintf(stderr,"%s: incorrect number of arguments.\n",s);
    return;
}

void find_cmd(char* cmdbuf,CMDENTRY cmds[])
{
    int i;
    char* cp;
    
    cp = non_ws(cmdbuf); /* locate first non-whitespace character */

    cblk.nargs = tokenise(cmdbuf,cmd,arg,qual,all);
    cblk.arg = arg;
    cblk.qualifier = qual;
    cblk.qual_int = atoi(qual);
    cblk.all = all;
    cblk.cmd = cmd;
    cblk.function = NULL;
    cblk.unknown_cmd = FALSE;
    for ( i=0 ; !STREMP(cmds[i].cmd); i++ ) {
        if (strlen(cmds[i].abbrev) == 1 && /* look for special command */
            cmds[i].abbrev[0] < 'A' &&
            *cp == cmds[i].abbrev[0]) {
            cblk.function = cmds[i].function;
            cblk.all = cp+1;
            return;
        }
        else if (strncmp(cmd,cmds[i].cmd,MAXBUFSZ) == 0 ||
                 strncmp(cmd,cmds[i].abbrev,MAXBUFSZ) == 0) {
            cblk.function = cmds[i].function;
            strncpy(cblk.cmd,cmds[i].cmd,MAXBUFSZ);
            cblk.cmd[MAXBUFSZ-1] = '\0';
            /* does # args match that required? */
            if (cmds[i].nargs != 0 &&
                cmds[i].nargs != cblk.nargs) {
                    cblk.nargs = -1;
            }
            return;
        }
    }
    /* empty sentinal command should invoke unknown command handler */
    if (!STREMP(cblk.cmd)) {
        cblk.function = cmds[i].function;
        cblk.unknown_cmd = TRUE;
    }
    return;
}
    
int btcmd(char* prompt_string,CMDENTRY app_cmds[],
              void(error_handler)(int))
{
    char* rlbuf = NULL;
    char* cp = NULL;
    static char cmdbuf[MAXBUFSZ+1];
    int status = 0;
    
    if (input == NULL) input = stdin;
    current_app_cmds = app_cmds;
    cblk.function = NULL;

    /* catch interrupts and always return here */
    if (setjmp(env) == 0) {
        signal(SIGINT,break_handler);
    }
    else {
        while (input != stdin) btcmd_close_execute(&cblk);
        fflush(stdout);
    }
    while (status >= 0) {
        if (rlbuf != NULL) {
            free(rlbuf);
            rlbuf = NULL;
        }
        if (tty_input(input)) {
#ifdef READLINE
            if (issue_prompt) 
                rlbuf = readline(prompt_string);
            else    
                rlbuf = readline(NULL);
            if (rlbuf != NULL) {
                if (rlbuf[0] == 0) continue;
                strncpy(cmdbuf,rlbuf,MAXBUFSZ);
                cmdbuf[MAXBUFSZ-1] = '\0';
            }
#else
            if (issue_prompt) printf("%s",prompt_string);
            cp = fgets(cmdbuf,MAXBUFSZ,input);
#endif            
        }
        else {
            cp = fgets(cmdbuf,MAXBUFSZ,input);
        }
        if (cp == NULL && rlbuf == NULL) {
            if (input == stdin) {
                return 0; /* end of file in tty command stream */
            }
            else {
                btcmd_close_execute(&cblk);
                continue;
            }
        }
        find_cmd(cmdbuf,app_cmds);
        if (cblk.function != NULL) {
            if (cblk.nargs < 0) {
                bad_args(cblk.cmd);
                status = 1;
            }
            else {
                rl_history(input,rlbuf);
                if (echo && !tty_input(input)) {
                    fprintf(stdout,"%s%s",
                            prompt_string,
                            cmdbuf);
                }
                status = (cblk.function)(&cblk);
                if (status > 0  && !cblk.unknown_cmd) {
                    (error_handler)(status);
                }
            }
            if (status > 0 && stop_on_error && !tty_input(input)) {
                fprintf(stdout,
                        "command file processing terminated "
                        "(error on).\n");
                if (input == stdin) {
                    return 0; /* must be error in here document */
                }
                else {
                    while (input != stdin) btcmd_close_execute(&cblk);
                }
            }   
        }
    }
    /* dump_cmdblk(); */
    signal(SIGINT,SIG_DFL);
    return status;
}


