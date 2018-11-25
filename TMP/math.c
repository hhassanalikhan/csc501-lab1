#define _CRT_SECURE_NO_WARNINGS
#define ALMOSTZERO 0.0000000000000000001
#define LN10 2.3025850929940456840179914546844

#include <stdio.h>

double log10(double);
double ln(double);

int main()
{
    double x, a;
    printf("This program calculates the logarithm base 10.\n");

   /* while ( 1 == scanf("%lf", &x)  &&  x > 0.0)
    {
        double a = log10(x);
        printf("log10(%lf) = %.12lf\n", x, a);
        printf("Enter a positive value: ");
    }*/

	
	  double x = (expdev(0.1));
	  printf(" %.12lf\n",x);
    return 0;
}

double expdev(double lambda) {
    double dummy;
    do
        dummy= (double) rand() / 100;
    while (dummy == 0.0);
    printf("log10(%lf) =", dummy);
    return -log10(dummy) / lambda;
}

double log10( double x ) {
    return ln(x) / LN10;    
}

double ln(double x)
{
    double old_sum = 0.0;
    double xmlxpl = (x - 1) / (x + 1);
    double xmlxpl_2 = xmlxpl * xmlxpl;
    double denom = 1.0;
    double frac = xmlxpl;
    double term = frac;                 // denom start from 1.0
    double sum = term;

    while ( sum != old_sum )
    {
        old_sum = sum;
        denom += 2.0;
        frac *= xmlxpl_2;
        sum += frac / denom;
    }
    return 2.0 * sum;
}

