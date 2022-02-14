#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef __NR_inspect_table
#define __NR_inspect_table 500
#endif

#define TABLETOP_MAX_PATH_LENGTH 256
struct fd_info {
	int fd;
	unsigned int flags;
	long long pos;
	char path[TABLETOP_MAX_PATH_LENGTH];
};

long inspect_table(pid_t pid, struct fd_info *entries, int max_entries)
{
	return syscall(__NR_inspect_table, pid, entries, max_entries);
}

void print_flags(unsigned int flags)
{
#define FLAG_CHECK(x) do { if (flags & (x)) printf("%s ", #x); } while (0)
	printf("flags: (0%o) ", flags);
	if ((flags & O_ACCMODE) == O_RDONLY)
		printf("%s", "O_RDONLY ");
	FLAG_CHECK(O_WRONLY);
	FLAG_CHECK(O_RDWR);
	FLAG_CHECK(O_APPEND);
	FLAG_CHECK(O_NONBLOCK);
	FLAG_CHECK(O_CLOEXEC);
#undef FLAG_CHECK
}

int main(int argc, char **argv)
{
	pid_t pid;
	long ret;
	struct fd_info *entries, *cur_entry;
	int max_entries, i;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <pid> <max_entries>\n", argv[0]);
		exit(1);
	}

	pid = atoi(argv[1]);
	max_entries = atoi(argv[2]);

	entries = malloc(sizeof(struct fd_info) * max_entries);
	if (!entries) {
		perror("malloc");
		exit(1);
	}
	ret = inspect_table(pid, entries, max_entries);
	printf("inspect_table (%ld): %s\n", ret, strerror(errno));

	if (ret < 0) {
		free(entries);
		exit(1);
	}

	if (ret > 0) {
		printf("\n----------------------------\n");
		for (i = 0; i < ret; i++) {
			cur_entry = entries + i;
			printf("fd: %d\n", cur_entry->fd);
			printf("path: %s\n", cur_entry->path);
			printf("pos: %llu\n", cur_entry->pos);

			print_flags(cur_entry->flags);
			printf("\n----------------------------\n");
		}
	}

	free(entries);
	return 0;
}
