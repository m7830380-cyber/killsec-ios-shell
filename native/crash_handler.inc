
// ---- KILLSEC iOS port: native crash log (appended to the generated dummy.cpp by CI) ----
// Writes Documents/KILLSEC-iOS-crash.log on fatal signals and uncaught C++ exceptions, with a raw
// backtrace and the ASLR slide so addresses can be symbolicated against the build.
#include <signal.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <exception>
#include <mach-o/dyld.h>

static char ks_crash_path[1024];

static void ks_write(int fd, const char *s) { if (s) write(fd, s, strlen(s)); }

static void ks_dump(const char *what) {
	int fd = open(ks_crash_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0) return;
	char buf[256];
	snprintf(buf, sizeof buf, "\n==== native crash: %s, time %ld, slide 0x%lx ====\n", what, (long)time(nullptr),
			(unsigned long)_dyld_get_image_vmaddr_slide(0));
	ks_write(fd, buf);
	void *frames[128];
	int n = backtrace(frames, 128);
	backtrace_symbols_fd(frames, n, fd);
	close(fd);
}

static void ks_signal(int sig) {
	ks_dump(strsignal(sig));
	signal(sig, SIG_DFL);
	raise(sig);
}

static void ks_terminate() {
	ks_dump("std::terminate (uncaught exception)");
	abort();
}

__attribute__((constructor)) static void ks_install_crash_handler() {
	const char *home = getenv("HOME");
	snprintf(ks_crash_path, sizeof ks_crash_path, "%s/Documents/KILLSEC-iOS-crash.log", home ? home : ".");
	int sigs[] = { SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGABRT, SIGTRAP };
	for (int s : sigs) signal(s, ks_signal);
	std::set_terminate(ks_terminate);
}
