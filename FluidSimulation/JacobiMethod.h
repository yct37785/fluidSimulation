#pragma once
#include "Utility.h"

class JacobiMethod
{
    static vec y;

    // Utils
    static double error(const vec& x, const vec& y, int n);

public:
    // Prototypes
    static void solve(const matrix& a, const vec& b, vec& x);

    // UT
    static void UT_JacobiMethod();
};