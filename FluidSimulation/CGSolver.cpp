#include "CGSolver.h"


//======================================================================


void CGSolver::print(string title, const vec& V)
{
    cout << title << '\n';

    int n = V.size();
    for (int i = 0; i < n; i++)
    {
        double x = V[i];   if (abs(x) < NEARZERO) x = 0.0;
        cout << x << '\t';
    }
    cout << '\n';
}


//======================================================================


void CGSolver::print(string title, const matrix& A)
{
    cout << title << '\n';

    int m = A.size(), n = A[0].size();                      // A is an m x n matrix
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double x = A[i][j];   if (abs(x) < NEARZERO) x = 0.0;
            cout << x << '\t';
        }
        cout << '\n';
    }
}


//======================================================================


vec CGSolver::matrixTimesVector(const matrix& A, const vec& V)     // Matrix times vector
{
    int n = A.size();
    vec C(n);
    for (int i = 0; i < n; i++) C[i] = innerProduct(A[i], V);
    return C;
}

/**
* each row in A is of the following: [i1, v1, i2, v2 .... in, vn], where 'in' corr. to idx n and vn corr. to value n
* since matrix A will be sparse, to save memory we only store non-zero values of each row denoted by it's index
*/
vec CGSolver::matrixTimesVectorS(const matrix& A, const vec& V)
{
    int n = A.size();
    vec C(n);
    // for each row in mat
    for (int i = 0; i < n; i++)
    {
        double prod = 0;
        for (int j = 0; j < A[i].size(); j += 2)
        {
            prod += A[i][j + 1] * V[A[i][j]];
        }
        C[i] = prod;
    }
    return C;
}

void CGSolver::UT_matrixTimesVector()
{
    bool success = true;
    // test 1
    // regular matrix
    matrix A1 = { { 5, 0, 0 }, { 0, 3, 0 }, { 0, 0, 6 } };
    vec V = { 1, 2, 3 };
    vec AP = matrixTimesVector(A1, V);
    // mem optimized
    matrix A2 = { { 0, 5 }, { 1, 3 }, { 2, 6 } };
    vec AP2 = matrixTimesVectorS(A2, V);
    if (AP != AP2)
        success = false;
    // test 2
    // regular matrix
    A1 = { { 5, 0, 0, 0 }, { 7, 3, 0, 0 }, { 0, 0, 6, 2 }, { 0, 0, 0, 8 } };
    V = { 1, 2, 3, 4 };
    AP = matrixTimesVector(A1, V);
    // mem optimized
    A2 = { { 0, 5 }, { 0, 7, 1, 3 }, { 2, 6, 3, 2 }, { 3, 8 } };
    AP2 = matrixTimesVectorS(A2, V);
    if (success)
        cout << "UT: matrixTimesVector success" << endl;
    else
        cout << "UT: matrixTimesVector fail" << endl;
}


//======================================================================


vec CGSolver::vectorCombination(double a, const vec& U, double b, const vec& V)        // Linear combination of vectors
{
    int n = U.size();
    vec W(n);
    for (int j = 0; j < n; j++) W[j] = a * U[j] + b * V[j];
    return W;
}


//======================================================================


double CGSolver::innerProduct(const vec& U, const vec& V)          // Inner product of U and V
{
    return inner_product(U.begin(), U.end(), V.begin(), 0.0);
}


//======================================================================


double CGSolver::vectorNorm(const vec& V)                          // Vector norm
{
    return sqrt(innerProduct(V, V));
}


//======================================================================


void CGSolver::optimizeMatrix(matrix& M)
{
    for (int i = 0; i < M.size(); ++i)
    {
        vec newRow;
        for (int j = 0; j < M[i].size(); ++j)
        {
            if (M[i][j] != 0.0)
            {
                newRow.push_back(j);
                newRow.push_back(M[i][j]);
            }
        }
        M[i] = newRow;
    }
}

//======================================================================


vec CGSolver::conjugateGradientSolver(const matrix& A, const vec& B)
{
    double TOLERANCE = 1.0e-10;

    int n = A.size();
    vec X(n, 0.0);

    vec R = B;
    vec P = R;
    int k = 0;

    while (k < n)
    {
        vec Rold = R;                                         // Store previous residual
        vec AP = matrixTimesVector(A, P);

        double alpha = innerProduct(R, R) / max(innerProduct(P, AP), NEARZERO);
        X = vectorCombination(1.0, X, alpha, P);            // Next estimate of solution
        R = vectorCombination(1.0, R, -alpha, AP);          // Residual 

        if (vectorNorm(R) < TOLERANCE) break;             // Convergence test

        double beta = innerProduct(R, R) / max(innerProduct(Rold, Rold), NEARZERO);
        P = vectorCombination(1.0, R, beta, P);             // Next gradient
        k++;
    }

    return X;
}

vec CGSolver::conjugateGradientSolverS(const matrix& A, const vec& B)
{
    double TOLERANCE = 1.0e-10;

    int n = A.size();
    vec X(n, 0.0);

    vec R = B;
    vec P = R;
    int k = 0;

    while (k < n)
    {
        vec Rold = R;                                         // Store previous residual
        vec AP = matrixTimesVectorS(A, P);

        double alpha = innerProduct(R, R) / max(innerProduct(P, AP), NEARZERO);
        X = vectorCombination(1.0, X, alpha, P);            // Next estimate of solution
        R = vectorCombination(1.0, R, -alpha, AP);          // Residual 

        if (vectorNorm(R) < TOLERANCE) break;             // Convergence test

        double beta = innerProduct(R, R) / max(innerProduct(Rold, Rold), NEARZERO);
        P = vectorCombination(1.0, R, beta, P);             // Next gradient
        k++;
    }

    return X;
}


//======================================================================


void CGSolver::UT_solveForX()
{
    cout << "Solves AX = B\n";

    // basic matrix
    cout << "\nTest 1: basic matrix" << endl;
    matrix A = { { 4, 1 }, { 1, 3 } };
    vec B = { 1, 2 };
    vec X = conjugateGradientSolver(A, B);
    vec AX = matrixTimesVector(A, X);
    print("B:", B);
    print("AX:", AX);
    optimizeMatrix(A);
    X = conjugateGradientSolverS(A, B);
    print("AX2:", AX);
    // larger matrix
    cout << "\nTest 2: larger matrix" << endl;
    A = { { 0.7444, -0.5055, -0.0851 }, 
        { -0.5055, 3.4858, 0.0572 },
        { -0.0851, 0.0572, 0.4738 } };
    B = { -0.0043, 2.2501, 0.2798 };
    X = conjugateGradientSolver(A, B);
    AX = matrixTimesVector(A, X);
    print("B:", B);
    print("AX:", AX);
    optimizeMatrix(A);
    X = conjugateGradientSolverS(A, B);
    print("AX2:", AX);
    // larger matrix
    cout << "\nTest 3: larger matrix" << endl;
	A = { { 5.5,    1,   0,   1 },
		{     1,  7.2,   1,   0 },
		{     0,    1, 8.6,   1 },
		{     1,    0,   1, 2.5 } };
    vec x_ans = { 9.5, 1.4, 2.8, 4.5 };
    B = matrixTimesVector(A, x_ans);
    X = conjugateGradientSolver(A, B);
    AX = matrixTimesVector(A, X);
    print("B:", B);
    print("AX:", AX);
    optimizeMatrix(A);
    X = conjugateGradientSolverS(A, B);
    print("AX2:", AX);
}


//======================================================================


void CGSolver::UT_cgsolver()
{
    UT_matrixTimesVector();
    UT_solveForX();
}