#include "tools.h"


const double pi = 3.141592654;


int min (int a, int b){
	if (a < b) return a;
	else return b;
}

int max (int a, int b){
	if (a > b) return a;
	else return b;
}

double min_double (double a, double b){
	if (a < b) return a;
	else return b;
}

double max_double (double a, double b){
	if (a > b) return a;
	else return b;
}

double abs_double (double x){
	if (x >= 0) return x;
	else return -x;
}

// generates a random float in ]-inf,Peak] according to a left half-normal distribution
// with mean Peak and standard deviation StdDeviation
double RAND_VAR_HALF_NORMAL_DISTRIBUTION (double Peak, double StdDeviation){
	double Unif1, Unif2;
	double SumSq = 1;
	// generate Unif1 and Unif2 values following uniform distribution on [-1,1[
	while (SumSq >= 1){
		Unif1 = 2*((double)rand() / (double)RAND_MAX)-1;
		Unif2 = 2*((double)rand() / (double)RAND_MAX)-1;
		SumSq = Unif1*Unif1 + Unif2*Unif2;
	}
	// NormalStandardized follows a normal distribution of mean 0 and variance 1
	double NormalStandardized = Unif1*sqrt((-2*log(SumSq))/SumSq);
	return -StdDeviation*(abs_double(NormalStandardized)) + Peak;
}