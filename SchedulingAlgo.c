#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
//#include <conio.h>
#include <time.h>

/* Shameer Ahmed 16K4030
Hasnain Noorani 16K4029*/

static float EdfWaitingTime = INT_MAX;
static float RRWaitingTime = INT_MAX;
static float PSWaitingTime = INT_MAX;

void GetCompared(float rr, float edf, float ps);

typedef struct Job {
    int ID;
    unsigned int ArrivalTime;
    unsigned int Priority;
    unsigned int BurstTime;
    unsigned int RemainingTime;
    int Execution, Deadline, Period;
    int WaitingTime, TurnAroundTime;
    bool Done;
}Job;

unsigned int TimeWidth =3;
int JobsN;
Job jobs[100];
int TotalBurstTime=0;

void PrintGCStart() {
  unsigned int i;
  printf("          GANTT CHART          \n");
  printf("          ***********          \n");
  printf(" --");
  for (i = 1; i < TotalBurstTime ; i++) {
    printf("---");
  }
  printf("\n|");
}

void PrintGCEnd() {
  unsigned int i;
  printf("\n");
  for (i = 0; i < TotalBurstTime ; i++) {
    printf("---");
  }
  printf("\n");
  for (i = 0; i <= TotalBurstTime ; i++) {
    printf("%-*u", TimeWidth, i);
  }
  printf("\n\n");
}

void PrintTable(Job p[], int n)
{
    int i;
    puts("+-----+------------+--------------+-----------------+");
    puts("|  ID | Burst Time | Waiting Time | Turnaround Time |");
    puts("+-----+------------+--------------+-----------------+");
    for(i=0;i<n;i++)
    {
        printf("| %2d  |     %2d     |      %2d      |        %2d       |\n", p[i].ID, p[i].BurstTime, p[i].WaitingTime, p[i].TurnAroundTime );
        puts("+-----+------------+--------------+-----------------+");
    }
}

void PrintTablePriority(Job p[], int n)
{
    int i;
    puts("+-----+------------+----------+--------------+-----------------+");
    puts("|  ID | Burst Time | Priority | Waiting Time | Turnaround Time |");
    puts("+-----+------------+----------+--------------+-----------------+");

    for(i=0; i<n; i++) {
        printf("| %2d  |     %2d     |    %2d    |      %2d      |        %2d       |\n", p[i].ID, p[i].BurstTime,p[i].Priority, p[i].WaitingTime, p[i].TurnAroundTime );
        puts("+-----+------------+----------+--------------+-----------------+");
    }
}

/** ROUND ROBIN **/
void ScheduleRR(unsigned int tq)    //TQ is time quantum
{
	clock_t start,end;
	double TimeUsed;
    unsigned int time=0, i,x,z, next=0;
    float y, AvgWaitingTime=0, AvgTurnaroundTime=0;
    char CompChoice;
    printf("Round Robin Scheduling:\n");
    PrintGCStart();
    start=clock();
    for(i=0;i<JobsN;i++)    //Set remaining time of all jobs equal to it's burst time
    {
        jobs[i].RemainingTime = jobs[i].BurstTime;
    }
    i=0;
    while(time<TotalBurstTime)  //Check to see if 'time' counter for RR is less than total burst time of all process
    {
        if(jobs[i].ArrivalTime<=time)
        {
            if(jobs[i].RemainingTime)
            {
                z=(tq < jobs[i].RemainingTime) ? tq: jobs[i].RemainingTime;
                x=z*TimeWidth;
                printf("%*d",x-(x/2),jobs[i].ID);
                printf("%*c",x/2,'|');
                jobs[i].Done=true;
                y=(float)(time - jobs[i].ArrivalTime)/JobsN;
                AvgWaitingTime += y;
                AvgTurnaroundTime += y + (jobs[i].BurstTime/JobsN);
                time+=z;
                jobs[i].RemainingTime-=z;
            }
            i=(i+1)%JobsN;
        }
        else
        {
            i=0;
        }
    }
    PrintGCEnd();

    float TotalWTime=0, TotalTTime=0;
    int n= JobsN;
    int burst[JobsN];
    for (i=0;i<n;i++)
    {
        burst[i]=jobs[i].BurstTime;
    }
    int t=0;    //Current time
    while(1)
    {
        bool done = true;
        for(i=0;i<n;i++)
        {
            if(burst[i] > 0)
            {
                done = false;
            }
            if(burst[i] > tq)
            {
                t+=tq;
                burst[i]-=tq;
            }
            else
            {
                t=t+burst[i];
                jobs[i].WaitingTime = t-jobs[i].BurstTime;
                burst[i]=0;
            }
        }
        //If all processes are done
        if(done== true)
        {
            break;
        }
    }
    //Calculating TAT
    for(i=0;i<n;i++)
    {
        jobs[i].TurnAroundTime=jobs[i].BurstTime+jobs[i].WaitingTime;
    }
    PrintTable(jobs,JobsN);
    for(i=0;i<n;i++)
    {
        TotalWTime+=jobs[i].WaitingTime;
        TotalTTime+=jobs[i].TurnAroundTime;
    }
    printf("Average TurnAroundTime: %.2f\n",TotalTTime/JobsN);
    end=clock();
    TimeUsed = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Time Taken %lf\n",TimeUsed);
    RRWaitingTime = TimeUsed;
    printf("Do you want a comparison? (Y/N): ");
    //fflush(stdin);
    scanf("%c",&CompChoice);
    if(CompChoice == 'y' || CompChoice == 'Y')
    {
        GetCompared(RRWaitingTime,EdfWaitingTime,PSWaitingTime);
    }
}

/**Earliest Deadline First**/
void EDF(int n)
{
    int h, i, j;
    float f;
    char CompChoice;
    clock_t start,end;
    double TimeUsed;
    start = clock();
    for(i=0; i<n; i++)
    {
        jobs[i].Execution = jobs[i].BurstTime;
    }
    int help[1000];
    for(i=0;i<n;i++)
    {
        help[i]=jobs[i].Period;
    }
    h=0;
    int LowestHelp, LowestIndex;
    while(h==0)
    {
        h=1;
        LowestHelp = help[0];
        LowestIndex=0;
        for(i=0; i<n; i++)
        {
            if(LowestHelp != help[i])
            {
                h=0;
                if(LowestHelp > help[i])
                {
                    LowestHelp = help[i];
                    LowestIndex=i;
                }
            }
        }
        if(h==0)
        {
            help[LowestIndex] = LowestHelp + jobs[LowestIndex].Period;
        }
    }
    int LCMPeriod = help[0];
    printf("LCM : %d\n",LCMPeriod);

    float Condition = 0;
    float x, y;
    printf("The Sum Of: \n");
    for(i=0; i<n; i++)
    {
        x=jobs[i].Execution;
        y=jobs[i].Period;
        Condition+=(x/y);
        printf(" (%d/%d) \n",jobs[i].Execution, jobs[i].Period);
    }
    printf("is equal to %f\n", Condition);
    if(Condition > 1)
    {
        printf("Scheduling is not possible\n");
        end=clock();
    }
    else
    {
        int EarlyDeadline, DeadlineIndex, Table[LCMPeriod];
        int RemainCapacity[1000], NextDeadline[1000];
        int NewPeriod[1000];
        for(i=0; i<n; i++)
        {
            NextDeadline[i]= jobs[i].Deadline;
            RemainCapacity[i] = jobs[i].Execution;
            NewPeriod[i] = 0;
        }
        for(i=0;i<LCMPeriod;i++)
        {
            EarlyDeadline = LCMPeriod;
            DeadlineIndex-=1;
            for(j=0; j<n;j++)
            {
                if(RemainCapacity[j] > 0)
                {
                    if(EarlyDeadline > NextDeadline[j])
                    {
                        EarlyDeadline = NextDeadline[j];
                        DeadlineIndex = j;
                    }
                }
            }
            printf(" (%d,%d) : [execution %d]\n", i, i+1, DeadlineIndex);
            RemainCapacity[DeadlineIndex]--;
            for(j=0; j<n; j++)
            {
                if(NewPeriod[j] == (jobs[j].Period -1))
                {
                    NextDeadline[j] = jobs[j].Deadline;
                    RemainCapacity[j] = jobs[j].Execution;
                    NewPeriod[j]=0;
                }
                else
                {
                    if(NextDeadline[j]>0)
                    {
                        NextDeadline[j]--;
                    }
                    NewPeriod[j]++;
                }
            }
        }
        end = clock();
        TimeUsed=((double)(end-start))/CLOCKS_PER_SEC;
        printf("Average Time: %lf\n",TimeUsed);
    }
    printf("Do you want a comparison? (Y/N): ");
    fflush(stdin);
    scanf("%c",&CompChoice);
    if(CompChoice == 'y' || CompChoice == 'Y')
    {
        GetCompared(RRWaitingTime, EdfWaitingTime,PSWaitingTime);
    }
}


/*Priority Scheduling*/
void SchedulePriority()
{
	unsigned int time=0, i, j, x, next=0, nextMin;
	float y, AvgWaitingTime=0, AvgTurnAroundTime=0;
	clock_t start,end;
	double TimeUsed;
	printf("Priority Scheduling: \n");
	start= clock();
	for(i=0; i<JobsN; i++)
	{
		jobs[i].Done = false;
	}
	for(i=0;i<JobsN;i++)
	{
		nextMin = INT_MAX;
		for(j=0; jobs[j].ArrivalTime <= time && j < JobsN; j++)
		{
			if(!jobs[j].Done && jobs[j].Priority<nextMin)
			{
				next=j;
				nextMin = jobs[next].Priority;
			}
		}
		jobs[next].Done = true;
		y= (float)(time-jobs[next].ArrivalTime)/JobsN;
		AvgWaitingTime+=y;
		AvgTurnAroundTime+=y+(jobs[next].BurstTime/JobsN);
		time+=jobs[next].BurstTime;
	}
	int n= JobsN;
	Job temp;
	for(i=0;i<n-1;i++)
	{
		for(j=0;j<n-1;j++)
		{
			if(jobs[j].Priority > jobs[j+1].Priority)
			{
				temp = jobs[j];
				jobs[j]=jobs[j+1];
				jobs[j+1]=temp;
			}
		}
	}
	float TotalWaitingTime=0, TotalTurnaroundTime;
	jobs[0].WaitingTime = 0;
	jobs[0].TurnAroundTime = jobs[0].BurstTime;
	for(i=1; i<n; i++)
	{
		jobs[i].WaitingTime = jobs[i-1].WaitingTime + jobs[i-1].BurstTime;
		TotalWaitingTime+=jobs[i].WaitingTime;
	}
	for(i=1;i<n;i++)
	{
		jobs[i].TurnAroundTime = jobs[i].WaitingTime+jobs[i].BurstTime;
		TotalTurnaroundTime+=jobs[i].TurnAroundTime;
	}
	TotalTurnaroundTime+=jobs[0].TurnAroundTime;
	PrintTablePriority(jobs,JobsN);
	printf("Average Turnaround time: %.2f\n",AvgTurnAroundTime);
	end=clock();
	TimeUsed = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Time Taken %lf\n",TimeUsed);
	PSWaitingTime = TimeUsed;
	char CompChoice;
	printf("Do you want a comparison? (Y/N): ");
    fflush(stdin);
    scanf("%c",&CompChoice);
    if(CompChoice == 'y' || CompChoice == 'Y')
    {
        GetCompared(RRWaitingTime, EdfWaitingTime,PSWaitingTime);
    }
}
int main(int argc, char const *argv[])
{
   int i, tq = 3;
  unsigned int ch;
  printf("Enter Total Number of Processes: ");
  scanf("%u", &JobsN);
  for (i = 0; i < JobsN; i++) 
  {
	jobs[i].ID = i;
	printf("  Burst Time for P[%u]: ", jobs[i].ID);
    scanf("%u", &jobs[i].BurstTime);
    jobs[i].RemainingTime = jobs[i].BurstTime;
    jobs[i].Done = false;
    TotalBurstTime += jobs[i].BurstTime;
  }
  for (i=0; i<JobsN; i++)
  	jobs[i].ArrivalTime = 0;

  ch = true;
  while(ch) {
    printf("Select a Scheduling:\n");
    printf("1: RR (Round Robin)\n");
    printf("2: EDF (Earliest Deadline First)\n");
    printf("3: PS (Priority Scheduling)\n");
    printf("0: Exit\n");
    printf("\nEnter your choice: ");
    fflush(stdin);
    scanf("%u", &ch);
    printf("\n");
    switch(ch) {
        case 1:
            printf("Enter the value of time quantum: \n");
            scanf("%u", &tq);
            ScheduleRR(tq);
            break;
        case 2:
            for (i=0; i< JobsN; i++)
            {
                printf("  Deadline for P[%u]: ", jobs[i].ID);
                scanf("%u", &jobs[i].Deadline);
            }
            for (i=0; i< JobsN; i++)
            {
                printf("  Period for P[%u]: ", jobs[i].ID);
                scanf("%u", &jobs[i].Period);
            }
            EDF(JobsN);
            break;
        case 3:
        	for(i=0; i<JobsN; i++)
        	{
        		printf("Priority for P[%u]: ",jobs[i].ID);
        		scanf("%u",&jobs[i].Priority);
			}
			SchedulePriority();
        	break;
        case 0:
            printf("Thank You...\n");
            break;
        default:
            printf("Invalid Choice!\n");
    }
    printf("\n\n");
  }
  return 0;
}

void GetCompared(float RR, float EDF, float PS)
{
	printf("Optimal in this case: ");
	if (RR<=EDF)
    {
        if(RR<=PS)
            printf("RR is optimal\n");
        else
            printf("PS Is optimal\n");
    }
    else
    {
        if(EDF<=PS)
            printf("EDF Is optimal\n");
        else
            printf("PS is Optimal\n");
    }
}
