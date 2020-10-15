#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX_SIZE 1024
int dif_sim_same(char** argv);
int min(int a, int b);
int loop_match(char* argv, char* buff2);

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
int main(int argc, char** argv)
{
	if (argc != 3) {
		write(2, "Error: More orLess than two files\n", 35);
		return -1;
	}
	int res = dif_sim_same(argv);
    return res;
}

int dif_sim_same(char** argv)
{
	char buff1[MAX_SIZE];
	char buff2[MAX_SIZE];
	char cur_buff[MAX_SIZE];
	char min_buff[MAX_SIZE];
	int match = 0, i, j, min_size, max_size, fp1, fp2, max_fp , same_len_flag = 0;
	off_t offset;
	fp1 = open(argv[1], O_RDONLY);
	if (fp1 == -1)
    	{
		write(2, "Error:First file doesn't exist.\n", 33);
		return -1;
    	}
        fp2 = open(argv[2], O_RDONLY);
	if (fp2 == -1)
        {
		write(2, "Error:Second file doesn't exist.\n", 34);
		close(fp1);
		return -1;
        }
	/*read the files into buffer*/
	int read1 = read(fp1, buff1, MAX_SIZE);
	int read2 = read(fp2, buff2, MAX_SIZE);
	if (read1 == -1 || read1 == -1)
	{
		write(2, "Error:can not read file.\n", 26);
		close(fp1);
		close(fp2);
		return -1;
	}
	close(fp1);
	close(fp2);
        /* At first, lets find the number of characters in each file*/
	/* The file offset is set to the size of the file plus offset bytes.*/
	int size_f1 = lseek(fp1, 0, SEEK_END);
	int size_f2 = lseek(fp2, 0, SEEK_END);
	/* Check if the files are the same */
	if (size_f1 == size_f2)
	{
		
		if (strcmp(buff1 , buff2) == 0)
		{
			return 1; // same
		} 
	}
	/* take the min result of the two options */
	int res = min(loop_match(argv[1], buff2), loop_match(argv[2], buff1));
	return res; 
}

int loop_match(char* argv, char* buff2)
{

	int fp = open(argv, O_RDONLY);
	char cur_buff[MAX_SIZE];
	int i, j, match = 0;
	int len_buff1 = lseek(fp, 0, SEEK_END);
	int len_buff2 = strlen(buff2);
	int min_size = min(len_buff1, len_buff2);
	for(i = 0; i < len_buff1; i++)
        {
		/* The file offset is set to offset bytes */
	       if (lseek(fp, i, SEEK_SET) == -1)
		{
			write(2, "Error: fail lseek function.\n", 24);
			close(fp);
			return -1;
                }
		int cur_size = read(fp, cur_buff, MAX_SIZE);
		for (j=0; j < min(len_buff2, cur_size) - 1; j++)
		{
			if(cur_buff[j] == buff2[j])
			{
				match++;
			}
		}
		if (match >= min_size / 2)
		{
			return 2; // similar
		}
		match = 0;

	}
	close(fp);
	return 3;// different
}
