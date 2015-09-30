#include "General.h"
#include "cmath"

long double General::exponential(long double L) {
    return (long double) - log(1 - uniform(0, 1)) / L;
}

long double General::uniform(long double xmin, long double xmax) {
    long double va;

    do {
        va = (long double) rand() / (RAND_MAX);
        va += ((long double) rand() / (RAND_MAX)) / (RAND_MAX);
        va += (((long double) rand() / (RAND_MAX)) / (RAND_MAX)) /
              (RAND_MAX);
    } while ((va <= 0) || (va >= 1.0 - pow(RAND_MAX, -3)));

    return xmin + va * (xmax - xmin);
}

