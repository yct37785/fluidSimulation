#pragma once
#include "Utility.h"

using precon_func = matrix(*)(const matrix&);

class CGSolver
{
	static double dot(const vec& x, const vec& y);
	static void mul(vec& output, const matrix& m, const vec& a);
	static vec residual(const matrix& A, const vec& x, const vec& b);

public:
	static vec conjugateGradient(const matrix& A, const vec& b, precon_func preconditioner = nullptr);

	// UT
	static void UT_CG();
};