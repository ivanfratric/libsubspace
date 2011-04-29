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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "eigen.h"

extern "C" int ilaenv_(int *ispec, const char *name__, const char *opts, int *n1,
	int *n2, int *n3, int *n4, int name_len, int opts_len);

extern "C" int dsyev_(const char *jobz, const char *uplo, int *n, double *a,
	int *lda, double *w, double *work, int *lwork, 
	int *info);

extern "C" int dsygv_(int *itype, const char *jobz, const char *uplo, int *
	n, double *a, int *lda, double *b, int *ldb, 
	double *w, double *work, int *lwork, int *info);

extern "C" int dggev_(const char *jobvl, const char *jobvr, int *n, double *
	a, int *lda, double *b, int *ldb, double *alphar, 
	double *alphai, double *beta, double *vl, int *ldvl, 
	double *vr, int *ldvr, double *work, int *lwork, 
	int *info);

namespace LibSubspace {

int eigen(double* A, double* V, double* E, int n, bool verbose) {
	int info;
	int ispec = 1;
	int lwork;

	lwork = (ilaenv_(&ispec,"DSYEV","U",&n,&n,&n,&n,5,1)+2)*n;
	double *work = (double *)malloc(lwork*sizeof(double));
	memcpy(V,A,n*n*sizeof(double));
	dsyev_("V","U",&n,V,&n,E,work,&lwork,&info);
	free(work);

	//check the return value
	if(info!=0) {
		if(verbose) {
			printf("Error computing eigenvectors: ");
			if(info>0) {
				printf("Algorithm failed to converge\n");
			} else if(info<0) {
				printf("Illegal argument\n");
			} else {
				printf("Unknown error\n");
			}
		}
		return 0;
	}

	return 1;
}


int geneigen(double *A, double *B, int n, double *W, int algorithm, bool verbose) {
	//getting the optimal lwork
	int ispec = 1;
	int lwork;
	int info;

	switch(algorithm) {

		case EIGEN_CHOL: 
		{
			//computing eigenvectors
			lwork = (ilaenv_(&ispec,"DSYGV","U",&n,&n,&n,&n,5,1)+2)*n;
			double *work = (double *)malloc(lwork*sizeof(double));
			int problemType = 1;
			char job = 'V';
			char uplo = 'U';
			dsygv_(&problemType,&job,&uplo,&n,A,&n,B,&n,W,work,&lwork,&info);

			free(work);

			//check the return value
			if(info!=0) {
				if(verbose) {
					printf("Error computing eigenvectors: ");
					if(info>n) {
						printf("Matrix B is not positive definite\n");
					} else if(info<=n) {
						printf("The problem failed to converge\n");
					} else if(info<0) {
						printf("Illegal argument\n");
					} else {
						printf("Unknown error\n");
					}
				}
				return 0;
			}
		}
		break;

		case EIGEN_QZ:
		{
			//more general algorithm
			double *alphar = (double *)malloc(n*sizeof(double));
			double *alphai = (double *)malloc(n*sizeof(double));
			double *beta = (double *)malloc(n*sizeof(double));
			double *VR = (double *)malloc(n*n*sizeof(double));
			lwork = 8*n;
			double *work = (double *)malloc(lwork*sizeof(double));
			dggev_("N","V",&n,A,&n,B,&n,alphar,alphai,beta,NULL,&n,VR,&n,work,&lwork,&info);
			//eigenvalues
			for(long i=0;i<n;i++) {
				if(beta[i]!=0) {
					W[i] = alphar[i]/beta[i];
				} else W[i] = 0;
			}
			//eigenvectors
			for(long i=0;i<n;i++) {
				for(long j=0;j<n;j++) {
					A[i*n+j] = VR[i*n+j];
				}
			}
			free(alphar);
			free(alphai);
			free(beta);
			free(VR);	
			free(work);

			if(info!=0) {
				printf("Error computing eigenvectors: ");
				if(info<0) {
					printf("Illegal argument\n");
				} else if(info<=n) {
					printf("QZ iteration failed\n");
				} else {
					printf("Unknown error\n");
				}
				return 0;
			}
		}
		break;

	}
	
	return 1;
}

} //namespace
