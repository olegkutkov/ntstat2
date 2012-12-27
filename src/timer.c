/* timer

   Copyright 2012  Oleg Kutkov <elenbert@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */

#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "timer.h"

static pthread_t timer_thread;
static volatile int timer_event = 0;
static timer_t timerid = { 0 };
static timer_callback callback_func = NULL;

void* thread_func(void* arg);


static void sig_handler(int sig, siginfo_t* si, void* uc)
{
	(void) &sig;
	(void) si;
	(void) uc;

	timer_event = 1;
}

static int setup_timer_sigaction()
{
	struct sigaction sig_action;

	sig_action.sa_flags = SA_SIGINFO;
	sig_action.sa_sigaction = sig_handler;
	sigemptyset(&sig_action.sa_mask);

	if (sigaction(SIGRTMIN, &sig_action, NULL) == -1) {
		printf("sigaction failed, error: %s", strerror(errno));
		return -1;
	}	

	return 0;
}

int timer_set_armed(const unsigned ms_timeout)
{
	struct itimerspec its;
	its.it_value.tv_sec = ms_timeout / 1000;
	its.it_value.tv_nsec = ms_timeout % 1000 * 1000000;

	its.it_value.tv_sec += its.it_value.tv_nsec / 1000000000UL;
	its.it_value.tv_sec += its.it_value.tv_nsec % 1000000000UL;

	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1) {
		printf("timer_settime faied, error: %s", strerror(errno));
		return -1;
	}

	return 0;
}

void set_timer_callback(timer_callback func)
{
	callback_func = func;
}

int start_timer(unsigned int timeout_ms, void* thread_arg)
{
	struct sigevent sev;

	if (setup_timer_sigaction() == -1) {
		return -1;
	}

	memset(&sev, 0, sizeof(struct sigevent));

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &timerid;

	if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
		printf("timer_create failed, error: %s", strerror(errno));
		return -1;
	}

	if (pthread_create(&timer_thread, NULL, thread_func, thread_arg) != 0) {
		printf("Failed to create timer thread, error %s", strerror(errno));
		return -1;
	}

	return timer_set_armed(timeout_ms);
}

void stop_timer()
{
	timer_set_armed(0);
	timer_delete(timerid);

	pthread_cancel(timer_thread);
	pthread_join(timer_thread, NULL);
}

void* thread_func(void* arg)
{
	if (callback_func) {
		callback_func(arg); /* first call even without timer events */
	}

	while (1) {
		if (timer_event && callback_func) {
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			callback_func(arg);
			timer_event = 0;
			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		}

		usleep(100000);
	}
}

