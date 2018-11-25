/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab1.h>

#define LOOP 100
extern int clockStart;
extern int clockEnd;
int proc_a(char), proc_b(char), proc_c(char);

int proc(char);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int s = 0;

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{

	int tj = 3;//Scheduler control variable
	int prA, prB, prC, prD, prE, prF, prG, prH;
	int count = 0;
	int i=0;

	srand(1234);
	
	int chd = 0;
	if(tj == 1)
	{
		setschedclass(EXPDISTSCHED);
		prA = create(proc_a, 2000, 10, "proc A", 1, 'A');
		prB = create(proc_b, 2000, 20, "proc B", 1, 'B');
		prC = create(proc_c, 2000, 30, "proc C", 1, 'C');
		resume(prA);
		resume(prB);
		resume(prC);
		sleep(10);
		kill(prA);
		kill(prB);
		kill(prC);
		float sum = (a_cnt+ b_cnt+ c_cnt);
		//kprintf("\nTest Result: A = %d, B = %d, C = %d\n",(int)(((float)a_cnt/sum)*100),(int)(((float)b_cnt/ sum)*100),(int)(((float)c_cnt/ sum)*100 ));
	kprintf("\nTest Result: A = %d, B = %d, C = %d\n", a_cnt, b_cnt, c_cnt);
}
	//Part 2 of assignment 2 
	else if(tj == 2)
	{
		setschedclass (LINUXSCHED);
		kprintf("\n\nHello World, Xinu lives\n\n");
			resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
			resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
			resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));

				while (count++ < LOOP) {
				kprintf("M");
				for (i = 0; i < 1000000; i++)
					;
			}
	}
	else
	{
		prA = create(proc_a, 2000, 10, "proc A", 1, 'A');
		prB = create(proc_b, 2000, 20, "proc B", 1, 'B');
		prC = create(proc_c, 2000, 30, "proc C", 1, 'C');
		resume(prA);
		resume(prB);
		resume(prC);
		sleep(10);
		kill(prA);
		kill(prB);
		kill(prC);
		float sum = (a_cnt+ b_cnt+ c_cnt);
		kprintf("\nTest Result: A = %d, B = %d, C = %d\n",(int)(((float)a_cnt/sum)*100),(int)(((float)b_cnt/ sum)*100),(int)(((float)c_cnt/ sum)*100 ));

	}
	return 0;
}

int proc_a(char c) {
	int i;
	kprintf("Start... %c\n", c);
	//b_cnt = 0;
	//c_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
	return 0;
}

int proc_b(char c) {
	int i;
	kprintf("Start... %c\n", c);
	//a_cnt = 0;
	//c_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;
	}
	return 0;
}

int proc_c(char c) {
	int i;
	kprintf("Start... %c\n", c);
	//a_cnt = 0;
	//b_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		c_cnt++;
	}
	return 0;
}


int proc(char c) {
	int i;
	int count = 0;
	//if(c=='C')
	//	sleep(1);
	while (count++ < LOOP) {
		kprintf("%c", c);

		for (i = 0; i < 1000000; i++)
			;
	}
	return 0;
}
