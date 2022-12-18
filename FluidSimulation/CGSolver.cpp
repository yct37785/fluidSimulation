#include "CGSolver.h"

double CGSolver::dot(const vec& x, const vec& y)
{
    double result = 0.0;
    for (int i = 0; i < x.size(); i++)
        result += x[i] * y[i];
    return result;
}

void CGSolver::mul(vec& output, const matrix& m, const vec& a)
{
    for (int i = 0; i < output.size(); i++)
    {
        output[i] = 0.0;
        for (int j = 0; j < m[i].size(); j += 2)
            output[i] += m[i][j + 1] * a[m[i][j]];
    }
}

// Function to compute the residual r = b - Ax
vec CGSolver::residual(const matrix& A, const vec& x, const vec& b)
{
    int n = x.size();
    vector<double> r(n);
    for (int i = 0; i < n; i++)
    {
        r[i] = b[i];
        for (int j = 0; j < A[i].size(); j += 2)
            r[i] -= A[i][j + 1] * x[A[i][j]];
    }
    return r;
}

// Function to solve the linear system Ax = b using the Conjugate Gradient method
vec CGSolver::conjugateGradient(const matrix& A, const vec& b, precon_func preconditioner)
{
    double tol = 1e-6;
    int maxIter = 100;
    int n = b.size();
    vec x(n, 0.0); // Initialize solution to 0
    vec r = residual(A, x, b);  // r = b - Ax (x is 0 so we set r = b)
    vec p = b; // Initialize search direction to b
    // preconditional
    //if (preconditioner)
    //{
    //    matrix M = preconditioner(A);
    //    mul(p, M, r);  // Initialize search direction to M^(-1) * r
    //}
    double rdotr = dot(r, r);
    vec Ap(n, 0.0);
    int iterCount = 0;
    while (iterCount < maxIter)
    {
        // check convergence
        if (sqrt(rdotr) < tol)
            break;
        // Compute Ap
        mul(Ap, A, p);
        double alpha = rdotr / dot(p, Ap);
        // Update solution and residual
        for (int j = 0; j < n; j++)
        {
            x[j] += alpha * p[j];
            r[j] -= alpha * Ap[j];
        }
        double rnewdotrnew = dot(r, r);
        // Update search direction
        for (int j = 0; j < n; j++)
            p[j] = r[j] + (rnewdotrnew / rdotr) * p[j];
        rdotr = rnewdotrnew;
        iterCount++;
    }
    cout << "Iter count: " << iterCount << endl;
    return x;
}

matrix incomplete_cholesky_preconditioner(const matrix& A) {
    int n = A.size();
    matrix L(n, vec(n));  // Initialize preconditioner to zero matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0;
            for (int k = 0; k < j; k++)
                sum += L[i][k] * L[j][k];
            if (i == j)
            {
                L[i][i] = sqrt(A[i][i] - sum);
                cout << "1: " << L[i][i] << endl;
            }
            else
            {
                L[i][j] = (A[i][j] - sum) / max(L[j][j], NEARZERO);
                cout << "2: " << L[i][i] << endl;
            }
        }
    }
    return L;
}

void CGSolver::UT_CG()
{
    // Test with a simple 3x3 system
    matrix A = { { 0, -10, 1, -1, 2, 2 },
                { 0, -1, 1, -11, 2, -1 },
                { 0, 2, 1, -1, 2, -10 } };
    vec x_ans = { 6.3, 2.2, 1.5 };
    vec b = { 0.0, 0.0, 0.0 };
    for (int i = 0; i < A.size(); i++)
        for (int j = 0; j < A[i].size(); j += 2)
            b[i] += A[i][j + 1] * x_ans[A[i][j]];
    cout << "Expected: [";
    for (int i = 0; i < x_ans.size(); i++)
        cout << x_ans[i] << (i == x_ans.size() - 1 ? "]" : ", ");
    cout << endl;
    vec x = conjugateGradient(A, b, incomplete_cholesky_preconditioner);

    cout << "Solution: [";
    for (int i = 0; i < x.size(); i++)
        cout << x[i] << (i == x.size() - 1 ? "]" : ", ");
    cout << endl;
}