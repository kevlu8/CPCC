#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CPCC_VER "0.0.1"

void ffprint(const char *path, FILE *out) {
	FILE *f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	rewind(f);
	char *buf = (char *)malloc(fsize + 1);
	fread(buf, fsize, 1, f);
	buf[fsize] = 0;
	fclose(f);
	fputs(buf, out);
	fputs("\n", out);
	free(buf);
}

int main(int argc, char *argv[]) {
	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		// usage
		printf("%s %s %s %s %s %s %s\n", 
			"usage:",
			argv[0],
			"[--help] [--version] <filename> [<args>]",
			"filename must end with .cp to specify a CP file.",
			"arguments:\n"
			"	--help		Prints this message\n",
			"	--version	Prints the version of CPCC you are using\n",
			"	--nocredit	Compiles the file without the credits at the beginning"
		);
		if (argc < 2)
			return 1;
		else
			return 0;
	}
	if (strcmp(argv[1], "--version") == 0) {
		printf("CPCC version %s\n", CPCC_VER);
		return 0;
	}
	if (strcmp(&argv[1][strlen(argv[1])-3], ".cp")) {
		printf("Invalid file extension; files must end with \".cp\"\n");
		return 1;
	}
	FILE *in = fopen(argv[1], "r");
	if (in == NULL) {
		printf("Invalid file supplied!\n");
		return 1;
	}
	FILE *out = fopen("out.cpp", "w");
	if (out == NULL) {
		printf("Could not open result file!\n");
		return 1;
	}
	int credit = 1;
	if (argc > 2 && strcmp(argv[2], "--nocredit") == 0) credit = 0;
	fseek(in, 0, SEEK_END);
	int fsize = ftell(in);
	rewind(in);
	char *buf = (char *)malloc(fsize + 1);
	char *bufval = buf;
	fread(buf, fsize, 1, in);
	fclose(in);
	if (credit) ffprint("crucial/credit", out);
	ffprint("crucial/base", out);
	while (*buf) {
		int flag=0;
		char *q = strchr(buf, '\n');
		if (q == NULL) q = strchr(buf, 0);
		int len_line = q-buf;
		char *line = (char *)malloc(len_line);
		char *old_line = line;
		if (len_line == 0) goto next;
		strncpy(line, buf, len_line);
		line[len_line - 1] = 0;
		if (strncmp(line, "use ", 4) == 0) {
			line += 4;

			const char *dir = "utils/";
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s", dir, line);
			if (access(path, F_OK) == -1) {
				printf("%s %s %s\n",
					line,
					"is not recognized as a valid library.",
					"Check to see if you are running an up-to-date CPCC version."
				);
				return 1;
			}
			ffprint(path, out);
		} else flag=1;
		next:
		free(old_line);
		if (flag) break;
		if (*q == '\n') buf = q+1;
		else buf=q;
	}
	fputs(buf, out);
	free(bufval);
	fclose(out);
	return 0;
}