#include "testcase1.h"
#include "workq.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define rtc_get_tick() tick
#define rtc_set_cmp(val)

static unsigned int func_call[3];
static uint32_t tick, next_execute_time, next_execute_time_last;
static int test_index = -1, test_slice_index = -1, timing_failure = 0;
static int test_done = 0;
static int test_passed = 0;

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);
WORKQ_ITEM_DECLARE(item2);
WORKQ_ITEM_DECLARE(item3);

void timing_handler(void);

void fun1(struct workq_item *item)
{
	if (++func_call[0] > 100) {
		workq_cancel(&wq_main, item);
		++test_done;
	} else {
		//		printf("%d,\n", 1);
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
		//		printf("%d,\n", 2);
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
		//		printf("%d,\n", 3);
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

	workq_post_delayed(&wq_main, &wqi_item1, 3277);
	workq_post_delayed(&wq_main, &wqi_item2, 1000);
	workq_post_delayed(&wq_main, &wqi_item3, 1003);

	while (test_done < 3) {
		if (next_execute_time == tick) {
			timing_handler();
		}

		tick++;
	}

	int result = (int)(TEST_SIZE - test_passed);

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

void timing_handler(void)
{
	uint32_t next_exec_dt = 0;

	next_exec_dt = workq_iterate(&wq_main, next_execute_time);
	next_execute_time = next_execute_time + next_exec_dt;

	if (next_exec_dt > 3) {
		/* set next wake up time then put system back to sleep mode */
		rtc_set_cmp(0x00FFFFFF & (rtc_get_tick() + next_exec_dt));
	} else if (next_exec_dt == 0) {
		//turn off RTC ?
	} else {
		/* within critical range, 2 solution:
		 * 1. execute iterate right now - using this solution.
		 * 2. re-assign next_execute_time = tick + 3
		 * */
		printf("Dead zone detected, index: %d\n", test_index);
		timing_handler();
	}

	if (next_execute_time_last != next_execute_time) {
		next_execute_time_last = next_execute_time;
		//					printf("%d,\n",next_execute_time_last);
		if (expect_32_time_slice[++test_slice_index] != next_execute_time_last) {
			++timing_failure;
			printf("Time slice failed: executed at %d instead of %d, index: %d\n\r",
				next_execute_time_last,
				expect_32_time_slice[test_slice_index], test_slice_index);
		}
	}
}