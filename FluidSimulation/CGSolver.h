#pragma once
#include "Utility.h"

using precon_func = matrix(*)(const matrix&);

class CGSolver
{
	static int Asize;
	static vec r, p, Ap;
	static double dot(const vec& x, const vec& y);
	static void mul(vec& output, const matrix& m, const vec& a);
	static void residual(vec& r, const matrix& A, const vec& x, const vec& b);

public:
	static void init(int Asize);
	static void solve(const matrix& A, const vec& b, vec& x, precon_func preconditioner = nullptr);

	// UT
	static void UT_CG();
};