#include "log.hpp"

void CONSOLE_LOG(char *argtype, ...) {
    struct tm* ptr;
    time_t t;
    t = time(NULL);
    ptr = gmtime(&t);

    char *times = asctime(ptr);
    times[strlen(times) - 1] = '\0';
    
    printf("UTC+0 %s: ", times);

    va_list list;

    va_start(list, argtype);

    for (int i = 0; argtype[i] != '\0'; i++) {
        if (argtype[i] != '%') {
            printf("%c", argtype[i]);
        } else {
            i++;
            if (argtype[i] == 'd') {
                printf(" %d", va_arg(list, int));
            } else if (argtype[i] == 'f') {
                printf(" %f", va_arg(list, float));
            } else if (argtype[i] == 'c') {
                printf(" %c", va_arg(list, char));
            } else if (argtype[i] == 's') {
                printf(" %s", va_arg(list, char *));
            } else if (argtype[i] == 'l' && argtype[i + 1] == 'f') {
                printf(" %lf", va_arg(list, double));
            } else if (argtype[i] == 'l' && argtype[i + 1] == 'd') {
                printf(" %ld", va_arg(list, long int));
            } else if (argtype[i] == 'l' && argtype[i + 1] == 'l' && argtype[i + 2] == 'd') {
                printf(" %lld", va_arg(list, long long));
            } 
        }
    }
    printf("\n");
}