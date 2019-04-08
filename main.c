#include "main.h"

int main()
{

    welcome_screen();
    while(getchar() != '\n');

    int i = 0, prompt_flag = 0, msh_is_interactive;
    char cmd_ch;
    char prompt[50];
    cmd_str cmd_t;
    char shell_dir[100];
    DList *head = NULL; DList *tail = NULL;

    /* see if we are running interactively */
    msh_fd = STDIN_FILENO;
    msh_is_interactive = isatty(msh_fd);

    if(msh_is_interactive)
    {
	/* Loop until we are in the foreground */
	while(tcgetpgrp(msh_fd) != (msh_pgid = getpgrp()))
	    kill(- msh_pgid, SIGTTIN);

	/* Initialize structure with NULL */
	memset(&cmd_t.cmd_buf, '\0', sizeof(cmd_t.cmd_buf));
	ret_val = 0;

	/* signal handling for SIGINT and SIGTSTP */
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = signal_handler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);

	/* Put ourselves in our own process group */
	msh_pgid = getpid();
	if(setpgid(msh_pgid, msh_pgid) < 0)
	{
	    perror("Unable to put msh in its own process group");
	    exit(1);
	}

	/* Grab control of the terminal */
	tcsetpgrp(msh_fd, msh_pgid);

	/* Save default terminal attributes for shell */
	tcgetattr(msh_fd, &msh_tmodes);

	system("clear");
	getcwd(shell_dir, sizeof(shell_dir));
	//puts(shell_dir);


	while(1)
	{
	    i = 0;
	    int dir_flag = 0;
	    printf("\033[1;31m");
	    if(!flag)
	    {
		printf(">>msh:");
		getcwd(cmd_t.addr_buf, sizeof(cmd_t.addr_buf));
		printf("\033[1;35m");
		printf("%s# ", cmd_t.addr_buf);
		printf("\033[0m");
	    }
	    else
		printf(">>%s:", prompt);

	    printf("\033[0m");
	    //memset(&cmd_t, '\0', sizeof(cmd_t));

	    cmd_ch = getchar();
	    if(cmd_ch == '\n')
		continue;
	    else
	    {
		cmd_t.cmd[0] = cmd_ch;
		cmd_t.cmd_buf[0][0][0] = cmd_ch;
		if((read_cmd(&cmd_t)) != e_failure)
		{
		    if((check_prompt_cmd(&cmd_t, &prompt_flag)) == e_success)
		    {
			if(prompt_flag == 1)
			{
			    strcpy(prompt, cmd_t.usr_prompt);
			    flag = 1;
			}
			else if(prompt_flag == 2)
			{
			    flag = 0;
			}
			continue;
		    }
		    else if((check_sys_call(&cmd_t)) == e_success)
		    {
			continue;
		    }
		    else if((check_sys_call(&cmd_t)) == e_quit)
		    {
			//kill(msh_pgid, SIGKILL);
			return 0;
		    }
		    else if((command_processor(&cmd_t, &dir_flag, &head, &tail)) == e_success)
		    {
			//ret_val = 0;
			if(dir_flag)
			{
			    puts(shell_dir);
			}
			continue;
		    }
		}
	    }
	}
    }
}
