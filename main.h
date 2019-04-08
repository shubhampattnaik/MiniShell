#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#define FAILURE -1
#define SUCCESS 0

typedef int data_t;
typedef struct node
{
    struct node *prev;
    data_t pid;
    data_t pgid;
    struct termios tmodes;
    struct node *next;
} DList;

static int flag = 0;
static int sig_flag = 0;
static int bg_flag = 0;
static int idx = 0;
static int msh_fd;
static int msh_pgid;
static struct termios msh_tmodes;
static int ret_val;

typedef enum
{
    e_success,
    e_failure,
    e_quit
} status;

typedef struct cmd
{
    char cmd_buf[5][5][10];
    char addr_buf[1024];
    char cmd[1024];
    char usr_prompt[50];
    char prompt_var[50];
} cmd_str;

typedef struct _parse_cmd
{
    char opt[100][100];
    char *buf[100][100];
    int pipe_count;
} parse_cmd;

void welcome_screen();
status check_prompt_cmd(cmd_str *cmd_t, int *);
status read_cmd(cmd_str *cmd_t);
status command_processor(cmd_str *cmd_t, int *flag, DList **head, DList **tail);
void command_parser(char *str, parse_cmd *cmd_p);
void process_piped_cmd(char *cmd[100][100]);
status check_sys_call(cmd_str *cmd_t);
status check_echo_cmd(cmd_str *cmd_t, int *flag);
void signal_handler(int arg);
int insert_at_last(DList **, data_t, data_t, DList **);
int delete_last(DList **, DList **);
int print_list(DList *head);
status exec_stopped_process_in_fg(cmd_str *cmd_t, DList **head, DList **tail);
status exec_stopped_process_in_bg(cmd_str *cmd_t, DList **head, DList **tail);

#endif
