#include "workq.h"
#include "os_assert.h"

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

	//TODO: where should insert the new item to?

	// insert at the end of slist
	//	if (workq_is_empty(q)) {
	//		q->end = w;
	//		q->start = q->end;
	//	} else {
	//		q->end->next = w;
	//		q->end = w;
	//	}

	//insert at the start of slist
	w->next = q->start;
	q->start = w;
}

void workq_post_delayed(struct workq *q, struct workq_item *w, WQ_TICK_TYPE dly)
{
	//	__ASSERT(NULL != q);
	//	__ASSERT(NULL != w);
	//	__ASSERT(0 != dly);
	//
	//	if (q->start == w) {
	//		q->start = w->next;
	//	} else {
	//		for (struct workq_item **iterator = &(q->start); *iterator != NULL;
	//			 iterator = &(*iterator)->next) {
	//			if ((*iterator)->next == w) {
	//				(*iterator)->next = w->next;
	//				break;
	//			}
	//		}
	//	}

	//TODO: How can we add back the removed item to the workq ?

	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	//TODO: where should insert the new item to?

	// insert at the end of slist
	//	if (workq_is_empty(q)) {
	//		q->end = w;
	//		q->start = q->end;
	//	} else {
	//		q->end->next = w;
	//		q->end = w;
	//	}

	//insert at the start of slist
	w->next = q->start;
	q->start = w;

	w->time = dly;
	//update next execution time for current item
	w->next_exec_time = q->timer + w->time;
}

void workq_cancel(struct workq *q, struct workq_item *w)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	if (q->start == w) {
		q->start = w->next;
        w->next = NULL;
	} else {
//		for (struct workq_item **iterator = &(q->start);
//			 iterator != NULL & *iterator != NULL;
//			 iterator = &(*iterator)->next) {
//
//			if ((*iterator)->next == w) {
//				(*iterator)->next = w->next;
//				w->next = NULL;
//				break;
//			}
//		}
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

uint32_t workq_iterate(struct workq *q)
{
	__ASSERT(NULL != q);
	WQ_TICK_TYPE tmp;

//	for (struct workq_item **iterator = &(q->start);
//		 iterator != NULL & *iterator != NULL;
//		 iterator = &(*iterator)->next) {
//
//		if (!(*iterator)->sq_delayed &
//			q->timer >= (*iterator)->next_exec_time) {
//			__ASSERT(NULL != (*iterator)->fun);
//
//			//update next execution time for current item
//			tmp = q->timer + (*iterator)->time;
//			if (q->timer >= tmp) {
//				(*iterator)->sq_delayed = true;
//			}
//			(*iterator)->next_exec_time = tmp;
//
//			//execute item's task
//			(*iterator)->fun(*iterator);
//		}
//	}

	for (struct workq_item *iterator = q->start;
		 iterator != NULL;
		 iterator = iterator->next) {

		if (!iterator->sq_delayed &
			q->timer >= iterator->next_exec_time) {
			__ASSERT(NULL != iterator->fun);

            //execute item's task
			iterator->fun(iterator);

			//update next execution time for current item
			tmp = q->timer + iterator->time;
			if (q->timer >= tmp) {
				iterator->sq_delayed = true;
			}
			iterator->next_exec_time = tmp;		
		}
	}
	return 0;
}

void workq_increase_tick(struct workq *q)
{
	__ASSERT(NULL != q);

	WQ_TICK_TYPE tmp = q->timer++;

	if (q->timer < tmp) {
//		for (struct workq_item **iterator = &(q->start);
//			 iterator != NULL & *iterator != NULL;
//			 iterator = &(*iterator)->next) {
//
//			if ((*iterator)->sq_delayed) {
//				(*iterator)->sq_delayed = false;
//			}
//		}
		for (struct workq_item *iterator = q->start;
			 iterator != NULL;
			 iterator = iterator->next) {

			if (iterator->sq_delayed) {
				iterator->sq_delayed = false;
			}
		}
	}
	//	++q->timer;
}