#ifndef WQUEUE_H_INCLUDED
#define WQUEUE_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct workq_item {
	struct workq_item *next;
	void (*fun)(struct workq_item *work);
	uint32_t time;
	//NOTE: add switch for execute task 1 time
	//NOTE: add member next_execute_time for internal control
	uint32_t next_exec_time;
};

struct workq {
	struct workq_item *start;
	struct workq_item *end;
    //NOTE: add member timer for internal control
	uint32_t timer;
};

#define WORKQ_DECLARE(__name__)       \
                static struct workq wq_##__name__ = { \
				.start = NULL, \
				.end = NULL \
}

#define WORKQ_ITEM_DECLARE(__name__)       \
                static struct workq_item wqi_##__name__ = { \
				.next = NULL, \
				.fun = NULL, \
				.time = 0 \
}

void workq_item_init(struct workq_item *w,
		void (*fun)(struct workq_item * work));

void workq_init(struct workq *q);

bool workq_is_empty(struct workq *q);

// add task to work the end of queue?
void workq_post(struct workq *q, struct workq_item *w);

void workq_post_delayed(struct workq *q, struct workq_item *w, uint32_t dly);

// remove task from work queue?
void workq_cancel(struct workq *q, struct workq_item *w);

uint32_t workq_iterate(struct workq *q);

void workq_increase_tick(struct workq *q);
#endif /*WQUEUE_H_INCLUDED*/
