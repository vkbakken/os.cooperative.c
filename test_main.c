#include <stdbool.h>
#include <stdint.h>

#include "workq.h"

static int func_call[3];

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);
WORKQ_ITEM_DECLARE(item2);
WORKQ_ITEM_DECLARE(item3);

void fun1(struct workq_item *item)
{
	func_call[0]++;
}

void fun2(struct workq_item *item)
{
	func_call[1]++;
}

void fun3(struct workq_item *item)
{
	if (++func_call[2] >= 1000) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item1);
		workq_cancel(&wq_main, &wqi_item2);
	}
}

int test_workq(void)
{

	//  #define WORKQ_DECLARE(__name__)       \
//                static struct workq wq_##__name__

	workq_init(&wq_main);
	workq_item_init(&wqi_item1, fun1);
	workq_item_init(&wqi_item2, fun2);
	workq_item_init(&wqi_item3, fun3);
	workq_post_delayed(&wq_main, &wqi_item1, 1000);
	workq_post_delayed(&wq_main, &wqi_item2, 2500);
	workq_post_delayed(&wq_main, &wqi_item3, 3000);
	while (1) {
		if (workq_iterate(&wq_main) == 0) {
			//Sleep
		}
		workq_increase_tick(&wq_main);
	}

	return 0;
}
