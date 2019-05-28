#include "workq.h"
#include <stdbool.h>
#include <stdint.h>

static unsigned int func_call[3];
static volatile uint32_t tick;

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);
WORKQ_ITEM_DECLARE(item2);
WORKQ_ITEM_DECLARE(item3);

void fun1(struct workq_item *item)
{
	if (++func_call[0] >= 100) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item3);
		workq_cancel(&wq_main, &wqi_item2);
	}
}

void fun2(struct workq_item *item)
{
	if (++func_call[1] >= 300) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item3);
		workq_cancel(&wq_main, &wqi_item1);
	}
}

void fun3(struct workq_item *item)
{
	if (++func_call[2] >= 150) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item1);
		workq_cancel(&wq_main, &wqi_item2);
	}
}

int test_workq(void)
{
	workq_init(&wq_main);

	workq_item_init(&wqi_item1, fun1);
	workq_item_init(&wqi_item2, fun2);
	workq_item_init(&wqi_item3, fun3);

	workq_post_delayed(&wq_main, &wqi_item1, 3000);
	workq_post_delayed(&wq_main, &wqi_item2, 1000);
	workq_post_delayed(&wq_main, &wqi_item3, 2000);

	while (1) {
		if (workq_iterate(&wq_main, tick) == 0) {
			//Sleep
			uint32_t tmp = tick++;
			if (tick < tmp) {
				workq_time_overflowed(&wq_main);
			}
		}
	}

	return 0;
}