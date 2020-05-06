#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define SEM_NAME_WRITE "xpupak01.ios.sem_write.sem"
#define SEM_NAME_ENTER "xpupak01.ios.sem_enter.sem"
#define SEM_NAME_CONFIRM "xpupak01.ios.sem_confirm.sem"

// Bless IJC and macros!
#define MAPIT(pointer) pointer = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
#define UNMAPIT(pointer) munmap(pointer, sizeof(int));

// God please be with me while I do this
int pi = 0;
int ig = 0;
int jg = 0;
int it = 0;
int jt = 0;

FILE *file = NULL;

// Shared
int *nb = NULL;
int *nc = NULL;
int *ne = NULL;
int *rest = NULL;
int *migRest = NULL;
int *a = 0;

// Semaphores
sem_t *sem_write;
sem_t *sem_enter;
sem_t *sem_confirm;

void randomSleep(int time) {
    if (time != 0) {
        usleep((rand() % time) * 1000);
    }
}

void close_sems() {
    sem_close(sem_write);
    sem_close(sem_enter);
    sem_close(sem_confirm);
}

void unlink_sems() {
    sem_unlink(SEM_NAME_WRITE);
    sem_unlink(SEM_NAME_ENTER);
    sem_unlink(SEM_NAME_CONFIRM);
}

int init() {
    // Get some randomness into it!
    srand(time(0));
    // Let's get those shared bytes
    MAPIT(rest)
    MAPIT(migRest);
    MAPIT(nb)
    MAPIT(nc)
    MAPIT(ne)
    MAPIT(a)
    *rest = pi;
    *migRest = pi;
    // And also some of those, eh, files
    file = fopen("proj2.out", "w");
    // Did you clean up after yousrself? Or do I have to do it now? :sadcat:
    if ((sem_write = sem_open(SEM_NAME_WRITE, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        return -1;
    }
    if ((sem_enter = sem_open(SEM_NAME_ENTER, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        return -1;
    }
    if ((sem_confirm = sem_open(SEM_NAME_CONFIRM, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) {
        return -1;
    }
    return 0;
}

void cleanup() {
    // Forget it, don't need it anymore
    UNMAPIT(nb)
    UNMAPIT(nc)
    UNMAPIT(ne)
    UNMAPIT(a)
    UNMAPIT(rest)
    UNMAPIT(migRest)
    // What could this do? Close something?
    close_sems();
    // Also forget this plz
    unlink_sems();
}

void imigrant_be_imigrant(int id) {
    // started
    sem_wait(sem_write);
        (*a)++;
        fprintf(file, "%d: IMM %d: starts \n", *a, id);
        fflush(file);
    sem_post(sem_write);

    // enter
    sem_wait(sem_enter);
        sem_wait(sem_write);
            (*a)++;
            (*ne)++;
            (*nb)++;
            fprintf(file, "%d: IMM %d: enters: %d: %d: %d \n", *a, id, *ne, *nc, *nb);
            fflush(file);
        sem_post(sem_write);
    sem_post(sem_enter);
    // register
    sem_wait(sem_write);
        (*a)++;
        (*nc)++;
        fprintf(file, "%d: IMM %d: checks: %d: %d: %d \n", *a, id, *ne, *nc, *nb);
        fflush(file);
    sem_post(sem_write);
    // wait for judge
    sem_wait(sem_confirm);
    // get certificate
    sem_wait(sem_write);
        (*a)++;
        fprintf(file, "%d: IMM %d: wants certificate: %d: %d: %d \n", *a, id, *ne, *nc, *nb);
        fflush(file);
    sem_post(sem_write);
    randomSleep(it);
    sem_wait(sem_write);
        (*a)++;
        fprintf(file, "%d: IMM %d: got certificate: %d: %d: %d \n", *a, id, *ne, *nc, *nb);
        fflush(file);
    sem_post(sem_write);
    // leave
    sem_wait(sem_enter);
        sem_wait(sem_write);
            (*a)++;
            (*nb)--;
            fprintf(file, "%d: IMM %d: leaves: %d: %d: %d \n", *a, id, *ne, *nc, *nb);
            fflush(file);
            (*migRest)--;
        sem_post(sem_write);
    sem_post(sem_enter);
    exit(0);
}

void judge_be_judge() {
    while (1) {
        while(*ne == 0) {}
        // Get ready
        randomSleep(jg);
        sem_wait(sem_write);
            (*a)++;
            fprintf(file, "%d: JUDGE: wants to enter \n", *a);
            fflush(file);
        sem_post(sem_write);
        // enter
        sem_wait(sem_enter);
        sem_wait(sem_write);
            (*a)++;
            fprintf(file, "%d: JUDGE: enters: %d: %d: %d \n", *a, *ne, *nc, *nb);
            fflush(file);
        sem_post(sem_write);
        // confirm

        int i = 0;
        while (1) {
            sem_wait(sem_write);
                if (*ne != *nc) {
                    sem_post(sem_write);
                    break;
                }
            sem_post(sem_write);
            if (i == 0) {
                sem_wait(sem_write);
                (*a)++;
                fprintf(file, "%d: JUDGE: waits for imm: %d: %d: %d \n", *a, *ne, *nc, *nb);
                fflush(file);
                sem_post(sem_write);
                i++;
            }
        }
        sem_wait(sem_write);
            (*a)++;
            fprintf(file, "%d: JUDGE:starts confirmation: %d: %d: %d \n", *a, *ne, *nc, *nb);
            fflush(file);
        sem_post(sem_write);
        randomSleep(jt);
        sem_wait(sem_write);
            int help = *nc;
            (*a)++;
            (*ne) = 0;
            (*nc) = 0;
            fprintf(file, "%d: JUDGE: ends confirmation: %d: %d: %d \n", *a, *ne, *nc, *nb);
            fflush(file);
        sem_post(sem_write);
        for (int i = 0; i < help; i++) {
            sem_post(sem_confirm);
        }
        // leave
        randomSleep(jt);
        sem_wait(sem_write);
            (*a)++;
            fprintf(file, "%d: JUDGE: leaves: %d: %d: %d \n", *a, *ne, *nc, *nb);
            fflush(file);
        sem_post(sem_write);
        sem_post(sem_enter);

        // check if finished
        (*rest) -= help;
        if (*rest == 0) {
            break;
        }
    }
    sem_wait(sem_write);
        (*a)++;
        fprintf(file, "%d: JUDGE: finishes \n", *a);
        fflush(file);
    sem_post(sem_write);
    exit(0);
}

void imigrant_spawner() {
    for (int i = 0; i < pi; ++i) {
        randomSleep(ig);
        pid_t RID = fork();
        if (RID == 0) {
            imigrant_be_imigrant(i + 1);
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    // Parsing
    if (argc != 6) {
        printf("%s", "ERROR - Wrong number of arguments\n");
        return 0;
    }

    // I sure do hope god is with me
    int iterator = 1;
    while (iterator < argc) {
        long x = 0;
        switch (iterator) {
            case 1:
                x = strtol(argv[iterator], NULL, 10);
                if (x <= 0) {
                    printf("%s", "ERROR - Wrong number of IP\n");
                    return 1;
                } else if (x <= INT_MAX) {
                    pi = (int)x;
                } else {
                    printf("%s", "ERROR - Way to many IP\n");
                    return 1;
                }
                break;
            case 2:
                x = strtol(argv[iterator], NULL, 10);
                if (x < 0 || x > 2000) {
                    printf("%s", "ERROR - Wrong IG\n");
                    return 1;
                } else if (x <= INT_MAX) {
                    ig = (int)x;
                } else {
                    printf("%s", "ERROR - IG error\n");
                    return 1;
                }
                break;
            case 3:
                x = strtol(argv[iterator], NULL, 10);
                if (x < 0 || x > 2000) {
                    printf("%s", "ERROR - Wrong JG\n");
                    return 1;
                    // Even though I am an atheist
                } else if (x <= INT_MAX) {
                    jg = (int)x;
                } else {
                    printf("%s", "ERROR - JG error\n");
                    return 1;
                }
                break;
            case 4:
                x = strtol(argv[iterator], NULL, 10);
                if (x < 0 || x > 2000) {
                    printf("%s", "ERROR - Wrong IT\n");
                    return 1;
                } else if (x <= INT_MAX) {
                    it = (int)x;
                } else {
                    printf("%s", "ERROR - IT error\n");
                    return 1;
                }
                break;
            case 5:
                x = strtol(argv[iterator], NULL, 10);
                if (x < 0 || x > 2000) {
                    printf("%s", "ERROR - Wrong JT\n");
                    return 1;
                } else if (x <= INT_MAX) {
                    jt = (int)x;
                } else {
                    printf("%s", "ERROR - JT error\n");
                    return 1;
                }
                break;
        }
        iterator++;
    }

    printf("%d, %d, %d, %d, %d\n", pi, ig, jg, it, jt);

    // Arguments were parsed, let's try to do some magic
    int in = init();
    if (in != 0) {
        cleanup();
        init();
    }

    pid_t RID = fork();
    if (RID == 0) {
        imigrant_spawner();
    }

    RID = fork();
    if (RID == 0) {
        judge_be_judge();
    }

    while (*migRest != 0) {}
    cleanup();
    return 0;
}

