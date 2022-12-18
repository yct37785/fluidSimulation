#include "JacobiMethod.h"
vec JacobiMethod::y;

// Function to find the maximum error in the solution
double JacobiMethod::error(const vec& x, const vec& y, int n)
{
    double max = 0;
    for (int i = 0; i < n; i++)
        if (abs(x[i] - y[i]) > max)
            max = abs(x[i] - y[i]);
    return max;
}

// Function to implement the Jacobi method
void JacobiMethod::solve(const matrix& a, const vec& b, vec& x)
{
    double tol = 1e-6;
    int maxIter = 200;
    int n = a.size();
    if (y.size() != n)
        y.resize(n);
    std::fill(y.begin(), y.end(), 0);
    double sum;
    int k = 0;
    double conv = 0.0;

    // Iterate until the error is less than the given tolerance
    // 30 seems to be the sweet spot
    while (k < maxIter)
    {
        // Store the previous solution in y[]
        for (int i = 0; i < n; i++)
            y[i] = x[i];

        // Update the solution
        for (int i = 0; i < n; i++)
        {
            sum = 0;
            // over each row in a
            double centerVal = 0.0;
            for (int j = 0; j < a[i].size(); j += 2)
            {
                int idx = int(a[i][j]);
                if (i != idx)
                    sum += a[i][j + 1] * y[idx];
                else
                    centerVal = a[i][j + 1];
            }
            /*for (int j = 0; j < a[i].size(); ++j)
            {
                if (i != j)
                {
                    sum += a[i][j] * y[j];
                }
                else
                    centerVal = a[i][j];
            }*/
            x[i] = (b[i] - sum) / centerVal;
        }
        // Check if the error is less than the given tolerance
        conv = error(x, y, n);
        if (conv < tol)
            break;

        // Print the intermediate solution
       /* cout << "Iteration " << k << ": ";
        for (i = 0; i < n; i++)
            cout << x[i] << " ";
        cout << endl;*/

        k++;
    }
    if (k > maxIter)
        cout << "Iter count: " << k << ", conv: " << (int)conv << endl;
}

void JacobiMethod::UT_JacobiMethod()
{
    int n = 3;

	// Coefficient matrix
    // for each array, every i = idx and every i + 1 = value
	/*matrix a = { { -10, -1, 2 },
				{ -1, -11, -1 },
				{ 2, -1, -10 } };*/
    matrix a = { { 0, -10, 1, -1, 2, 2 },
                { 0, -1, 1, -11, 2, -1 },
                { 0, 2, 1, -1, 2, -10 } };

	// Right-hand side vector
    cout << "RHS vec: ";
    vec x_ans = { 6.3, 2.2, 1.5 };
    vec b = { 0, 0, 0 };
	for (int i = 0; i < n; i++)
	{
        /*for (int j = 0; j < a[i].size(); j++)
            b[i] += a[i][j] * x_ans[j];*/
		for (int j = 0; j < a[i].size(); j += 2)
			b[i] += a[i][j + 1] * x_ans[a[i][j]];
		cout << b[i] << " ";
	}
	cout << endl;

	// Initial guess
    vec x = { 0, 0, 0 };

	// Call the Jacobi function
	solve(a, b, x);

	// Print the final solution
	cout << "Solution: ";
	for (int i = 0; i < n; i++)
		cout << x[i] << " ";

	// Mul x with a
	cout << "\nCompare: ";
	for (int i = 0; i < n; i++)
	{
		double prod = 0.0;
		/*for (int j = 0; j < n; ++j)
			prod += a[i][j] * x[j];*/
        for (int j = 0; j < a[i].size(); j += 2)
            prod += a[i][j + 1] * x[a[i][j]];
		cout << prod << " ";
	}
}
