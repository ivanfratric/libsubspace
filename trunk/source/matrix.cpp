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
#include <string.h>
#include <math.h>

#include "matrix.h"

namespace LibSubspace {

Matrix::Matrix() {
	nrows = 0;
	ncols = 0;
	data = NULL;
}

Matrix::Matrix(long nrows, long ncols) {
	this->nrows = nrows;
	this->ncols = ncols;
	data = (double *)malloc(nrows*ncols*sizeof(double));
	memset(data,0,nrows*ncols*sizeof(double));
}

void Matrix::Init(long nrows, long ncols) {
	if(data) free(data);

	this->nrows = nrows;
	this->ncols = ncols;
	data = (double *)malloc(nrows*ncols*sizeof(double));
	memset(data,0,nrows*ncols*sizeof(double));
}

Matrix::Matrix(const Matrix& src) {
	nrows = src.nrows;
	ncols = src.ncols;
	data = (double *)malloc(nrows*ncols*sizeof(double));
	memcpy(data,src.data,nrows*ncols*sizeof(double));
}

Matrix::~Matrix() {
	if(data) free(data);
}

Matrix& Matrix::operator=(const Matrix &src) {
	nrows = src.nrows;
	ncols = src.ncols;
	if(data) free(data);
	data = (double *)malloc(nrows*ncols*sizeof(double));
	memcpy(data,src.data,nrows*ncols*sizeof(double));
	return *this;
}

Matrix Matrix::Transpose() {
	long i,j;
	Matrix t(ncols,nrows);
	for(i=0;i<nrows;i++) {
		for(j=0;j<ncols;j++) {
			t.data[j*nrows+i] = data[i*ncols+j];
		}
	}
	return t;
}

Matrix Matrix::operator*(Matrix& src) {
	if(ncols != src.nrows) {
		printf("Matrix multiplication error: matrix dimensions don't match.\n");
		//return empty matrix
		Matrix ret;
		return ret;
	}
	long i,j,k;
	double sum;

	Matrix ret(nrows, src.ncols);

	if(ncols >= 100) { 	//use more efficient caching for large matrices
		Matrix srct = src.Transpose();
		double *row1,*row2;

		for(i=0;i<nrows;i++) {
			row1 = &(data[i*ncols]);
			for(j=0;j<src.ncols;j++) {
				row2 = &(srct.data[j*srct.ncols]);
				sum=0;
				for(k=0;k<ncols;k++) {
					sum+=row1[k]*row2[k];
				}
				ret.data[i*src.ncols+j]=sum;
			}
		}
	} else { //standard matrix multiplication
		for(i=0;i<nrows;i++) {
			for(j=0;j<src.ncols;j++) {
				sum=0;
				for(k=0;k<ncols;k++) {
					sum+=data[i*ncols+k]*src.data[k*src.ncols+j];
				}
				ret.data[i*src.ncols+j]=sum;
			}
		}
	}

	return ret;
}

Matrix Matrix::operator+(Matrix& src) {
	if((nrows != src.nrows)||(ncols != src.ncols)) {
		printf("Matrix addition error: matrix dimensions don't match.\n");
		//return empty matrix
		Matrix ret;
		return ret;
	}
	Matrix ret(nrows, ncols);
	long i,size;
	size = nrows*ncols;
	for(i=0;i<size;i++) {
		ret.data[i]=data[i]+src.data[i];
	}
	return ret;
}

void Matrix::scalarMultiply(double scalar) {
	long size = nrows*ncols;
	for(long i=0;i<size;i++) {
		data[i]*=scalar;
	}
}

void Matrix::Save(char *filename) {
	FILE *fp;
	fp = fopen(filename,"wb");
	if(!fp) {
		printf("Error opening %s\n",filename);
		return;
	}
	fwrite(&nrows,1,sizeof(long),fp);
	fwrite(&ncols,1,sizeof(long),fp);
	fwrite(data,nrows*ncols,sizeof(double),fp);
	fclose(fp);
}

void Matrix::Load(char *filename) {
	FILE *fp;
	fp = fopen(filename,"rb");
	if(!fp) {
		printf("Error opening %s\n",filename);
		return;
	}
	fread(&nrows,1,sizeof(long),fp);
	fread(&ncols,1,sizeof(long),fp);
	if(data) free(data);
	data = (double *)malloc(nrows*ncols*sizeof(double));
	fread(data,nrows*ncols,sizeof(double),fp);
	fclose(fp);
}

} //namespace
