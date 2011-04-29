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

namespace LibSubspace {

//a simple matrix class
class Matrix {
protected:
	long nrows; //number of rows
	long ncols; //number of columns
	double *data;
	
public:
	//constructors and destructor
	Matrix();
	Matrix(long m, long n);
	Matrix(const Matrix& src);
	~Matrix();

	//inits the matrix to nrows rows and ncols cols
	//sets all elements to 0
	void Init(long nrows, long ncols);

	//gets th i-th row
	double *operator[] (int i) {
		return &(data[i*ncols]);
	}

	//property getters
	long GetNumRows() {
		return nrows;
	}

	long GetNumCols() {
		return ncols;
	}

	double *GetData() {
		return data;
	}

	//some simple operators
	Matrix& operator=(const Matrix &src);
	Matrix operator*(Matrix& src);
	Matrix operator+(Matrix& src);
	
	//multiplies the matrix with a scalar
	void scalarMultiply(double scalar);

	//returns the transposed matrix
	Matrix Transpose();

	//saves the matrix to file with the name 'filename'
	void Save(char *filename);
	//loads the matrix from file with the name 'filename'
	void Load(char *filename);
};

} //namespace

