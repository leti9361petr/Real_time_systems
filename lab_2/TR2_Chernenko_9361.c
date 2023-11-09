/*
 * Выполнил: Черненко Петр, 9361
 * Задание: Т2 Вызов лифта
 * Дата выполнения: 14.10.2023
 * Версия: 0.1
 *
 * Скрипт для компиляции и запуска программы:
 * gcc TR2_Chernenko_9361.c -o t_2
 * ./t_2
 */
// ------------------------------------------- //
/*
 * Общее описание программы:
 * Данная программа имитирует модель работы автомата, управляющего лифтом при его вызове с некоторого этажа.
 */
// ------------------------------------------- //

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define    TOP            0  // лифт вверху
#define    BOTTOM         1  // лифт внизу
#define    THIS_FLOOR     2  // лифт на нашем этаже

#define    Up	      48  // Сигналы реального времени
#define    Down	      49

void move(int direction) {
    int n = 3;
    for (int i = 1; i <= n; i++) {
        if (direction == Up) {
            printf("*** Go UP\n");
        } else if (direction == Down) {
            printf("*** Go DOWN\n");
        }
        usleep(500000);
    }
}

void lift_control(int signo, siginfo_t* info, void* nk) {
    int* state = (int*)info->si_value.sival_ptr;

    switch(*state) {
        case THIS_FLOOR:
            if(signo == Up) {
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Up);
                *state = TOP;
            }
            if(signo == Down) {
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Down);
                *state = BOTTOM;
            }
            break;
        case TOP:
            if(signo == Up) {
                move(Down);
                *state = THIS_FLOOR;
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Up);
                *state = TOP;
            }
            if(signo == Down) {
                move(Down);
                *state = THIS_FLOOR;
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Down);
                *state = BOTTOM;
            }
            break;
        case BOTTOM:
            if(signo == Up) {
                move(Up);
                *state = THIS_FLOOR;
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Up);
                *state = TOP;
            }
            if(signo == Down) {
                move(Up);
                *state = THIS_FLOOR;
                printf("This Floor, door is opened \n");
                sleep(1);
                move(Down);
                *state = BOTTOM;
            }
            break;
        default:
            printf("ERROR: Unknown floor: %d! \n", *state);
            break;
    }
}

void* push_button(void* args) {
    char ch = 0;

    sigset_t set = *(sigset_t*)args;

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = &lift_control;
    sa.sa_flags = SA_SIGINFO;
    sigaction(Up, &sa, NULL);
    sigaction(Down, &sa, NULL);

    sa.sa_mask = set;
    sigprocmask(SIG_UNBLOCK, &set, NULL);

    int current_floor = THIS_FLOOR;
    union sigval s_value;
    s_value.sival_ptr = &current_floor;

    while(ch != 'q') {
        ch = getchar();
        switch(ch) {
            case 'u':
                sigqueue(getpid(), Up, s_value);
                break;
            case 'd':
                sigqueue(getpid(), Down, s_value);
                break;
            case 'q':
                break;
            case '\n':
                break;
            default:
                printf("ERROR: Unknown signal: %c! \n", ch);
                break;
        }
    }

    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    pthread_t t;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, Up);
    sigaddset(&set, Down);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("Start\n");

    pthread_create(&t, NULL, &push_button, (void*)&set);
    pthread_join(t, NULL);

    printf("Finish\n");
    return EXIT_SUCCESS;
}

// ----------------------------------------//
/*
Start
u
This Floor, door is opened
*** Go UP
*** Go UP
*** Go UP
u
*** Go DOWN
*** Go DOWN
*** Go DOWN
This Floor, door is opened
*** Go UP
*** Go UP
*** Go UP
d
*** Go DOWN
*** Go DOWN
*** Go DOWN
This Floor, door is opened
*** Go DOWN
*** Go DOWN
*** Go DOWN
d
*** Go DOWN
*** Go DOWN
*** Go DOWN
This Floor, door is opened
*** Go DOWN
*** Go DOWN
*** Go DOWN
q
Finish
*/
