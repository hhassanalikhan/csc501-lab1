/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
extern int equalPriorities;
extern int epCheck;
extern int actualProcessesCount;
extern int schedulerType ;
extern int pList[50];
int samePriority;
extern int epCheck ;
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	pptr->inNext = 1;
	
	if(schedulerType == 1 && actualProcessesCount>1 )
	{
		int i =0;
		int samePriority = proctab[pList[1]].pprio;	
		for (i = 0; i < actualProcessesCount; ++i) 
        {
        	if(pList[i]!=49 && proctab[pList[i]].pprio != samePriority)
        	{
        		samePriority = -1;
        		break;
        	}
        }
	
        if(samePriority != -1)
        {	
			if(schedulerType == 1)
			{
        	epCheck = 1;
			proctab[49].pprio = proctab[pList[1]].pprio;
			}
			else
			{
				proctab[49].pprio = proctab[pList[1]].pprio;
			}
        //	schedulerType = 0;
        }
}
	restore(ps);
	return(newprio);
}
