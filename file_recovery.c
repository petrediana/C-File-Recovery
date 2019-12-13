#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FILE_NAME "file_to_recover.txt"

void create_file() {
	FILE *file = fopen(FILE_NAME, "w");
	char buf[256];
	printf("\nWrite file content (Ctrl+D to stop):\n\n");
	while(fgets(buf, 256, stdin))
		fprintf(file, "%s", buf);
	fclose(file);
}

void wait_for_input(const char *msg) {
	char buf[32];

	printf("%s\n", msg);
	printf(" -- Press ENTER to continue ...\n");
	fflush(stdout);
	fgets(buf, 32, stdin);
}

void do_command(char *command, char *msg) {

	pid_t pid = fork();

	switch (pid) {
	case -1:
		//Error
		perror("Fork failed");
		exit(EXIT_FAILURE);
		break;
	case 0:
		//Child
		if(msg != NULL)
			printf("%s\n", msg);
		// Execute command
		execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		perror("Exec failed");
		exit(EXIT_FAILURE);
		break;
	default:
		//Parent
		waitpid(pid, NULL, 0);
	}

}

int main() {

	FILE *file;
	char buf[256];

	printf("Creating file...\n");
	create_file();
	printf("\nDone creating file '%s'\n", FILE_NAME);
	do_command("ls -lh", "Here, have a quick 'ls -lh':\n");
	printf("\n");

	printf("Opening file '%s'\n", FILE_NAME);
	file = fopen(FILE_NAME, "r");
	if (file == NULL) {
		perror("File not found");
		return 1;
	}
	printf("File now open, please check if it still exists\n");
	sprintf(buf, "cat %s", FILE_NAME);
	do_command(buf, "Here, have a quick 'cat':\n");
	wait_for_input("");
	printf("\n");

	printf("Removing the file ...\n");
	sprintf(buf, "rm %s", FILE_NAME);
	do_command(buf, NULL);

	printf("Check if your file is there anymore ...\n\n");
	do_command("ls -lh", "Here, have a quick 'ls -lh':\n");
	wait_for_input("");
	printf("\n");

	printf("Trying to recover the file ...\n");
	/*
	 Here is where the magic happens
	 
	 */
	do_command("cp /proc/$(pidof file_recovery)/fd/3 recovered.txt",
		   NULL);

	printf("Your file is now back!\n");
	do_command("ls -lh", "Here, have a quick 'ls -lh':\n");
	do_command("cat recovered.txt", "\nAnd also a quick 'cat':\n");
	wait_for_input("");

	fclose(file);
	return 0;
}

