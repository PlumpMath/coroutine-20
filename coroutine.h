#ifndef COROUTINE_H_
#define COROUTINE_H_

#include "schedule.h"

enum COROUTINE_STATE
{
	COROUTINE_DEAD = 0,
	COROUTINE_READY,
	COROUTINE_RUNNING,
	COROUTINE_SUSPEND,
};

struct coroutine_t
{
	schedule_t *schedule_;
	schedule_t::coroutine_func_t mainfunc_;
	void *ud_;
	char *stack_;
	COROUTINE_STATE state_;
};

#endif
