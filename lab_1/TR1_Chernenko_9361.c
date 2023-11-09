/*
 * Выполнил: Черненко Петр, 9361
 * Задание: Т1 xtnf nfv, [p
 *  Z ghjt,fkcz b yfxfk gbcfnm dnjhe. kf,e nen? nfr xnj ghbltncz cyfxfkf bcghfdbnm gfhe rjczrjd? xnj,s pfgecnbnm gthde. kf,e
 * Дата выполнения: 14.10.2023
 * Версия: 0.1
 *
 * Скрипт для компиляции и запуска программы:
 * gcc TR1_Chernenko_9361.c -o t_1
 * ./t_1
 */
// ------------------------------------------- //
/*
 * Общее описание программы:
 * Данная программа имитирует модель работы автомата, управляющего лифтом при его вызове с некоторого этажа.
 */
// ------------------------------------------- //

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void alarmHandler() {
    printf("#####-- Restart required! --#####\n");
    kill(getpid(), SIGUSR1); // Посылаем сигнал SIGUSR1 для завершения работы системы
}

void doControl() {
    int t;
    int dt = rand() % 1000000; // Случайное число до 1 секунды (1000000 микросекунд)
    int x = 1;

    t = 500000 + dt;
    printf("########## %d\n", t);

    if (x == 0) {
        while (1); // Имитация "зависания" системы
    }

    usleep(t);
}

int main() {
    srand(time(NULL));
    signal(SIGALRM, alarmHandler); // Установка обработчика сигнала SIGALRM
    signal(SIGUSR1, SIG_IGN); // Игнорируем сигнал SIGUSR1 на данном этапе
    int delay_time = 0.5;
    double expected_time = 0.5;
    double soft_dt = 0.02;
    double hard_dt = 1;
    while (1) {
        alarm(0); // Сброс таймера
        alarm(hard_dt); // Установка жесткого дедлайна

        time_t start_time = time(NULL); // Время начала работы функции

        doControl(); // Выполняем функцию управления

        time_t end_time = time(NULL); // Время окончания работы функции
        double execution_time = difftime(end_time, start_time); // Вычисляем время выполнения

        if (execution_time > (expected_time + soft_dt)) {
            deadlineHandler(); // Если превышен "мягкий" дедлайн, вызываем обработчик
        } else {
            printf("#### Deadline NOT exceeded ####\n");
        }

        sleep(delay_time);
    }

    return 0;
}

// ----------------------------------------//
/*
########## 1429143
#####-- Restart required! --#####
#### Deadline exceeded ####
########## 611877
#### Deadline exceeded ####
########## 851610
#### Deadline exceeded ####
########## 758430
#### Deadline NOT exceeded ####
########## 1365741
#####-- Restart required! --#####
#### Deadline exceeded ####
*/
