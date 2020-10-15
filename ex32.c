#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NO_C_FILE		0
#define COMPILATION_ERROR	1
#define TIMEOUT			2
#define WRONG			3
#define SIMILAR			4
#define EXCELLENT		5

#define STR_SIZE 1024

int read_string(int fd, char* str);
int go_over_sub_folders(char* folder, char* input_file);

int main(int argc, char** argv)
{
	int configFile, i;
	char folder[STR_SIZE], input_file[STR_SIZE], right_output[STR_SIZE];
	if (argc < 2)
	{
		printf("Usage: ex32 <file name>\n");
		return 0;
	}
	configFile = open(argv[1], O_RDONLY);/*, S_IRUSR | S_IWUSR);*/
	if (configFile < 0)
	{
		printf("Cannot open %s file\n", argv[1]);
		return 0;
	}
	if (!read_string(configFile, folder))
	{
		/* ERROR */
		return 0;
	}
	if (!read_string(configFile, input_file))
	{
		/* ERROR */
		return 0;
	}
	if (!read_string(configFile, right_output))
	{
		/* ERROR */
		return 0;
	}
/*printf("%s\n", folder);
printf("%s\n", input_file);
printf("%s\n", right_output);*/
	close(configFile);
	go_over_sub_folders(folder, input_file);

	return 0;
}

int handle_sub_folder(char* folder, char* input_file)
{
	DIR *d;
	struct dirent *dir;
	char full_file_name[STR_SIZE], full_exe_name[STR_SIZE], full_output_name[STR_SIZE];
	int len, count_c_files = 0, status;
	pid_t pid;
	char* argv[5];
	argv[0] = "gcc";
	argv[2] = "-o";
	argv[4] = NULL;
	d = opendir(folder);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			/* handle files only and not folders (thus no need to filter out "." and "..") */
			if (dir->d_type == DT_REG)
			{
				len = strlen(dir->d_name);
				if ((len > 2) && (dir->d_name[len - 2] == '.') && (dir->d_name[len - 1] == 'c'))
				{
					count_c_files++;
					sprintf(full_file_name, "%s/%s", folder, dir->d_name);
					sprintf(full_exe_name, "%s/a.out", folder);
					//////////////////////printf("%s\n", full_file_name);
					argv[1] = full_file_name;
					argv[3] = full_exe_name;
					if ((pid = fork()) < 0)
					{
						/* ERROR */
					}
					else if (pid == 0)
					{
						if (execvp("gcc", argv) < 0)
						{
							/* ERROR */
						}
					}
					else
					{
						/* wait that the execvp will finish */
						while (wait(&status) != pid);
					}
					char *run_comma[] = {full_exe_name , NULL};
					sprintf(full_output_name, "%s/output.txt", folder);
					int cur_output = open(full_output_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

					printf("input file  == %s\n", input_file);

					int fp_input_file = open(input_file, O_RDONLY);
					if (cur_output < 0)
					{
						printf("cantt open unput file\n");
						//*******err
					}
					if ((pid = fork()) < 0)
					{
						printf("eer pud 0");
						/* ERROR */
					}
					else if (pid == 0) /*c= child */
					{

						/* the standard output will go to output file */
						dup2(cur_output, 1);
						/* get the user input from input file */
						dup2(fp_input_file, 0);
						if ((execvp(full_exe_name, run_comma)) < 0)
						{
		
							/* ERROR */
						} else
						{
							/* wait that the execvp will finish */
							while (wait(&status) != pid);
						}
					}
					
				} // c file 
			}// if (dir->d_type == DT_REG)
			
		} // end while
			closedir(d);
	} // end if(d)
	else
	{
	 return -1;
	} 
	if (count_c_files == 0)
		return NO_C_FILE;
	return 1;
}

int go_over_sub_folders(char* folder, char* input_file)
{
	DIR *d;
	struct dirent *dir;
	char sub_folder[STR_SIZE];
	d = opendir(folder);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0))
				continue;
			sprintf(sub_folder, "%s/%s", folder, dir->d_name);
			printf("SUB FOLDER Name: %s\n", dir->d_name);
			handle_sub_folder(sub_folder, input_file);
		}
		closedir(d);
	}
	else
	{
		/* ERROR */
		return 0;
	}
	return 1;
}

int read_string(int fd, char* str)
{
	int i;
	/* read first char */
	if (read(fd, str, 1) < 1)
	{
		return 0;
	}
	i = 1;
	while (str[i - 1] != '\n')
	{
		if (read(fd, &str[i], 1) < 1)
		    return 0;
		i++;
	}
	str[i - 1] = '\0';
	return 1;
}

