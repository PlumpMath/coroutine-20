#ifndef OBJPOOL_H_
#define OBJPOOL_H_

#include <stdlib.h>
#include <string.h>
#include <vector>

#define DEFAULT_SIZE (64)

template<typename T>
class objpool_t
{
public:
	typedef std::vector<int> recover_ids_t;

public:
	objpool_t()
	{
		this->pool_ = (T*)malloc(DEFAULT_SIZE * sizeof(T));
		memset(this->pool_,0,sizeof(DEFAULT_SIZE * sizeof(T)));
		this->pos_ = -1;
		this->cap_ = DEFAULT_SIZE;
	}

	virtual ~objpool_t()
	{
		free(this->pool_);
	}


	int alloc_id()
	{
		if (this->recover_ids_.size() == 0)
		{
			if (this->pos_ >= this->cap_)
			{
				return this->realloc();
			} 
			return ++this->pos_;
		}

		int id = this->recover_ids_.back();
		this->recover_ids_.pop_back();
		return id;
	}

	void free_id(int id)
	{
		this->recover_ids_.push_back(id);
	}
	
	T &operator[](int i) 
	{ 
		return this->pool_[i]; 
	}

	const T &operator[](int i) const
	{ 
		return this->pool_[i]; 
	}

	int capacity()
	{
		return this->cap_;
	}

protected:
	int realloc()
	{
		int old_cap = this->cap_;
		T *old_ptr = this->pool_;

		int new_cap = old_cap << 1;
		T *new_ptr = (T*)malloc(new_cap * sizeof(T));
		memset(new_ptr,0,new_cap * sizeof(T));
		memcpy(new_ptr,old_ptr,old_cap);

		free(old_ptr);

		this->pool_ = new_ptr;
		this->cap_ = new_cap;
		return ++this->pos_;
	}
private:
	T *pool_;
	int pos_;
	int cap_;

	recover_ids_t recover_ids_;
};
#endif
