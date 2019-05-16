#include "workq.h"
#include "os_assert.h"

void workq_init(struct workq *q)
{
	__ASSERT(NULL != q);

	q->start = NULL;
	q->end = NULL;
}

void workq_item_init(struct workq_item *w,
		void (*fun)(struct workq_item * work))
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
	if (workq_is_empty(q)) {
		q->end = w;
		q->start = q->end;
	} else {
		q->end->next = w;
		q->end = w;
	}

	//insert at the start of slist
//	w->next = q->start;
//	q->start = w;
}

void workq_post_delayed(struct workq *q, struct workq_item *w, uint32_t dly)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);
	__ASSERT(0 != dly);

}

void workq_cancel(struct workq *q, struct workq_item *w)
{
	__ASSERT(NULL != q);
	__ASSERT(NULL != w);

	if (q->start == w) {
		q->start = w->next;
	} else {
		for (struct workq_item **iterator = &(q->start); *iterator != NULL;
				iterator = &(*iterator)->next) {
			if ((*iterator)->next == w) {
				(*iterator)->next = w->next;
				break;
			}
		}
	}
}

uint32_t workq_iterate(struct workq *q)
{
	__ASSERT(NULL != q);

	for (struct workq_item *iterator = q->start; iterator != NULL; iterator =
			iterator->next) {
		__ASSERT(NULL != iterator->fun);

		iterator->fun(iterator);
	}

	return 0;
}
