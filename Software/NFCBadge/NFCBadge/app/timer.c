#include <atmel_start.h>
#include "timer.h"

struct timer_task task;
volatile uint32_t time = 0;

static void timer_work(const struct timer_task *const task)
{
	time++;
}

void millis_init(void)
{
	task.interval = 4; // 32768 / 8 = 4096, 4 counts
	task.cb = timer_work;
	task.mode = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &task);
	timer_start(&TIMER_0);
}

volatile uint32_t millis(void)
{
	return time;
}
