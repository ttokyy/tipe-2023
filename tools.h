#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

extern const double pi;

int min (int a, int b);
int max (int a, int b);
double min_double (double a, double b);
double max_double (double a, double b);
double RAND_VAR_HALF_NORMAL_DISTRIBUTION (double Peak, double StdDeviation);

#endif