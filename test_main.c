#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "workq.h"
#include "testcase1.h"

static unsigned int func_call[3];
static volatile uint32_t tick, next_execute_time, next_execute_time_last;
static int test_index = -1, test_slice_index = -1;
static int test_done = 0;
static int test_passed = 0;

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);
WORKQ_ITEM_DECLARE(item2);
WORKQ_ITEM_DECLARE(item3);

void fun1(struct workq_item *item)
{
	if (++func_call[0] > 100) {
		workq_cancel(&wq_main, item);
		++test_done;
	} else {
		if (1 == expect_result[++test_index]) {
			++test_passed;
		} else {
			printf("[%d/%d] func1 failed! func%d should be executed at this order\n",
					test_index, TEST_SIZE, expect_result[test_index]);
		}
	}
}

void fun2(struct workq_item *item)
{
	if (++func_call[1] > 300) {
		workq_cancel(&wq_main, item);
		++test_done;
	} else {
		if (2 == expect_result[++test_index]) {
			++test_passed;
		} else {
			printf("[%d/%d] func2 failed! func%d should be executed at this order\n",
					test_index, TEST_SIZE, expect_result[test_index]);
		}
	}
}

void fun3(struct workq_item *item)
{
	if (++func_call[2] > 150) {
		workq_cancel(&wq_main, item);
		++test_done;
	} else {
		if (3 == expect_result[++test_index]) {
			++test_passed;
		} else {
			printf("[%d/%d] func3 failed! func%d should be executed at this order\n",
					test_index, TEST_SIZE, expect_result[test_index]);
		}
	}
}

int main(void)
{
	workq_init(&wq_main);

	workq_item_init(&wqi_item1, fun1);
	workq_item_init(&wqi_item2, fun2);
	workq_item_init(&wqi_item3, fun3);

	workq_post_delayed(&wq_main, &wqi_item1, 3000);
	workq_post_delayed(&wq_main, &wqi_item2, 1000);
	workq_post_delayed(&wq_main, &wqi_item3, 2000);

	while (test_done < 3) {
		next_execute_time = workq_iterate(&wq_main, tick);

		if (next_execute_time == 0) {
			/* Put system into sleep mode */
		} else if (next_execute_time - tick <= 3) {
			/* within critical range, 2 solution:
			 * 1. execute iterate right now
			 * 2. re-assign next_execute_time = tick + 3
			 * */
		} else {
			/* set next wake up time then put system into sleep mode */
			if (next_execute_time_last != next_execute_time) {
				next_execute_time_last = next_execute_time;
				if (expect_32_time_slice[++test_slice_index]
						!= next_execute_time_last) {
					printf("Time slice failed: executed at %d instead of %d, index: %d\n\r",
							next_execute_time_last,
							expect_32_time_slice[test_slice_index],
							test_slice_index);
				}
			}
		}

		tick++;
	}

	int result = (int) (TEST_SIZE - test_passed);

	if (0 == result)
		printf("Test order passed: %d/%d tasks passed!\n\r", test_passed,
		TEST_SIZE);
	else
		printf("Test order failed: %d/%d tasks passed!\n\r", test_passed,
		TEST_SIZE);

	printf("Tasks executed in wrong order: %d tasks!\n\r",
	TEST_SIZE - test_passed);

	return result;
}
