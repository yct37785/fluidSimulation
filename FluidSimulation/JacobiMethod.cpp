#include "JacobiMethod.h"

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
void JacobiMethod::solve(const matrix& a, const vec& b, vec& x, int n)
{
    vec y(n);
    double sum;
    int i, j, k = 1;

    // Iterate until the error is less than the given tolerance
    while (1)
    {
        // Store the previous solution in y[]
        for (i = 0; i < n; i++)
            y[i] = x[i];

        // Update the solution
        for (i = 0; i < n; i++)
        {
            sum = 0;
            for (j = 0; j < n; j++)
                if (i != j)
                    sum += a[i][j] * y[j];
            x[i] = (b[i] - sum) / a[i][i];
        }

        // Check if the error is less than the given tolerance
        if (error(x, y, n) < 0.00001)
            break;

        // Print the intermediate solution
       /* cout << "Iteration " << k << ": ";
        for (i = 0; i < n; i++)
            cout << x[i] << " ";
        cout << endl;*/

        k++;
    }
}

void JacobiMethod::UT_JacobiMethod()
{
    int n = 3;

	// Coefficient matrix
	matrix a = { { -10, -1, 2 },
				{ -1, -11, -1 },
				{ 2, -1, -10 } };

	// Right-hand side vector
    vec x_ans = { 6.3, 2.2, 1.5 };
    vec b = { 0, 0, 0 };
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; ++j)
			b[i] += a[i][j] * x_ans[j];
		cout << b[i] << " ";
	}
	cout << endl;

	// Initial guess
    vec x = { 0, 0, 0 };

	// Call the Jacobi function
	solve(a, b, x, n);

	// Print the final solution
	cout << "Solution: ";
	for (int i = 0; i < n; i++)
		cout << x[i] << " ";

	// Mul x with a
	cout << "\nCompare: ";
	for (int i = 0; i < n; i++)
	{
		double prod = 0.0;
		for (int j = 0; j < n; ++j)
			prod += a[i][j] * x[j];
		cout << prod << " ";
	}
}
