#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <execinfo.h>
#include "Logger.h"

#include "worker.hpp"

void daemonize(void) {
    int fd;

    if (fork() != 0) exit(0); /* parent exits */
    setsid(); /* create a new session */

    /* Every output goes to /dev/null. If Redis is daemonized but
     * the 'logfile' is set to 'stdout' in the configuration file
     * it will not log at all. */
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }
}

hummer::CWorker worker;

void sigtermHandler(int sig)
{
	(void)sig;
    LOG_TRACE("SIGTERM received, scheduling shutting down...");
    worker.UnInit();
}

void segvHandler(int sig, siginfo_t *info, void *secret) {
    (void)info;
	(void)secret;
	void *trace[100];
    char **messages = NULL;
    int i, trace_size = 0;
    struct sigaction act;

    /* It's not safe to sdsfree() the returned string under memory
     * corruption conditions. Let it leak as we are going to abort */
    trace_size = backtrace(trace, 100);
    messages = backtrace_symbols(trace, trace_size);

    for (i=1; i<trace_size; ++i)
        LOG_TRACE("backtrace"<<messages[i]);

    /* free(messages); Don't call free() with possibly corrupted memory. */
    //if (server.daemonize) unlink(server.pidfile);

    /* Make sure we exit with the right signal at the end. So for instance
     * the core will be dumped if enabled. */
    sigemptyset (&act.sa_mask);
    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
     * is used. Otherwise, sa_handler is used */
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;
    sigaction (sig, &act, NULL);
    kill(getpid(),sig);
}


void setupSigSegvAction(void) {
	struct sigaction act;

	sigemptyset (&act.sa_mask);
	/* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
 	* is used. Otherwise, sa_handler is used */
	act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
	act.sa_sigaction = segvHandler;
	sigaction (SIGSEGV, &act, NULL);
	sigaction (SIGBUS, &act, NULL);
	sigaction (SIGFPE, &act, NULL);
	sigaction (SIGILL, &act, NULL);
	sigaction (SIGBUS, &act, NULL);

	act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
	act.sa_handler = sigtermHandler;
	sigaction (SIGTERM, &act, NULL);
	return;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		cout << "input work directory" << endl;
		return -1;
	}

	daemonize();
	setupSigSegvAction();	
	// init logger
	string path = argv[1];
	string configPath = path + "/" + "worker_local.xml";
	string logxml = path + "/" + "log4cxx.properties";
	if(access(path.c_str(),X_OK | R_OK | W_OK) != 0 
		|| access(configPath.c_str(), R_OK) 
		|| access(logxml.c_str(), R_OK)){
		cout << "directory have not permission or config file is not exist" << endl;
		return -1;
	}

	VSLogger::Init(logxml.c_str(), "dataserver");
	if (!worker.Init(configPath.c_str()))
	{
		cout<<"start monitor failed"<<endl;
		return -1;
	}
	worker.Run();
	for (;;)
	{
   		sleep(3);
	}
	
	worker.UnInit();	 
	return 0;
}


