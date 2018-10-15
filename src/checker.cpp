
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include "lib/TeaCurl.h"
#include "lib/TeaPCRE.h"



int check(int argc, char *argv[]) {
	int worker_limit = 5;
	for (int i = 0; i < 2; ++i)
	{
		int pid = fork();
		if (pid == 0) {
			char *filename = (char*)malloc(64);
			sprintf(argv[0], "checkerd_run --file=uncheck_%d.tmp --worker-limit=%d", i, worker_limit);
			sprintf(filename, "uncheck_%d.tmp", i);
			FILE *handle = fopen(filename, "r");
			flock(fileno(handle), LOCK_SH);
			
			char *buff = (char*)malloc(8);
			char *ip   = (char*)malloc(64);
			char *port = (char*)malloc(32);
			char *country = (char*)malloc(256);

			int gotIp = 0;
			int workers = 0;
			while (!feof(handle)) {
				size_t rbt = fread(buff, 1, sizeof(char), handle);
				if (rbt) {
					buff[1] = '\0';
					if (buff[0] != '|' && (!gotIp)) {
						strcat(ip, buff);
					} else if (buff[0] == '|' && (!gotIp)) {
						gotIp = 1;
					} else if (buff[0] == '|' && gotIp) {
						while(rbt) {
							rbt = fread(buff, 1, sizeof(char), handle);
							buff[1] = '\0';
							if (buff[0] == '\n') break;
							if (buff[0] == ' ') buff[0] = '_';
							strcat(country, buff);
						}

						int chPid = fork();
						workers++;
						if (chPid == 0) {
							sprintf(argv[0], "checker_worker --ip=%s --port=%s", ip, port);
							printf("%s:%s %s\n", ip, port, country);
							sleep(10);
							exit(0);
						}

						if (workers == worker_limit)
						{
							wait(NULL);
							workers = 0;
						}


						gotIp = 0;

						delete ip;
						delete port;
						delete country;
						ip  = (char*)malloc(64);
						port = (char*)malloc(32);
						country = (char*)malloc(256);
					} else {
						strcat(port, buff);
					}
				}
			}

			flock(fileno(handle), LOCK_UN);
			fclose(handle);
			exit(0);
		}
	}
	wait(NULL);
	return 0;
}
