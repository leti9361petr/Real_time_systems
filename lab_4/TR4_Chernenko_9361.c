/*
 * Выполнил: Черненко Петр, 9361
 * Задание: Т4 Циклическая система управления
 * Дата выполнения: 26.11.2023
 * Версия: 0.1
 *
 * Скрипт для компиляции и запуска программы:
 * gcc TR4_Chernenko_9361.c -o t_4
 * ./t_4
 */
// ------------------------------------------- //
/*
 * Общее описание программы:
 * Для управления некоторым объектом необходимо
 * обеспечить периодическое выполнение трех
 * независимых управляющих процедур.  Периоды
 * вызова процедур должны составлять 500, 750
 * и 1000 миллисекунд  соответственно, и запуск
 * каждой из них следует осуществлять от
 * периодических программных таймеров,
 * используя сигналы в качестве схемы
 * уведомления. При каждом выполнении процедуры
 * должен отображаться момент ее текущего
 * вызова и время, прошедшее с момента
 * предыдущего вызова
 */
// ------------------------------------------- //

#include <stdio.h>
#include <time.h>
#include <signal.h>

#define MY_SIGNAL        SIGUSR2  // SIGUSR1   SIGALRM

// somehow it is not implemented by default
#define SIGEV_SIGNAL_INIT(__e, __s)			 \
        ((__e)->sigev_notify = SIGEV_SIGNAL, \
        (__e)->sigev_signo = (__s))

time_t currentTime;
struct tm* timeInfo;

void pretty_print_time(const struct tm* T, clock_t S) {
    printf("%02d:%02d:%02.3f", T->tm_hour, T->tm_min, ((float)(S % 1000)/1000) + (float)(T->tm_sec % 60));
}

void print_debug(const char* message) {
    currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
    float seconds = ((float)(clock() % 1000)/1000) + (float)(timeInfo->tm_sec % 60);
    printf("%02d:%02d:%02.3f", timeInfo->tm_hour, timeInfo->tm_min, seconds);
}

void doControl_1(int signal_n) {
    static struct tm*  last_;
    printf(" Procedure # 1");
    [<момент_вызова>] %f ms, [<период>] %f ms”, . . .);
}
void set_periodic_timer_fields(struct itimerspec* timer, size_t sec, size_t ms) {
    timer->it_value.tv_sec = sec;
    timer->it_value.tv_nsec = ms * 1000000;
    timer->it_interval.tv_sec = sec;
    timer->it_interval.tv_nsec = ms * 1000000;
}

int main() {
    struct sigevent         event;
    struct itimerspec       itime;
    timer_t                 timer_id;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, MY_SIGNAL);

    struct sigaction act;
    act.sa_handler = &catch_sig;
    act.sa_flags = 0;
    act.sa_mask = set;
    sigaction(MY_SIGNAL, &act, NULL);

    /*
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = MY_SIGNAL;
    */

    int signo = MY_SIGNAL;
    SIGEV_SIGNAL_INIT(&event, signo);

    timer_create(CLOCK_REALTIME, &event, &timer_id);

    set_periodic_timer_fields(&itime, 0, 500);
    timer_settime(timer_id, 0, &itime, NULL);

    for (;;) {
    }
}

// ----------------------------------------//
/*
TODO: add output example here
*/
