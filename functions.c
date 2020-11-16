#include "main.h"

/*=================================================================================
  Signal handling for ctrl+z and ctrl+c
 * Input:		Signals: SIGINT, SIGTSTP
 * Output:		sig_flag
 * Description:		Handle signals for SIGINT and SIGTSTP, if signals are raised
 because of CTRL+C and CTRL+Z respectiviely.
 ==================================================================================*/
void signal_handler(int arg)
{
    if(arg == SIGINT)
    {
        //TERMINATE CURRENT CHILD PROCRSS
        sig_flag = 'c';
    }
    else if(arg == SIGTSTP)
    {
        //STOP CHILD PROCESS AND INSERT PID OF CHILD AT LAST
        sig_flag = 's';
    }
}

/*=================================================================================
  Read command till next-line char and store in into a buffer 
 * Input:		Structure pointer
 * Output:		e_success
 * Description:		Get one-one character from stdin, store it into an array,
 as well as into a 2D array with their options. 
 ==================================================================================*/
status read_cmd(cmd_str *cmd_t)
{
    int a = 0, i = 0, j = 0, k = 0;
    char ch;

    while((ch = getchar()) != '\n')
    {
        if(ch == ' ')
        {
            cmd_t->cmd_buf[i][j][++k] = '\0';
            k = -1;
            j++;
            cmd_t->cmd[++a] = ch;
            ch = getchar();
        }
        if(ch == '|')
        {
            //cmd_t->cmd_buf[i][j][++k] = '\0';
            k = -1;
            j = 0;
            i++;
            cmd_t->cmd[++a] = ch;
            ch = getchar();
            cmd_t->cmd[++a] = ch;
            ch = getchar();
        }
        cmd_t->cmd_buf[i][j][++k] = ch;
        cmd_t->cmd[++a] = ch;
    }
    if(cmd_t->cmd_buf[i][j][k] == ' ')
    {
        cmd_t->cmd_buf[i][j][k] = '\0';
        cmd_t->cmd[a] = '\0';
    }
    else
    {
        cmd_t->cmd_buf[i][j][++k] = '\0';
        cmd_t->cmd[++a] = '\0';
    }

    return e_success;
}

/*===============================================================================
 * Check for PS1 commands
 * Input:		Structure
 * Output:		e_success / e_failure
 * Description:		This function will check for <PS1=> command, and store
 new prompt to usr_prompt string.
 ================================================================================*/
status check_prompt_cmd(cmd_str *cmd_t, int *prompt_flag)
{
    int i, j = 0, len = 0;
    char ps_arr[6];
    char var_ps[6];
    char var_arr[20];

    len = strlen(cmd_t->cmd);

    if(len > 6)
    {
        for(i = 0; i < 5; i++)
        {
            ps_arr[i] = cmd_t->cmd[i];
        }
        ps_arr[i] = '\0';

        if(strcmp("PS1=$", ps_arr) == 0)
        {
            for(i = 0, j = 5; cmd_t->cmd[j] != '\0'; i++, j++)
            {
                var_arr[i] = cmd_t->cmd[j];
            }
            var_arr[i] = '\0';
            if(strcmp(cmd_t->prompt_var, var_arr) == 0)
            {
                *prompt_flag = 2;
                return e_success;
            }
        }
        else
        {
            ps_arr[4] = '\0';

            if(strcmp("PS1=", ps_arr) == 0)
            {
                for(i = 0; cmd_t->cmd[i] != '\0'; i++)
                {
                    cmd_t->usr_prompt[i] = cmd_t->cmd[i+4];
                }
                cmd_t->usr_prompt[i] = '\0';
                *prompt_flag = 1;

                return e_success;
            }
            else
            {
                for(i = (len - 1), j = 4; i >= (len-5); i--, j--)
                {
                    var_ps[j] = cmd_t->cmd[i];
                }
                var_ps[5] = '\0';
                if(strcmp("=$PS1", var_ps) == 0)
                {
                    for(i = 0; cmd_t->cmd[i] != '='; i++)
                    {
                        cmd_t->prompt_var[i] = cmd_t->cmd[i];
                    }
                    cmd_t->prompt_var[i] = '\0';

                    return e_success;
                }
            }
        }
    }
    return e_failure;
}

/*===============================================================================
 * Check if system call for the command is available or not 
 * Input:		Structure
 * Output:		e_success / e_failure / e_quit
 * Description:		This function will check for available system calls, for 
 cd and pwd, using string compare, and call the respective system call for that 
 command. Also if user is passing quit, it will terminate the minishell.
 ================================================================================*/
status check_sys_call(cmd_str *cmd_t)
{
    if(strcmp("cd", cmd_t->cmd_buf[0][0]) == 0)
    {
        if(*(cmd_t->cmd_buf[0][1]) == '\0')
        {
            chdir(getenv("HOME"));
        }
        else
        {
            if(chdir(cmd_t->cmd_buf[0][1]) == -1)
            {
                perror("chdir");
                return e_failure;
            }
        }
        return e_success;
    }
    if(strcmp("pwd", cmd_t->cmd_buf[0][0]) == 0)
    {
        //memset(&cmd_t->addr_buf, '\0', sizeof(cmd_t->addr_buf));
        getcwd(cmd_t->addr_buf, sizeof(cmd_t->addr_buf));
        printf("%s\n", cmd_t->addr_buf);
        return e_success;
    }

    if(strcmp("quit", cmd_t->cmd_buf[0][0]) == 0)
    {
        //Terminate from minishell
        return e_quit;
    }
}

/*===================================================================================
  Check for echo $ commands
 * Input:		Structure, flag
 * Output:		e_success / e_failure
 * Description:		This function will check for echo commands with '$' eg.-
 [1] echo $$:		Print PID of current process i.e., minishell
 [2] echo $?:		Print exit status of previous command
 [3] echo $SHELL:	Print address of minishell executive file
 ====================================================================================*/
status check_echo_cmd(cmd_str *cmd_t, int *flag)
{
    int i;
    char buf[20];

    if(strcmp("echo", cmd_t->cmd_buf[0][0]) == 0)
    {
        if(*(cmd_t->cmd_buf[0][1]) == '$')
        {
            strcpy(buf, cmd_t->cmd_buf[0][1]);

            if((strcmp("$$", buf)) == 0)
            {
                printf("%d\n", getpid());
            }
            else if((strcmp("$?", buf)) == 0)
            {
                printf("%d\n", ret_val);
            }
            else if((strcmp("$SHELL", buf)) == 0)
            {
                *flag = 1;
            }
            else
            {
                puts("Default case");
                return e_failure;
            }

            ret_val = 0;
            return e_success;
        }
        return e_failure;
    }
}

/*====================================================================================
 * Execute stopped process in foreground if available
 * Input:		Structure, DLL-Head, DLL-Tail
 * Output:		e_success / e_failure
 * Description:		If fg is triggered, check for avaibility of PID into linked list
 if stopped proccess is available, take the last stopped process PID and send a SIGCONT
 signal to that using kill system call, and remove that PID from linked list, else return
 e_failure.
 =====================================================================================*/
status exec_stopped_process_in_fg(cmd_str *cmd_t, DList **head, DList **tail)
{
    if((*head) != NULL)
    {
        int status;
        pid_t b_pid;

        DList *temp = *head;

        while(temp->next)
            temp = temp->next;
        b_pid = temp->pid;

        printf("%d\n", b_pid);

        if(kill((b_pid), SIGCONT) < 0)
        {
            perror("kill (SIGCONT)");
        }
        else
        {
            delete_last(head, tail);
            idx--;
        }

        waitpid(b_pid, &status, WUNTRACED);
    }
    else
    {
        printf("minishell: fg: current: no such job\n");
        return e_success;
    }
    return e_success;
}

/*====================================================================================
 * Execute stopped process in background if available
 * Input:		Structure, DLL-Head, DLL-Tail
 * Output:		e_success
 * Description:		If bg is triggered, check for avaibility of PID into linked list
 if stopped proccess is available, take the last stopped process PID and send a SIGCONT
 signal to that using kill system call, also handle SIGCHLD signal for background processes,
 and remove that PID from linked list, else return e_failure.
 =====================================================================================*/
status exec_stopped_process_in_bg(cmd_str *cmd_t, DList **head, DList **tail)
{
    if((*head) != NULL)
    {
        pid_t b_pid;
        int status;

        DList *temp = *head;

        signal(SIGCHLD, SIG_IGN);

        while(temp->next)
            temp = temp->next;
        b_pid = temp->pid;

        printf("%d &\n", b_pid);
        kill((b_pid), SIGCONT);

        delete_last(head, tail);
        idx--;

        //waitpid(b_pid, &status, WNOHANG);
    }
    else
    {
        printf("minishell: fg: current: no such job\n");
    }
    return e_success;
}

/*=======================================================================================
 * Process command in child process
 * Input:		Structure, flag, DLL-Head, DLL-Tail
 * Output:		e_success / e_failure
 * Description:		Send command (in string format) to command parser toconvert it into
 3-D array, then,
 [1] Check for echo commands -> Call check_echo_cmd()
 [2] Check for fg command    -> Call exec_stopped_process_in_fg()
 [3] Check for bg command    -> Call exec_stopped_process_in_bg()
 [4] Check for jobs command  -> Print linked list data
 * If command is without piping, create one child process, by calling fork, and execute 
 command in child process using execvp.
 * If command is with pipe pass command to process_piped_cmd() function.
 =======================================================================================*/
status command_processor(cmd_str *cmd_t, int *flag, DList **head, DList **tail)
{
    int status;
    parse_cmd cmd_p;

    command_parser(cmd_t->cmd, &cmd_p);
    if((check_echo_cmd(cmd_t, flag)) == e_success)
    {
        return e_success;
    }

    if(strcmp("fg", cmd_p.buf[0][0]) == 0)
    {
        //print_list(*head);
        if((exec_stopped_process_in_fg(cmd_t, head, tail)) == e_success)
        {
            return e_success;
        }
    }
    else if(strcmp("bg", cmd_p.buf[0][0]) == 0)
    {
        if((exec_stopped_process_in_bg(cmd_t, head, tail)) == e_success)
        {
            return e_success;
        }
    }
    else if(strcmp("jobs", cmd_p.buf[0][0]) == 0)
    {
        print_list(*head);
        ret_val = 0;
        return e_success;
    }

    if(!cmd_p.pipe_count)
    {
        pid_t pgid;
        pid_t pid = fork();
        if(!pid)
        {
            ret_val = execvp(cmd_p.buf[0][0], cmd_p.buf[0]);
            if(ret_val == -1)
            {
                printf("minishell: %s: command not found\n", cmd_p.buf[0][0]);
                exit(1);
            }
        }
        else
        {
            wait(&status);

            ret_val = status;

            setpgid(pid, msh_pgid);

            if(sig_flag == 'c')
            {
                printf("\n");
                if((kill(pid, SIGKILL)) < 0)
                {
                    printf("pid: %d\n", pid);
                    perror("kill (SIGKILL)");
                }

                sig_flag = 0;
                return e_failure;
            }
            else if(sig_flag == 's')
            {
                sig_flag = 0;
                pgid = pid;

                insert_at_last(head, pid, pgid, tail);
                printf("\n[%d]+  Stopped			%s %s  pid: %d\n", ++idx, cmd_p.buf[0][0], cmd_p.buf[0][1], pgid);
                return e_success;
            }
        }
    }
    else
    {
        process_piped_cmd(cmd_p.buf);
    }
    return e_success;

}

/*=========================================================================================
 * Separate commands with their options
 * Input:		Structure, String
 * Output:		void type
 * Description:		Separate commands with their options and store it into a 3-D array.
 =========================================================================================*/
void command_parser(char *str, parse_cmd *cmd_p)
{
    cmd_p->pipe_count = 0;
    //printf("arr2 = %s\n", str);

    int i = 0, j = -1, k = 0, a = 0, n = 0;

    while(str[k] != '\0')
    {
        if(str[k] == '|')
        {
            cmd_p->pipe_count++;
            cmd_p->buf[n][a] = NULL;
            a = 0; n++; k++; j = -1;
            if(str[k] == ' ')
            {
                k++;
            }
        }
        while(str[k] != ' ')
        {
            if(str[k] == '\0')
            {
                k--;
                goto label;
            }
            if(str[k] == '"')
            {
                k++;
            }
            else
            {
                cmd_p->opt[i][++j] = str[k];
                k++;
            }
        }
label:cmd_p->opt[i][++j] = '\0';
      cmd_p->buf[n][a] = cmd_p->opt[i];
      k++; i++; a++; j = -1;
    }
    cmd_p->opt[i][++j] = '\0';
    cmd_p->buf[n][a] = NULL;
    *(cmd_p->buf[++n]) = NULL;
}

/*===================================================================================
 * Process commands having 
 * Input:		Command buffer (3-D array)
 * Output:		Void type
 * Description:		Check with buffer, for first command create pipe, call fork(),
 inside child process, redirect STDIN to fd_in, check for next command in array if it
 is not NULL redirect STDOUT to fd[1](writer fd of pipe), close fd[0](reader fd of pipe)
 and execute the command with execvp, in parent process close.
 ====================================================================================*/
void process_piped_cmd(char *cmd[100][100])
{
    int i = 0;
    int fd[2];
    pid_t pid;
    int fd_in = 0;
    int status;

    while(cmd[i][0] != NULL)
    {
        pipe(fd);
        if((pid = fork()) == -1)
        {
            exit(1);
        }
        else if(pid == 0)
        {
            dup2(fd_in, 0);
            if(cmd[i + 1][0] != NULL)
            {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            ret_val = execvp(cmd[i][0], cmd[i]);
            if(ret_val == -1)
            {
                printf("minishell: %s: command not found\n", cmd[i][0]);
                exit(1);
            }
        }
        else
        {
            wait(&status);
            ret_val = status;
            close(fd[1]);
            fd_in = fd[0];
            i++;
        }
    }
}

/* Execute predefined commands for minishell */
status check_predefined_minishell_cmd(cmd_str *cmd_t)
{
    if(strcmp("--help", cmd_t->cmd_buf[0][0]) == 0)
    {
        welcome_screen();
        return e_success;
    }
    else if(strcmp("--h", cmd_t->cmd_buf[0][0]) == 0)
    {
        welcome_screen();
        return e_success;
    }
    return e_failure;
}

/* Welcome screen on startup */
void welcome_screen(void)
{
    system("clear");
    printf("\n\n\n\n\n\n\n\n============================================================================================================================================================\n");
    printf("============================================================================================================================================================\n\n\n\n\n");
    printf("\033[1;31m");
    printf("                                                                      *** MINI-SHELL ***\n\n");
    printf("\033[0m");  
    printf("                                                                        >Version 1.0\n");
    printf("                                                                    >By Shubham Pattnaik\n");
    printf("                                                                 >Released Date: 25-Mar-2019\n");
    printf("                                                       >This is a minimal demonstration of actual shell\n\n");
    printf("                                                       >type: --help<Enter>		for usage information\n");
    printf("                                                       >type: quit<Enter>		to exit\n");



    printf("\n\n\n\n                                                       	  <<<< Press enter key to execute minishell >>>>\n\n\n\n\n");
    printf("============================================================================================================================================================\n");
    printf("============================================================================================================================================================\n");
}




















