#pragma once

typedef enum SUSPEND_BEHAVIOUR
{
	SLEEP, SPIN
} SUSPEND_BEHAVIOUR;

typedef enum TASK_RESULT
{
	ABORT, CONTINUE
} TASK_RESULT;

typedef TASK_RESULT (*task)(); // function pointer typedef for start_task

// gets the current time as microseconds
uint64_t get_now();

//suspends the execution by specified microseconds, either by sleeping or spinning
void suspend(const uint64_t microseconds, const SUSPEND_BEHAVIOUR behaviour);

// loops taskPtr, which is a function pointer. if task takes less than taskDuration in microseconds, execution will suspend by the difference.
void start_task(task taskPtr, uint64_t taskDuration, SUSPEND_BEHAVIOUR suspendBehaviour);

