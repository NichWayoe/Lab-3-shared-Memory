#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// Nicholas Wayoe
// @02903830

int genRandonNumber(int lower, int upper)
{
    return ((random() % (upper - lower)) + lower);
}

int depositMoney(int account)
{
    int balance = genRandonNumber(0, 100);
    if (balance % 2 == 0)
    {
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account+balance);
        return balance;
    }
    else
    {
        printf("Dear old Dad: Doesn't have any money to give\n");
        return 0;
    }
}

void childProcess(int SharedMem[])
{
    sleep(genRandonNumber(0, 5));
    int account = SharedMem[0];
    while (SharedMem[1] != 1)
    {
    };
    int balance = genRandonNumber(0, 50);
    if (balance <= account)
    {
        account -= balance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        SharedMem[0] = account;
    }
    else
    {
        printf("Poor Student: Not Enough Cash ($%d)\n", account);
    }
    SharedMem[1] = 0;
}

int main(int argc, char *argv[])
{
    srandom(time(0));
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0)
    {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }

    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1)
    {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");
    ShmPTR[0] = 0;
    ShmPTR[1] = 0;
    pid = fork();
    if (pid < 0)
    {
        printf("*** fork error (server) ***\n");
        exit(1);
    }
    else if (pid == 0)
    {
        for (int i = 0; i < 25; i++)
        {
          
            while (ShmPTR[1] != 1)
            {
            };
//             childProcess(ShmPTR);
            sleep(genRandonNumber(0, 5));
            int account = ShmPTR[0];
            
            int balance = genRandonNumber(0, 50);
            if (balance <= account)
            {
                account -= balance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
                ShmPTR[0] = account;
            }
            else
            {
                printf("Poor Student: Not Enough Cash ($%d)\n", account);
            }
            ShmPTR[1] = 0;
        }
        exit(0);
    }
    else
    {
        for (int i = 0; i < 25; i++)
        {
            while (ShmPTR[1] != 0)
            {
            };
            sleep(genRandonNumber(0, 5));
            int account = ShmPTR[0];
          
            if (account <= 100)
            {
                account += depositMoney(account);
            }
            else
            {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
            }
            ShmPTR[0] = account;
            ShmPTR[1] = 1;
        }
    }
    wait(&status);
    printf("Server has detected the completion of its child...\n");
    shmdt((void *) ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}
