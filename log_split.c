#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 2048

volatile int fl_write;

static int open_main_log(const char *path);
static void toggle_write(int signal);
static void show_optimized(void);

int main(int argc, char *argv[])
{
	int fd, b_read, b_write;
	int count, c;
	int max_size;
	char buffer[BUF_SIZE];
	char new_name[256];
	char *file_name;
	time_t log_time;
	struct tm *tm_ptr;
	struct sigaction sa;

	if (argc == 2 && strcmp(argv[1], "-v") == 0) {
		printf("log_split %s - %s\n\n", VER, BUILD);
		show_optimized();
		exit(0);
	}

	if (argc != 3) {
		printf("%s <log_name> <max_log_size in MB>\n", argv[0]);
		exit(0);
	}

	file_name = argv[1];
	max_size = atoi(argv[2]) *1024*1024;

	printf("Max log size: %d bytes (%d MB)\n", max_size, atoi(argv[2]));

	memset(&sa, 0, sizeof(struct sigaction));

	// Setup the sighub handler
	sa.sa_handler = &toggle_write;
	// Restart the system call, if at all possible
	sa.sa_flags = SA_RESTART;
	// Block every signal during the handler
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGHUP");
	}

	fd = open_main_log(file_name);

	c = 0;
	fl_write = 1;
	while (1) {
		b_read = read(0, buffer, BUF_SIZE);
		if (b_read <= 0) {
			perror("log_split");
			exit(1);
		}
		if (fl_write) {
			b_write = write(fd, buffer, b_read);
			count += b_write;
		}
		if (count > max_size) {
			log_time = time(NULL);
			tm_ptr = localtime(&log_time);

			close(fd);
			snprintf(new_name, 255, "%s_%04d%02d%02d_%02d%02d-%02d",
					file_name,
					tm_ptr->tm_year+1900, tm_ptr->tm_mon,
					tm_ptr->tm_mday,
					tm_ptr->tm_hour, tm_ptr->tm_min,
					c++);
			rename(file_name, new_name);
			fd = open_main_log(file_name);
			count = 0;
		}
	}

	exit(0);
}

static void toggle_write(int signal)
{
	(void) signal;

	if (fl_write) fl_write = 0;
	else fl_write = 1;
	printf("toggle_write: %d\n", fl_write);
}

static int open_main_log(const char *path)
{
	int fd;

	fd = open(path, O_WRONLY|O_CREAT, 0666);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	return fd;
}

static void show_optimized(void)
{
#ifdef __OPTIMIZE__
	printf("GCC: Optimized\n");
#else
	printf("GCC: Not Optimized !!!\n");
#endif
}

