//Copyright (C) 2011 by Ivan Fratric
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.


#define EIGEN_CHOL 0
#define EIGEN_QZ 1

namespace LibSubspace {

//Computes eigenvalues & eigenvectors of a symmetric matrix:
//                A Vi  =  Ei Vi
//Returns 1 on success, 0 on failure
//Parameters: A(n, n) - source symmetric matrix (n - rows & columns number),
//            V(n, n) - matrix of its eigenvectors 
//                      (i-th row is an eigenvector Vi),
//            E(n)    - vector of its eigenvalues
//                          (i-th element is an eigenvalue Ei),
//            verbose - prints error messages to stdout
int eigen(double* A, double* V, double* E, int n, bool verbose = false);



//solves the A*x = (lambda)*B*x generalized eigenvalue problem
//where A and B are real symmetric matrices and B is positive definite
//returns 1 on success, 0 on failiure
//matrices A and B are destroyed during the computation of eigenvectors
//Parameters:
//		A (input/output)	:	on input, matrix A (nxn), 
//								on output, the matrix of eigenvectors
//								(row-ordered)
//		B (input)			:	matrix B (nxn)
//		n (input)			:	the order of matrices A and B
//		W (output)			:	the vector of eigenvalues in rising order
//		algorithm (input)	:	algorithm to use when computing eigenvectors
//								EIGEN_CHOL uses the Cholesky factorization, works only 
//									for symmetric A and symmetric positive definite B
//								EIGEN_QZ uses the QZ algorithm
//      verbose             :   prints error messages to stdout
int geneigen(double *A, double *B, int n, double *W, int algorithm = EIGEN_CHOL, bool verbose = false);

} //namespace
