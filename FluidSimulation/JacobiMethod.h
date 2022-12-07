#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

const double NEARZERO = 1.0e-10;       // interpretation of "zero"

using vec = vector<double>;         // vector
using matrix = vector<vec>;            // matrix (=collection of (row) vectors)

class JacobiMethod
{
    // Utils
    static double error(const vec& x, const vec& y, int n);

public:
    // Prototypes
    static void solve(const matrix& a, const vec& b, vec& x, int n);

    // UT
    static void UT_JacobiMethod();
};