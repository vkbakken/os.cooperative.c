#include <stdbool.h>
#include <stdint.h>

#include "board/pins.h"
#include "cpu/io.h"
#include "workq.h"
#include <hal/hal_gpio.h>

static unsigned int func_call[3];

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item1);
WORKQ_ITEM_DECLARE(item2);
WORKQ_ITEM_DECLARE(item3);

void fun1(struct workq_item *item)
{
	hal_gpio_pin_toggle((uint32_t *)NRF_P0, BOARD_LED1_bp);
	if (++func_call[0] >= 100) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item3);
		workq_cancel(&wq_main, &wqi_item2);
	}
}

void fun2(struct workq_item *item)
{
	hal_gpio_pin_toggle((uint32_t *)NRF_P0, BOARD_LED2_bp);
	if (++func_call[1] >= 300) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item3);
		workq_cancel(&wq_main, &wqi_item1);
	}
}

void fun3(struct workq_item *item)
{
	hal_gpio_pin_toggle((uint32_t *)NRF_P0, BOARD_LED3_bp);
	if (++func_call[2] >= 150) {
		workq_cancel(&wq_main, item);
		workq_cancel(&wq_main, &wqi_item1);
		workq_cancel(&wq_main, &wqi_item2);
	}
}

int test_workq(void)
{
	if ((NRF_CLOCK->LFCLKSTAT & CLOCK_LFCLKSTAT_STATE_Msk) == CLOCK_LFCLKSTAT_STATE_NotRunning) {
		NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos;
		NRF_CLOCK->TASKS_LFCLKSTART = 1;
		while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
		NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	}

	NRF_RTC0->TASKS_STOP = 1;
	NRF_RTC0->PRESCALER = 0;
	NRF_RTC0->EVENTS_OVRFLW = 0;
//	NRF_RTC0->INTENSET = RTC_INTENSET_OVRFLW_Msk;
	NRF_RTC0->TASKS_CLEAR = 1;
	NRF_RTC0->TASKS_START = 1;

	NVIC_SetPriority(RTC0_IRQn, 0);
	NVIC_EnableIRQ(RTC0_IRQn);
    NRF_RTC0->INTENSET = RTC_INTENSET_TICK_Msk;

//    NRF_RTC0->CC[0] = 100 & 0x00ffffff;
//    NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;

	hal_gpio_config((uint32_t *)NRF_P0, BOARD_LED1_bp, GPIO_OUTPUT, GPIO_NOPULL);
	hal_gpio_config((uint32_t *)NRF_P0, BOARD_LED2_bp, GPIO_OUTPUT, GPIO_NOPULL);
	hal_gpio_config((uint32_t *)NRF_P0, BOARD_LED3_bp, GPIO_OUTPUT, GPIO_NOPULL);
	hal_gpio_config((uint32_t *)NRF_P0, BOARD_LED4_bp, GPIO_OUTPUT, GPIO_NOPULL);

	hal_gpio_pin_set((uint32_t *)NRF_P0, BOARD_LED1_bp);
	hal_gpio_pin_set((uint32_t *)NRF_P0, BOARD_LED2_bp);
	hal_gpio_pin_set((uint32_t *)NRF_P0, BOARD_LED3_bp);
	hal_gpio_pin_set((uint32_t *)NRF_P0, BOARD_LED4_bp);
	//  #define WORKQ_DECLARE(__name__)       \
//                static struct workq wq_##__name__

	workq_init(&wq_main);

	workq_item_init(&wqi_item1, fun1);
	workq_item_init(&wqi_item2, fun2);
	workq_item_init(&wqi_item3, fun3);

	workq_post_delayed(&wq_main, &wqi_item1, 3000);
	workq_post_delayed(&wq_main, &wqi_item2, 1000);
	workq_post_delayed(&wq_main, &wqi_item3, 2000);

	while (1) {
		if (workq_iterate(&wq_main) == 0) {
			//Sleep
		}		
	}

	return 0;
}

void rtc0_handler(void)
{
	if(NRF_RTC0->EVENTS_TICK){		
        workq_increase_tick(&wq_main);
        NRF_RTC0->EVENTS_TICK = 0;
	}
}