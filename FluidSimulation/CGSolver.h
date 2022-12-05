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

class CGSolver
{
	// Prototypes
	static void print(string title, const vec& V);
	static void print(string title, const matrix& A);
	static vec matrixTimesVector(const matrix& A, const vec& V);
	static vec vectorCombination(double a, const vec& U, double b, const vec& V);
	static double innerProduct(const vec& U, const vec& V);
	static double vectorNorm(const vec& V);

	static void UT_matrixTimesVector();

public:
	static vec conjugateGradientSolver(const matrix& A, const vec& B);
	static void UT_cgsolver();
};