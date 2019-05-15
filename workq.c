#include "workq.h"
#include "os_assert.h"

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

	if (workq_is_empty(q)) {
		q->end = w;
	} else {
		q->end->next = w;
		q->end = w;
	}
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

	struct workq_item **iterator = &(q->start);

	if (*(iterator) == w) {
		*(iterator) = NULL;
	} else {
		while ((*(iterator))->next != NULL) {
			if ((*(iterator))->next == w) {
				(*(iterator))->next = (*(iterator))->next->next;
			}
			iterator = &((*iterator)->next);
		}
	}
}

uint32_t workq_iterate(struct workq *q)
{
	__ASSERT(NULL != q);

	struct workq_item **iterator = &(q->start);



	return 0;
}
