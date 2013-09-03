#include "schedule.h"
#include "coroutine.h"
#include <assert.h>

schedule_t::schedule_t(void)
{
	this->stack_ = (char*) malloc(STACK_SIZE);
	memset(this->stack_, 0, STACK_SIZE);

	this->running_ = -1;
}

schedule_t::~schedule_t(void)
{
	free(this->stack_);
}

int schedule_t::coroutine_create(coroutine_func_t func, void *ud)
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

	switch (co->state_)
	{
	case COROUTINE_READY:
	{
		getcontext(&co->ctx_);
		co->ctx_.uc_stack.ss_sp = this->stack_ ;
		co->ctx_.uc_stack.ss_size = STACK_SIZE;
		co->ctx_.uc_link = &this->main_;
		co->state_ = COROUTINE_RUNNING;

		this->running_ = id;
		uintptr_t ptr = (uintptr_t) this;
		makecontext(&(co->ctx_), (void (*)())schedule_t::mainfunc, 1,ptr);
		swapcontext(&this->main_, &co->ctx_);
		break;
	}
	case COROUTINE_SUSPEND:
	{
		memcpy(this->stack_ + STACK_SIZE - co->size_,co->stack_,co->size_);
		this->running_ = id;
		co->state_ = COROUTINE_RUNNING;
		swapcontext(&this->main_,&co->ctx_);
		break;
	}
	default:
		assert(0);
	}
}

void schedule_t::coroutine_yield()
{
	coroutine_t *co = this->coroutine_find(this->running_);
	this->save_stack(co, (char*)this->stack_ + STACK_SIZE);
	co->state_ = COROUTINE_SUSPEND;
	this->running_ = -1;
	swapcontext(&co->ctx_,&this->main_);
}

void schedule_t::mainfunc(schedule_t *ptr)
{
	schedule_t *schedule = (schedule_t*) ptr;
	int co_id = schedule->coroutine_running();
	coroutine_t *co = schedule->coroutine_find(co_id);

	co->mainfunc_(schedule, co->ud_);

	schedule->coroutine_pool_.free_id(co_id);
	schedule->coroutine_running(-1);
}

void schedule_t::save_stack(coroutine_t *co, char *top)
{
	char dummy = 0;
	assert(top - &dummy <= STACK_SIZE);

	if (co->cap_ < top - &dummy)
	{
		free(co->stack_);
		co->cap_ = top - &dummy;
		co->stack_ = (char*)malloc(co->cap_);
	}
	co->size_ = top - &dummy;
	memcpy(co->stack_,&dummy,co->size_);
}

