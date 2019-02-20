#include "stdafx.h"
#include "Math.h"

#ifdef _MSC_VER
double trunc(double x)
{
    return (double)((int)x);
}

double round(double x)
{
    double t = trunc(x);
    double d = x - t;
    return (d<0.5) ? t : (t + 1.0);
}
#endif

int clp2(int x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x + 1;
}

double frac(double x)
{
    return (x - trunc(x));
}

int iround(double x)
{
    return int(round(x));
}
