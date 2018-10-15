
#include <cstdio>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "workers.h"

void runScrapeThread(unsigned int sleepTime, int argc, char *argv[]) {
	int pid = fork();
	if (pid == 0) {
		sprintf(argv[0], "scraperd --background");
		while(1) {
			scrape(argc, argv);
			sleep(sleepTime);
		}
	}
	wait(NULL);
}

void runCheckThread(unsigned int sleepTime, int argc, char *argv[]) {
	int pid = fork();
	if (pid == 0) {
		sprintf(argv[0], "checkerd --background");
		while(1) {
			check(argc, argv);
			sleep(sleepTime);
		}
	}
	wait(NULL);
}

int main(int argc, char *argv[])
{
	sprintf(argv[0], "proxy-scraper: master --daemon");

	signal(SIGCHLD, SIG_IGN);

	std::thread scraperThread(runScrapeThread, 60u, argc, argv);
	std::thread checkerThread(runCheckThread, 65u, argc, argv);

	scraperThread.join();
	checkerThread.join();

	return 0;
}
