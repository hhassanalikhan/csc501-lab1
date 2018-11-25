/* create.c - create, newpid */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <stdio.h>
LOCAL int newpid();
extern int smallestPR;
extern int currentQuantum;
extern int currentEPoch;
extern int actualProcessesCount;
extern int pList[50];
int equalPriorities =-2;
int epCheck = 0;
extern unsigned long ctr1000;
extern int schedulerType;
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	unsigned long	savsp, *pushsp;
	STATWORD 	ps;    
	int		pid;		/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int		i;
	unsigned long	*a;		/* points to list of args	*/
	unsigned long	*saddr;		/* stack address		*/
	int		INITRET();

	disable(ps);
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (int) roundew(ssize);
	if (((saddr = (unsigned long *)getstk(ssize)) ==
	    (unsigned long *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		restore(ps);
		return(SYSERR);
	}

	numproc++;
	pptr = &proctab[pid];

	pptr->fildes[0] = 0;	/* stdin set to console */
	pptr->fildes[1] = 0;	/* stdout set to console */
	pptr->fildes[2] = 0;	/* stderr set to console */

	for (i=3; i < _NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = FDFREE;

	pptr->pstate = PRSUSP;
	for (i=0 ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->pbase = (long) saddr;
	pptr->pstklen = ssize;
	pptr->psem = 0;
	pptr->phasmsg = FALSE;
	pptr->plimit = pptr->pbase - ssize + sizeof (long);	
	pptr->pirmask[0] = 0;
	pptr->pnxtkin = BADPID;
	pptr->pdevs[0] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;
	//set variables for linux type scheduler 
	pptr->quantum = priority;
	pptr->counter = priority;
	pptr->interruptedInQuantum = 0;
	//pptr->counter = 0;
	pptr->goodness = 2*priority;
	pptr->executed = 0;
	if((strcmp(proctab[currpid].pname , "prnull") != 0))
		pptr->actual = 1; 
	//end

		/* Bottom of stack */
	*saddr = MAGIC;
	savsp = (unsigned long)saddr;

	/* push arguments */
	pptr->pargs = nargs;
	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process' stack	*/
	*--saddr = (long)INITRET;	/* push on return address	*/

	*--saddr = pptr->paddr = (long)procaddr; /* where we "ret" to	*/
	*--saddr = savsp;		/* fake frame ptr for procaddr	*/
	savsp = (unsigned long) saddr;




	//set smallest 


/* this must match what ctxsw expects: flags, regs, old SP */
/* emulate 386 "pushal" instruction */
	*--saddr = 0;
	*--saddr = 0;	/* %eax */
	*--saddr = 0;	/* %ecx */
	*--saddr = 0;	/* %edx */
	*--saddr = 0;	/* %ebx */
	*--saddr = 0;	/* %esp; fill in below */
	pushsp = saddr;
	*--saddr = savsp;	/* %ebp */
	*--saddr = 0;		/* %esi */
	*--saddr = 0;		/* %edi */
	*pushsp = pptr->pesp = (unsigned long)saddr;

	restore(ps);
//	if(currentEPoch == 0)
	if(( strcmp(pptr->pname , "prnull") != 0))
	{

	//	currentQuantum += priority;
		pList[actualProcessesCount++] = pid;
		for (i = 0; i < actualProcessesCount; ++i) 
        {
	    int j =0;
            for (j = i + 1; j < actualProcessesCount; ++j)
            {
                if (proctab[pList[i]].quantum > proctab[pList[j]].quantum) 
                {
                    int a1 =  pList[i];
                    pList[i] = pList[j];
                    pList[j] = a1;
                }
            }
        }
		proctab[pid].inNext = 1;
		if(actualProcessesCount == 0)
		{
			currentQuantum = priority;
			proctab[pid].inNext = 0;
		}
		if(actualProcessesCount == 1)
		{
			currentQuantum = priority;
			preempt = priority;
		//	kprintf("\n\nSetting the preempt to :%d\n",preempt);
		}
		//currentQuantum += priority;


	}
	pptr->startTime = ctr1000;
	//kprintf("%d %s\n ",(int)currentQuantum,name);
	if(schedulerType == 1 && actualProcessesCount>1 )
	{
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
        {	if(schedulerType == 1){
        	epCheck = 1;
		proctab[49].pprio = proctab[pList[1]].pprio;}
		else
		{
proctab[49].pprio = proctab[pList[1]].pprio;
		}
        //	schedulerType = 0;
        }
//kprintf("New proc priority:%d, epcheck:%d",priority, epCheck );
	}

	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL int newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}


