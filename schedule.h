#ifndef SCHEDULE_H_
#define SCHEDULE_H_

//#include <ucontext.h>
#include <stdint.h>
#include "objpool.h"


#define STACK_SIZE (1024*1024)

struct coroutine_t;

class schedule_t
{
public:
	typedef objpool_t<coroutine_t> coroutine_pool_t;
	typedef void (*coroutine_func_t)(schedule_t *schedule,void *ud);

public:
	schedule_t(void);
	~schedule_t(void);

	int coroutine_create(coroutine_func_t func,void *ud);
	coroutine_t *coroutine_find(int id);

	int coroutine_status(int id);

	int coroutine_running();
	void coroutine_running(int id);

	void coroutine_resume(int id);
	void coroutine_yield();

protected:
	static void mainfunc(schedule_t *schedule);
	void save_stack(coroutine_t *co,char *top);
	
private:
	//ucontext_t main_;
	char *stack_;

	int running_;

	coroutine_pool_t coroutine_pool_;
};

#endif
