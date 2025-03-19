



`
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int executor(char **argv, int i, char **envp);


int main(int argc, char **argv, char **envp)
{
	(void)argc;
	
	int status;
	int i;
	
	status = 0;
	i = 0;

	while(argv[i])
	{
		argv += i + 1;

		i = 0;

		while(argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
		{
			i++;
		}

		if(i)
		{
			status = executor(argv, i, envp);
		}
	}
	return (status);
}



void error_msg(char *str)
{
	while(*str)
	{
		write(2, str, 1);
		str++;
	}
}


int ft_cd(char **argv, int i)
{
	if (i != 2)
	{
		error_msg("error: cd: bad arguments\n");
		return(1);
	}
	if (chdir(argv[1]) == -1)
	{
		error_msg("error: cd: cannot change directory to ");
		error_msg(argv[1]);
		error_msg("\n");
		return (1);
	}
	return (0);
}



void set_dup2(int is_pipe, int *fd, int new_fd)
{
	if(is_pipe && (dup2(fd[new_fd], new_fd) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		error_msg("error: fatal\n");
		exit(1);
	}
}


int executor(char **argv, int i, char **envp)
{
	int is_pipe, fd[2], status;

	pid_t pid;

	is_pipe = argv[i] && !strcmp(argv[i], "|");

	if(!is_pipe && !strcmp(argv[0], "cd"))
	{
		return (ft_cd(argv, i));
	}

	if(is_pipe && pipe(fd) == -1)
	{
		error_msg("error: fatal\n");
		exit(1);
	}

	if((pid = fork()) == -1)
	{
		error_msg("error: fatal\n");
		exit(1);
	}


	if (pid == 0)
	{
		argv[i] = 0;

		set_dup2(is_pipe, fd, 1);

		if (!strcmp(argv[0], "cd"))
		{
			exit(ft_cd(argv, i));
		}

		if(execve(argv[0], argv, envp) == -1)
		{
			error_msg("error: cannot execute ");
			error_msg(argv[0]), error_msg("\n");
			exit(1);
		}

	}

	waitpid(pid, &status, 0);

	set_dup2(is_pipe, fd, 0);

	return(WEXITSTATUS(status));
	
}
