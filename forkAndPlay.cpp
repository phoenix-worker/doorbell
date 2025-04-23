#include <unistd.h>
#include <stdlib.h>

void ringBell();

void forkAndPlay()
{
	if (fork() == 0) {
		char ENV_HOME[] = "HOME=/home/customer";
		char ENV_XDG_RUNTIME_DIR[] = "XDG_RUNTIME_DIR=/run/user/1000";
		setgid(1000);
		setuid(1000);
		putenv(ENV_HOME);
		putenv(ENV_XDG_RUNTIME_DIR);
		ringBell();
		_exit(0);
	}
}
