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
public:
    // Prototypes
    static void print(string title, const vec& V);
    static void print(string title, const matrix& A);
    static vec matrixTimesVector(const matrix& A, const vec& V);
    static void matrixTimesVectorS(const matrix& A, const vec& V, vec& toSet);
    static vec vectorCombination(double a, const vec& U, double b, const vec& V);
    static void vectorCombination_inPlace(double a, const vec& U, double b, const vec& V, vec& toSet);
    static double innerProduct(const vec& U, const vec& V);
    static double vectorNorm(const vec& V);
    static void optimizeMatrix(matrix& M);

    // UT
    static void UT_matrixTimesVector();
    static void UT_solveForX();

    static vec conjugateGradientSolver(const matrix& A, const vec& B);
    static void conjugateGradientSolverS(const matrix& A, const vec& B, vec& X);
    static void UT_cgsolver();
};