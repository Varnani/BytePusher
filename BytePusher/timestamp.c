#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "timestamp.h"
#pragma comment(lib, "winmm.lib")

static FILETIME ft;
static int initialized = 0;

uint64_t get_now()
{
	if (!initialized)
	{
		initialized = 1;
		TIMECAPS tcp;
		timeGetDevCaps(&tcp, sizeof(TIMECAPS));
		timeBeginPeriod(tcp.wPeriodMin);
	}

	GetSystemTimePreciseAsFileTime(&ft);

	uint64_t stamp = ((uint64_t)ft.dwHighDateTime) << 32 | ((uint64_t)ft.dwLowDateTime);
	stamp /= 10;
	return stamp;
}

void suspend(const uint64_t microseconds, const SUSPEND_BEHAVIOUR behaviour)
{
	if (behaviour == SPIN)
	{
		uint64_t target = get_now() + microseconds;
		while (get_now() < target)
		{
			;
		}
	}

	else if (behaviour == SLEEP)
	{
		DWORD milliseconds = (DWORD)((microseconds + 500) / 1000);
		Sleep(milliseconds);	
	}
}

void start_task(task taskPtr, uint64_t taskDuration, SUSPEND_BEHAVIOUR suspendBehaviour)
{
	uint64_t offset = 0; //sleep offset carried over from the previous frame
	int offsetSign = 1; // whether to add or subtract the offset from the taskStart

	while (1)
	{
		uint64_t taskStart = get_now();

		uint64_t nextTaskStart = taskStart + taskDuration;
		if (offsetSign) nextTaskStart += offset;
		else nextTaskStart -= offset;

		TASK_RESULT result = taskPtr();
		if (result == ABORT) break;

		uint64_t taskEnd = get_now();

		if (nextTaskStart > taskEnd)
		{
			uint64_t targetSleep = nextTaskStart - taskEnd;
			suspend(targetSleep, suspendBehaviour);

			uint64_t lateTaskEnd = get_now();
			offsetSign = lateTaskEnd < nextTaskStart;
			if (offsetSign) offset = nextTaskStart - lateTaskEnd;
			else offset = lateTaskEnd - nextTaskStart;

			//logging
			//double tgSleep = (double)targetSleep / 1000;
			//double actSleep = (double)(lateTaskEnd - taskEnd) / 1000;
			//double diff = ((double)offset / 1000) * (offsetSign ? 1 : -1);
			//double frameTime = (double)(taskEnd - taskStart) / 1000;
			//printf("sleep target: %.3f ms | actual sleep: %.3f ms | offset: %.3f ms | frametime: %.3f ms\n", tgSleep, actSleep, diff, frameTime);
		}

		else
		{
			printf("below 60fps!\n");
			offset = 0;
			offsetSign = 1;
		}
	}
}