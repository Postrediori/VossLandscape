#include "pch.h"
#include "MathFunctions.h"

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
