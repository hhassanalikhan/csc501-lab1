/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

extern int currentQuantum;
extern int toBeDeleted;
extern int actualProcessesCount;
extern int pList[];
int killRunning = 0;
int shouldEnd = 0;
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	killRunning = 1;
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	disable(ps);
//	if(strcmp(proctab[currpid].pname , "prnull") != 0)
	{
			//toBeDeleted = proctab[pid].quantum;
			//kprintf("\n\n kill and removing from pList running %s %d\n\n",proctab[pid].pname,proctab[pid].pstate);
			int i = 0;
			for(i =0;i<actualProcessesCount;i++)
			{
				if(pList[i] == pid)
				{
					// for(int i=0;i<actualProcessesCount;i++)
					// {
					// 	kprintf("%s,",proctab[pList[i]].pname);
					// }
					// kprintf("\n");
					int j = 0;
					for(j =i;j<actualProcessesCount;j++)
						{
							pList[j] = pList[j+1];
							//break;
						}

				}
			
			//	kprintf("\n");

			}
			actualProcessesCount--;
			// for(int i=0;i<actualProcessesCount;i++)
			// 	{
			// 		kprintf("%s,",proctab[pList[i]].pname);
			// 	}
			// 	kprintf("\n");
		//		proctab[pid].pstate = PRCURR;
		//	currentQuantum = 10;
			currentQuantum -= proctab[pid].quantum;

		}
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		
		return(SYSERR);
	}
	if (--numproc == 0 || actualProcessesCount == 0)
	{
		shouldEnd = 1;
	//	preempt = 10;
		xdone();
	//	restore(ps);
	//	return(OK);
	}

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	//send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
		//	kprintf("GOING IN HERE \n");
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}


//kprintf("CAme out HERE %s\n", proctab[currpid].pname);
	restore(ps);
	killRunning = 0;
	return(OK);
}
