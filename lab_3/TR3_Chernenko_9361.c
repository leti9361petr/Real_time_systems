/*
 * Выполнил: Черненко Петр, 9361
 * Задание: Т3 Поставщик – Потребитель
 * Дата выполнения: 09.11.2023
 * Версия: 0.2
 *
 * Скрипт для компиляции и запуска программы:
 * gcc -DSEMAPHORE_VER=0 TR3_Chernenko_9361.c -o t_3_cond
 * ./t_3_cond
 * gcc -DSEMAPHORE_VER=1 TR3_Chernenko_9361.c -o t_3_sem
 * ./t_3_sem
 */
// ------------------------------------------- //
/*
 * Общее описание программы:
 * Два потока  -  «Поставщик» и «Потребитель» - работают с некоторой общей целочисленной переменной Buffer,
 * при этом каждое обращение «Поставщика» увеличивает значение Buffer на 1, а  «Потребителя» - уменьшает.
 */
// ------------------------------------------- //

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#ifndef SEMAPHORE_VER
#define SEMAPHORE_VER 0
#endif

const int full_buffer = 2;
const int consumer_time = 3;
const int supplier_time = 2;
struct timespec T;

time_t currentTime;
struct tm* timeInfo;

int Buffer = 0;

void print_debug(const char* message) {
    currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
    float seconds = ((float)(clock() % 1000)/1000) + (float)(timeInfo->tm_sec % 60);
    printf("%02d:%02d:%02.3f: %s\n", timeInfo->tm_hour, timeInfo->tm_min, seconds, message);
}

#if SEMAPHORE_VER
sem_t bin_semaphore, cons_sem, suppl_sem;

void* Consumer (void* args) {
    int* consume = (int*)args;
    while (*consume) {
        sem_wait(&cons_sem);
        print_debug("Consumer tries to get some goods");

        sem_wait(&bin_semaphore);
        printf("There's %d goods for consumer\n", Buffer);

        // «Критическая секция»
        if (Buffer > 0) {
            Buffer--;
            print_debug("Consumer got some goods");
        } else {
            print_debug("Error occurred while consuming!");
            sem_post(&bin_semaphore);
            sem_post(&suppl_sem);
            return (void*)EXIT_FAILURE;
        }

        sem_post(&bin_semaphore);
        sem_post(&suppl_sem);
        sleep(consumer_time);
    }
    return EXIT_SUCCESS;
}

void* Supplier (void* args) {
    int* supply = (int*)args;
    while (*supply) {
        sem_wait(&suppl_sem);
        print_debug("Supplier tries to bring some goods");

        sem_wait(&bin_semaphore);
        printf("There's %d goods out of %d\n", Buffer, full_buffer);

        // «Критическая секция»
        if (Buffer < full_buffer) {
            Buffer++;
            print_debug("Supplier brought some goods");
        } else {
            print_debug("Error occurred while supplying!");
            sem_post(&bin_semaphore);
            sem_post(&cons_sem);
            return (void*)EXIT_FAILURE;
        }

        sem_post(&bin_semaphore);
        sem_post(&cons_sem);
        sleep (supplier_time);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    printf("Max goods at storage: %d\n", full_buffer);
    pthread_t thread_Cons, thread_Sup;
    int consume = 1, supply = 1;
    sem_init(&bin_semaphore, 0, 1);
    sem_init(&cons_sem, 0, 0);
    sem_init(&suppl_sem, 0, full_buffer);
    pthread_create(&thread_Cons, NULL, &Consumer, (void*)(&consume));
    pthread_create(&thread_Sup, NULL, &Supplier, (void*)(&supply));

    getchar();
    consume = 0;
    supply = 0;

    pthread_join(thread_Cons, NULL);
    pthread_join(thread_Sup, NULL);
    sem_destroy(&bin_semaphore);
    sem_destroy(&cons_sem);
    sem_destroy(&suppl_sem);
    return EXIT_SUCCESS;
}
#else
pthread_cond_t cond;
pthread_mutex_t mutex;

void* Consumer (void* args) {
    int* consume = (int*)args;
    while (*consume) {
        print_debug("Consumer tries to get some goods");
        pthread_mutex_lock(&mutex);
        printf("There's %d goods\n", Buffer);

        // «Критическая секция»
        if (Buffer > 0) {
            Buffer--;
            print_debug("Consumer got some goods");
            if (Buffer == full_buffer - 1) {
                pthread_cond_signal(&cond);
            }
            pthread_mutex_unlock(&mutex);
            sleep(consumer_time);
        } else {
            print_debug("There's no goods for consumer yet");
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_cond_signal(&cond);
    return EXIT_SUCCESS;
}

void* Supplier (void* args) {
    int* supply = (int*)args;
    while (*supply) {
        print_debug("Supplier tries to bring some goods");
        pthread_mutex_lock(&mutex);
        printf("There's %d goods\n", Buffer);

        // «Критическая секция»
        if (Buffer < full_buffer) {
            Buffer++;
            print_debug("Supplier brought some goods");
            if (Buffer == 1) {
                pthread_cond_signal(&cond);
            }
            pthread_mutex_unlock(&mutex);
            sleep (supplier_time);
        } else {
            print_debug("There's no place for goods at the storage yet");
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_cond_signal(&cond);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    printf("Max goods at storage: %d\n", full_buffer);
    if (full_buffer == 0) {
        // to avoid situations when condition is false for both consumer and supplier
        printf("Storage size must be greater than 0\n");
        return 0;
    }
    pthread_t thread_Cons, thread_Sup;
    int consume = 1, supply = 1;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&thread_Cons, NULL, &Consumer, (void*)(&consume));
    pthread_create(&thread_Sup, NULL, &Supplier, (void*)(&supply));

    getchar();
    consume = 0;
    supply = 0;

    pthread_join(thread_Cons, NULL);
    pthread_join(thread_Sup, NULL);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
#endif

// ----------------------------------------//
/*
Max goods at storage: 2
03:36:2.941: Supplier tries to bring some goods
There's 1 goods
03:36:2.955: Supplier brought some goods
03:36:2.999: Consumer tries to get some goods
There's 2 goods
03:36:2.025: Consumer got some goods
03:36:4.093: Supplier tries to bring some goods
There's 1 goods
03:36:4.152: Supplier brought some goods
03:36:5.298: Consumer tries to get some goods
There's 2 goods
03:36:5.399: Consumer got some goods
03:36:6.555: Supplier tries to bring some goods
There's 1 goods
03:36:6.658: Supplier brought some goods
03:36:8.923: Supplier tries to bring some goods
There's 2 goods
03:36:8.982: There's no place for goods at the storage yet
03:36:8.049: Consumer tries to get some goods
There's 2 goods
03:36:8.128: Consumer got some goods
03:36:10.241: Supplier tries to bring some goods
There's 1 goods
03:36:10.304: Supplier brought some goods
*/