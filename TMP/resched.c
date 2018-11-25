//have to work on chngprio

/* resched.c  -  resched */
#define _CRT_SECURE_NO_WARNINGS
#define ALMOSTZERO 0.00000000000000000001
#define LN10 2.3025850929940456840179914546844
#define RAND_MAX 77777
#include <conf.h>

#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab1.h>
#include <q.h>

extern int epCheck ;
double lastDummy =0;
double log10(double,double);
double ln(double);
double expdev(double );
extern unsigned long ctr1000;
int edRun = 0;
int arr[3];
double dummy;
extern int clockStart;
extern int clockEnd;
int currentEPoch = 0;
int currentQuantum = 0;	//actually in create.c
int toBeAddedToQuant= 0;
int toBeDeleted = 0;
extern int startQuant;
unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
extern int schedulerType;
extern int smallestPR;
extern int smallestPRProcess;
extern int actualProcessesCount;
extern int pList[];
extern int killRunning;
extern int shouldEnd;
int nswitch = 0;
unsigned long startTime = 0;
int et  = 0;
int i,j;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
//kprintf("Coming in hereafter LS");


	//change the value of counter
	//if(proctab[currpid].pstate != PRSLEEP)
	{
		et = (int)(ctr1000- proctab[currpid].startTime);
		proctab[currpid].counter -= et;
	}
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	
	if(epCheck ==1 || schedulerType == 0 || actualProcessesCount <=1 || killRunning == 1 || currpid == 0 || proctab[currpid].pstate == PRSLEEP
		|| proctab[currpid].pstate == PRRECV || proctab[currpid].pstate == PRSUSP|| proctab[currpid].pstate == PRWAIT)
	{
//printf("Chosen, %d\n",currpid);
		killRunning = 0;
	 	if (((optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
	 		//printf("Chosen, %d\n",currpid);
	
			return(OK);

	 	}
	 	if(shouldEnd)
	 	{
	 		if (optr->pstate == PRCURR) 
	 		{
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}

			//shutdown();
	 		preempt = 10000000;
	 		nptr = &proctab[0];
	 		shouldEnd = 0;
	 		nptr->pstate = PRCURR;		/* mark it currently running	*/
	 		nptr->startTime = ctr1000;
		 	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			kprintf("\n \nReached here %s \n",nptr->pname);
	 		return (OK);

	 	}
	 	else
	 	{
			
	 		if (optr->pstate == PRCURR) 
	 		{
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
	 		nptr = &proctab[ (currpid = getlast(rdytail)) ];
	 		//printf("Chosen, %d\n",currpid);
		 	nptr->pstate = PRCURR;		/* mark it currently running	*/
	 		if(epCheck == 1)
	 		{
			 #ifdef	RTCLOCK
			 	preempt = 10;		/* reset preemption counter	*/
			 #endif
	 		}
	 		else
	 		{
			 #ifdef	RTCLOCK
			 	preempt = QUANTUM;		/* reset preemption counter	*/
			 #endif
	 		}
		 	nptr->startTime = ctr1000;
		 	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		
		}

		return OK;
	}


	//////////////EXPDEV
	else if(actualProcessesCount> 1 && proctab[currpid].pstate != PRFREE && schedulerType == 1  && strcmp(proctab[currpid].pname , "prnull") != 0)
	{
//kprintf("\n\Swapped2 arr %d\n",actualProcessesCount);
		int samePriority = proctab[pList[1]].pprio;	
		for (i = 0; i < actualProcessesCount; ++i) 
        {
        	if(pList[i]!=49 && proctab[pList[i]].pprio != samePriority)
        	{
        		samePriority = -1;
        		break;
			epCheck = 0;
        	}
        }
	
        if(samePriority != -1)
        {
        	epCheck = 1;
		proctab[49].pprio = proctab[pList[1]].pprio;
		
        }
	

	    double x1 = expdev(0.1);
	    int npid = -1;
		int index = 0;
		//swap on round robin
		int lastPid =0;
		int j = (int)(x1);
		if(x1-j>0.4)
			x1+=1;
//kprintf("\n\Swapped2 arr %d\n",actualProcessesCount);
		for(i=0;i < actualProcessesCount; i++)
		{
			
			if(proctab[pList[i]].pstate != PRFREE &&  (proctab[pList[i]].pstate == PRREADY || proctab[pList[i]].pstate == PRCURR))
			{
				if(i == actualProcessesCount-1 && proctab[pList[i]].pprio <= x1)
				{

					npid = pList[i];
					index = i;
					break;
				}
				else if (proctab[pList[i]].pprio >= x1)
				{
					npid = pList[i];
					index = i;
					break;
				}
			}
		}
		if( npid == -1)
		{
		
			optr = &proctab[currpid];
	
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
			nptr = &proctab[(currpid = pList[actualProcessesCount-1])];
		 	nptr->pstate = PRCURR;	
			
			dequeue(currpid);
			npid = currpid;

			 #ifdef	RTCLOCK
			 	preempt = 10;
			 #endif
			nptr->startTime = ctr1000;
			//kprintf("Chosen:.%d\n",currpid);
		 	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		 	return (OK);
		}
		else
		{
			//kprintf("Ciooo\n");
			//printf("x=%d, i=%d-,NP-%d%s\n",(int)x1, npid,actualProcessesCount,proctab[npid].pname );
			optr = &proctab[currpid];
			//insert(currpid,rdyhead,optr->pprio);
			nptr = &proctab[ (npid) ];
			if (optr->pstate == PRCURR)
			{
						optr->pstate = PRREADY;
						insert(currpid,rdyhead,optr->pprio);
			}
			currpid = npid;
			dequeue(npid);
			nptr->pstate = PRCURR;
			#ifdef	RTCLOCK
				preempt = 10;		/* reset preemption counter	*/
			#endif
			nptr->startTime = ctr1000;
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;
		}
	   //kprintf("Log done");
	   return (OK);
	}

	/////////////LINUX TYPE
	else if(actualProcessesCount> 1 && proctab[currpid].pstate != PRFREE && schedulerType == 2  && strcmp(proctab[currpid].pname , "prnull") != 0)
	{
		//kprintf("\n current: %d, epoch %d, \n\n\n",currentEPoch, currentQuantum);
		currentEPoch+= et;

		if(currentEPoch >= currentQuantum)
		{
			//kprintf("\n current: %d, epoch %d, \n\n\n",currentEPoch, currentQuantum);
			//kprintf("\n new epoch");
			//Add new time to Epoch, delete from epoch, inNext = 0;
			currentQuantum = 0;
			currentEPoch = 0;
			for (i = 0; i < NPROC; ++i)
			{
					//	proctab[i].quantum = proctab[i].pprio;
				if(proctab[i].pstate != PRFREE && strcmp(proctab[i].pname , "") != 0 && strcmp(proctab[i].pname , "prnull") != 0)
				{
					if(proctab[i].counter> 0 && proctab[i].inNext == 0)	
					{
						proctab[i].quantum =   ((proctab[i].counter)/2)+ proctab[i].pprio;
						proctab[i].counter =   ((proctab[i].counter)/2)+ proctab[i].pprio;
					}
					else 
					{
						proctab[i].quantum =  proctab[i].pprio;
						proctab[i].counter =  proctab[i].pprio;
					}	
					proctab[i].inNext = 0;
					currentQuantum += proctab[i].quantum;
				}
			}
			for (i = 0; i < actualProcessesCount; ++i) 
	        {
	            for (j = i ; j < actualProcessesCount; ++j)
	            {
	                if (proctab[pList[i]].quantum > proctab[pList[j]].quantum) 
	                {
	                    int a =  pList[i];
	                    pList[i] = pList[j];
	                    pList[j] = a;
	                }
	            }
	        }
			//currentEPoch =0;
			int npid = -1;
			int index = 0;
			for(i=actualProcessesCount-1;i>=0;i--)
			{
				
				if( proctab[pList[i]].inNext == 0 && proctab[currpid].pstate != PRFREE &&  (proctab[pList[i]].pstate == 1 || proctab[pList[i]].pstate == 3))
				{
					npid = pList[i];
					index = i;
					break;
				}
			}
			if(npid == -1)
			{
				npid = 0;
				currpid = npid;
				dequeue(npid);
				nptr->pstate = PRCURR;

				#ifdef	RTCLOCK
					preempt = QUANTUM;		/* reset preemption counter	*/
				#endif
				nptr->startTime = ctr1000;
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return (OK);
			}
			else
			{
				for(i=index;i>0;i--)
				{
					pList[i] = pList[i-1];
				}
				pList[0] = npid;
				optr = &proctab[currpid];
				nptr = &proctab[ (npid) ];
				if (optr->pstate == PRCURR)
				{
							optr->pstate = PRREADY;
							insert(currpid,rdyhead,optr->quantum);
				}
				currpid = npid;
				dequeue(npid);
				nptr->pstate = PRCURR;

				#ifdef	RTCLOCK
					preempt = nptr->quantum;		/* reset preemption counter	*/
				#endif
				nptr->startTime = ctr1000;
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}

			//
		}
		//switch on process quantum completion 
		{	
			//else 
			//kprintf("\nProcess:%s, counter: %d",proctab[currpid].pname,proctab[currpid].counter);
			if(proctab[currpid].pstate != PRFREE && proctab[currpid].counter <= 0)
			{

				int npid = -1;
				int index = 0;
			
				for(i=actualProcessesCount-1;i>=0;i--)
				{
					//kprintf("\n counter : %d\n ",proctab[pList[i]].pstate );
					if( proctab[pList[i]].inNext == 0 &&  (proctab[pList[i]].pstate == 1 || proctab[pList[i]].pstate == 3))
					{
						npid = pList[i];
						index = i;
						break;
					}
				}
//kprintf("\n current: %d, epoch %d, \n\n\n",currentEPoch, currentQuantum);

				if(proctab[npid].counter < 0)
					proctab[npid].counter = 0;
				//kprintf("\n Found : %d\n ",npid );
				if(npid == -1)
				{

					npid = 0;
					currpid = npid;
					dequeue(npid);
					nptr->pstate = PRCURR;
					nptr->counter = nptr->pprio;
					#ifdef	RTCLOCK
						preempt = QUANTUM;		/* reset preemption counter	*/
					#endif
					nptr->startTime = ctr1000;
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
					return (OK);
				}
				else
				{
					

					for(i=index;i>0;i--)
					{
						pList[i] = pList[i-1];
					}
					pList[0] = npid;
					optr = &proctab[currpid];
					nptr = &proctab[ (npid) ];
					if (optr->pstate == PRCURR)
					{
						optr->pstate = PRREADY;
						insert(currpid,rdyhead,optr->quantum);
					}

					currpid = npid;
					dequeue(npid);
					nptr->pstate = PRCURR;
					#ifdef	RTCLOCK
						preempt = nptr->quantum;		/* reset preemption counter	*/
					#endif
					nptr->startTime = ctr1000;
				//	kprintf(" %s %d- ",proctab[currpid].pname,proctab[currpid].counter );
				//	kprintf("\n Found : %d\n ",npid );
					for(i=0;i<1000;i++){}
					
					//kprintf("");
					//wait(1);
//kprintf("Coming in here");
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
					
return OK;
				}
				return OK;
			}
		}
	}
	//handle kernel side processes too 
	//handle null process too;
	return OK;
}

double expdev(double lambda) {
    do
  	    dummy = ((double)rand());//RAND_MAX;
    while (dummy == 0 && dummy !=lastDummy);
    lastDummy = dummy;
    dummy =dummy/RAND_MAX;

   return (-(log10(dummy,lastDummy) /(lambda)));
}

double log10( double x, double r ) {

    double h = ln(x) / LN10;
    return h;
}

double ln(double x)
{
    double old_sum = 0.0;
    double xmlxpl = (x - 1) / (x + 1);
    double xmlxpl_2 = xmlxpl * xmlxpl;
    double denom = 1.0;
    double frac = xmlxpl;
    double term = frac;
    double sum = term;
    while ( sum != old_sum )
    {
        old_sum = sum;
        denom += 2.0;
        frac *= xmlxpl_2;
        sum += frac / denom;
    }
    return 2.18 * sum;
}