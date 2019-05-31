#include "workq.h"
#include "os_assert.h"

/*---------------------------Internal interface-------------------------
  won't need to assert input because it's called inside this module only
  */

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static inline uint32_t workq_get_min(struct workq *q)
{
	uint32_t min_var = 0;

	if (q->start == NULL) {
		return min_var;
	} else {
		min_var = q->start->next_exec_time;
	}

	for (struct workq_item *iterator = q->start;
		 iterator != NULL;
		 iterator = iterator->next) {

		if (!iterator->postponed) {
			min_var = MIN(min_var, iterator->next_exec_time);
		}
	}

	return min_var;
}

void workq_init(struct workq *q)
{
	__ASSERT(NULL != q);

	q->start = NULL;
	q->end = NULL;
}

void workq_item_init(struct workq_item *w,
	void (*fun)(struct workq_item *work))
{
	__ASSERT(NULL != w);
	__ASSERT(NULL != fun);

	w->fun = fun;
	w->next = NULL;
	w->time = 0;
}

bool workq_is_empty(struct workq *q)
{
	__ASSERT(NULL != q);

	return (NULL == q->end ? true : false);
}

void workq_post(struct workq *q, struct workq_item *w)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	w->next = q->start;
	q->start = w;
}

void workq_post_delayed(struct workq *q, struct workq_item *w, uint32_t dly)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	//insert at the start of slist
	w->next = q->start;
	q->start = w;

	w->time = dly;
	//update next execution time for current item
	w->next_exec_time = 0;//current_time + w->time;
}

void workq_cancel(struct workq *q, struct workq_item *w)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	if (q->start == w) {
		q->start = w->next;
		w->next = NULL;
	} else {
		for (struct workq_item *iterator = q->start;
			 iterator != NULL;
			 iterator = iterator->next) {

			if (iterator->next == w) {
				iterator->next = w->next;
				w->next = NULL;
				break;
			}
		}
	}
}

uint32_t workq_iterate(struct workq *q, uint32_t current_time)
{
	__ASSERT(NULL != q);
	uint32_t tmp;

	//execute pending tasks
	for (struct workq_item *iterator = q->start;
		 iterator != NULL;
		 iterator = iterator->next) {

		if ((!iterator->postponed) &
			(current_time >= iterator->next_exec_time)) {
			__ASSERT(NULL != iterator->fun);

			//execute item's task
			iterator->fun(iterator);

			//update next execution time for current item
			tmp = current_time + iterator->time;
			if (current_time >= tmp) {
				//next execution time is overflowed
				iterator->postponed = true;
			}
			iterator->next_exec_time = tmp;
		}
	}

	//find the next execution time
	return workq_get_min(q);
}

void workq_time_overflowed(struct workq *q)
{
	for (struct workq_item *iterator = q->start;
		 iterator != NULL;
		 iterator = iterator->next) {

		if (iterator->postponed) {
			iterator->postponed = false;
		}
	}
}
