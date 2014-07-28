#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/wait.h> 
#include <memory.h>
#include <fcntl.h>
#include <string.h>

#define LOG_LEN 2048
#define SAMPLE_LEN 250
#define SAMPLE_INTTERVAL 200000
#define CPU_READ_COLUMN 7
#define MEM_READ_COLUMN 2


int g_fd_log_cpu;
int g_fd_log_mem;

//在形如str = pid   123 323  10      5  011，以空格分隔的字符串数组中，读第count个数值
int find_num(char * src, int count)
{
	char * pos_start = src;
	char * pos_end;
	char char_num[8];
	char char_blank[2]=" ";
	int i, num = 0;
	memset(char_num, 0, 8);

	//printf("char_blank = **%s**\n", char_blank);

	for(i = 1; i < count; i++)
	{
	//	printf("i=%d, pos_start = %s\n", i, pos_start);
		pos_start = strstr(pos_start, &char_blank);
	//	printf("i=%d, pos_start = %s\n", i, pos_start);
		while(*pos_start == *char_blank  && pos_start < src + strlen(src))
			pos_start++;
	}
     //printf("the %d number of src = %s\n", count, pos_start);

	pos_end = strstr(pos_start, &char_blank);
	printf("num len = %d\n", pos_end - pos_start);
	strncpy(char_num, pos_start, pos_end - pos_start);
	printf("char_num = %s\n", char_num);
	
	num = atoi(char_num);
	return num ;
}


int sample_resource(const char * pid, int round)
{
	char * cpu_result;
	char * mem_result;
	const char *pcfile = "cpu.log";
	const char *pmfile = "mem.log";
	int fd_cpu = -1;
	int fd_mem = -1;
	int ret;
	char * pos;
	int cpu_value=0, mem_value=0;
	char cpu_log_str[16];
	char mem_log_str[16];

	memset(cpu_log_str, 0 , 16);
	memset(mem_log_str, 0 , 16);
      cpu_result = malloc(LOG_LEN);
      mem_result = malloc(LOG_LEN);
	memset(cpu_result, 0 , LOG_LEN);
      memset(mem_result, 0 , LOG_LEN);
	  
	system("rm -rf cpu.log");
	system("rm -rf mem.log");
	system("top -n 1 > cpu.log");	
	system("top -n 1 -m> mem.log");

//top输出不是至标准输出，因此用文件记录	
	fd_cpu = open(pcfile, O_RDONLY);
	if (fd_cpu < 0)
	{
			printf("open file %s failed\n", pcfile);
			return -1;
	}
	printf("cpu file opened\n");
	fd_mem= open(pmfile, O_RDONLY);
	if (fd_mem < 0)
	{
			printf("open file %s failed\n", pmfile);
			return -1;
	}
	printf("mem file opened\n");

//读取采样log，解析	
	ret = read(fd_cpu, cpu_result, LOG_LEN);
	printf(" cpu_result = %s", cpu_result);
	pos =  strstr(cpu_result, pid);
	if(NULL == pos)
		printf("can't find pid in cpu result\n");
	else
	{
		cpu_value= find_num(pos, CPU_READ_COLUMN);
		printf("cpu:%d\n", cpu_value);
	}
	
	ret = read(fd_mem, mem_result, LOG_LEN);
	printf(" mem_result = %s", mem_result);
	pos =  strstr(mem_result, pid);
	if(NULL == pos)
		printf("can't find pid in mem result\n");
	else
	{
		mem_value= find_num(pos, MEM_READ_COLUMN);
		printf("mem:%d\n", mem_value);
	}
//格式化，写入log文件	
	sprintf(cpu_log_str, "%d %d\r\n", round, cpu_value);
	sprintf(mem_log_str, "%d %d\r\n", round, mem_value);	
	write(g_fd_log_cpu, &cpu_log_str , sizeof(cpu_log_str));
	write(g_fd_log_mem, &mem_log_str, sizeof(mem_log_str));
	
	system("rm -rf cpu.log");
	system("rm -rf mem.log");
	close(fd_cpu);
	close(fd_mem);
	return 0;
}

int main(void)
{
	int i;
	int fd_log_cpu,fd_log_mem;
	char pid[5]="997";

#if 1
	 if (daemon(1, 0)) 
	{
		 perror("daemon");
		 return -1;
	 }
	#endif 
	fd_log_cpu = open("cpu.txt", O_RDWR |O_CREAT |O_APPEND );
	g_fd_log_cpu = fd_log_cpu;
	fd_log_mem = open("mem.txt", O_RDWR |O_CREAT |O_APPEND );
	g_fd_log_mem = fd_log_mem;
	
	for(i = 0; i < SAMPLE_LEN; i++)
	{
		usleep(SAMPLE_INTTERVAL);
//	char * string = "pid   123 323  10      5  011";
	//printf("find ret= %d\n", find_num(string,5));
		printf("--------------------------------------------\n");
		sample_resource(pid, i);
	}
/*
	for(i = 0; i < SAMPLE_LEN; i++)
	{
		printf("cpu ret= %d\n", p1.cpu[i] );
		printf("mem ret= %d\n", p1.mem[i] );
	}
*/
	close(fd_log_cpu);
	close(fd_log_mem);
	system("chmod a+rw *.txt");
	return 0;
}
