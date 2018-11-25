#include<lab1.h>
#include <stdio.h>
int schedulerType = 0;
void setschedclass (int sched_class)
{
	schedulerType = sched_class;
	kprintf("Scheduler type set to : %d", schedulerType);
}

int getschedclass()
{
	return schedulerType;
}