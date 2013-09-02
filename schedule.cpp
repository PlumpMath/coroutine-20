#include "schedule.h"
#include "coroutine.h"
#include <assert.h>


schedule_t::schedule_t(void)
{
	this->stack_ = (char*)malloc(DEFAULT_SIZE);
	memset(this->stack_,0,DEFAULT_SIZE);

	this->running_ = -1;
}


schedule_t::~schedule_t(void)
{
	free(this->stack_);
}

int schedule_t::coroutine_create(coroutine_func_t func,void *ud)
{
	int co_id = this->coroutine_pool_.alloc_id();
	assert(co_id != -1);

	coroutine_t *co = &this->coroutine_pool_[co_id];
	co->schedule_ = this;
	co->stack_ = NULL;
	co->state_ = COROUTINE_READY;
	co->ud_ = ud;
	co->mainfunc_ = func;

	return co_id;
}

coroutine_t *schedule_t::coroutine_find(int id)
{
	assert(id >= 0 && id < this->coroutine_pool_.capacity());
	coroutine_t *co = &this->coroutine_pool_[id];
	assert(co != NULL);
	return co;
}

int schedule_t::coroutine_status(int id)
{
	coroutine_t *co = this->coroutine_find(id);
	return co->state_;
}

int schedule_t::coroutine_running()
{
	return this->running_;
}

void schedule_t::coroutine_running(int id)
{
	this->running_ = id;
}

void schedule_t::coroutine_resume(int id)
{
	coroutine_t *co = this->coroutine_find(id);

	switch(co->state_)
	{
	case COROUTINE_READY:
		{

		}
		break;

	case COROUTINE_SUSPEND:
		{

		}
		break;

	default:
		assert(0);
	}
}

void schedule_t::coroutine_yield()
{
	coroutine_t *co = this->coroutine_find(this->running_);
	this->save_stack(co,this->stack_ + STACK_SIZE);


	co->state_ = COROUTINE_SUSPEND;
	this->running_ = -1;
}


void schedule_t::mainfunc(schedule_t *schedule)
{
	int co_id = schedule->coroutine_running();
	coroutine_t *co = schedule->coroutine_find(co_id);

	co->mainfunc_(schedule,co->ud_);

	schedule->coroutine_pool_.free_id(co_id);
	schedule->coroutine_running(-1);
}

void schedule_t::save_stack(coroutine_t *co,char *top)
{

}

