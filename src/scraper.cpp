
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <sys/wait.h>
#include <sys/file.h>

#include "lib/TeaCurl.h"
#include "lib/TeaPCRE.h"

int scrape(int argc, char *argv[]) {

	const char *targetUrl[3] = {
		"https://www.sslproxies.org/",
		"https://free-proxy-list.net/",
		"https://www.us-proxy.org/"
	};

	for (int ii = 0; ii < 3; ++ii)
	{
		int pid = fork();
		if (pid == 0) {
			sprintf(argv[0], "scrape --outfile=uncheck_%d.tmp --background --target=%s", ii, targetUrl[ii]);
			TeaCurl *tch = new TeaCurl(targetUrl[ii]);
			printf("Scraping %s...\n", targetUrl[ii]);
			tch->exec();
			tch->close();
			size_t bodyLength = strlen(tch->getBody().c_str());
			char *body = (char*)malloc(bodyLength);
			memcpy(body, tch->getBody().c_str(), bodyLength);
			delete tch;
			TeaPCRE *tre = new TeaPCRE;
			tre->setPattern(
				"(?:<tr><td>)([\\d\\.]*)(?:<.+<td>)(\\d{2,5})(?:<.+<td class='hm'>)(.*)(?:<)", 
				PCRE_DOTALL | PCRE_CASELESS | PCRE_MULTILINE | PCRE_UNGREEDY
			);
			tre->setSubject(body);
			delete body;
			unsigned int i, j, matchCount, matchCountD2;
			char ***result = (char***)malloc(2000 * sizeof(char***));
			tre->multiFindAll(result, &matchCount, &matchCountD2);
			delete tre;
			char *filename = (char*)malloc(64);
			printf("Got %d proxies from %s\n", matchCount, targetUrl[ii]);
			sprintf(filename, "uncheck_%d.tmp", ii);
			FILE *handle = fopen(filename, "w");
			flock(fileno(handle), LOCK_EX);
			for (i = 0; i < matchCount; i++) {
				for (j = 1; j < matchCountD2; j++) {
					fwrite(result[i][j], strlen(result[i][j]), sizeof(char), handle);
					// printf("%s", result[i][j]);
					if (j != matchCountD2 - 1) {
						// printf("|");
						fwrite("|",1, sizeof(char), handle);
					}
				}
				// printf("\n");
				fwrite("\n", 1, sizeof(char), handle);
			}
			fflush(handle);
			flock(fileno(handle), LOCK_UN);
			fclose(handle);
			sleep(10);
			exit(0);
		}
	}

	wait(NULL);
	return 0;

}
