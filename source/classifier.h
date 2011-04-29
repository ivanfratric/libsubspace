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

#define DISTANCE_EUCLIDEAN 1
#define DISTANCE_COSINE 2
#define DISTANCE_HAMMING 3

namespace LibSubspace {

//implements a one-NN (nearest neighbor) classifier
class OneNNClassifier {
protected:
	//returns dot product of vectors v1 and v2 of size n
	double DotProduct(double *v1, double *v2, long n);

	//returns norm of vector v of size n
	double Norm(double *v, long n);

	//returns Euclidean distance of vectors v1 and v2 of size n
	double EuclideanDistance(double *v1, double *v2, long n);

	//returns 1-cosine of angles between vectors v1 and v2 of size n
	double CosineDistance(double *v1, double *v2, long n);

	//returns normalized Hamming distance of vectors v1 and v2 of size n
	//only looks at the signs of vector components
	double HammingDistance(double *v1, double *v2, long n);

	//returns the distance between sample1 and sample 2
	//only the first dim components are considered
	//if dim=0, use the dimensionality of sample1
	double Distance(Sample *sample1, Sample *sample2, long dim = 0);

public:
	//distance measure to be used, by default DISTANCE_EUCLIDEAN
	int distanceMeasure;

	//outputs details of errorneous classifications
	bool verbose;

	OneNNClassifier() {
		distanceMeasure = DISTANCE_EUCLIDEAN;
		verbose = false;
	}

	//classifies sample testSample and returns a pointer to the claimed class name
	//parameters:
	//   testSample : sample whose class is determined
	//   baseSamples : database samples, used for comparison with the testSample
	//   dim : sample dimensionality, if 0 the dimensionality of the first base sample will be used
	char *ClassifySample(Sample *testSample, SampleSet *baseSamples, long dim = 0);


	//performs a classification test, returns classification accuracy
	//parameters:
	//   baseSamples : database samples, used for comparison with the testSamples
	//   testSamples : probe samples
	//   dim : sample dimensionality, if 0 the dimensionality of the first base sample will be used
	float ClassificationTest(SampleSet *baseSamples, SampleSet *testSamples, long dim = 0);

	//computes a distance matrix between two sample sets
	//an element (i,j) of the score matrix will be the distance of the i-th test sample to the j-th base sample
	//parameters:
	//   baseSamples : database samples
	//   testSamples : probe samples
	//   dim : sample dimensionality, if 0 the dimensionality of the first base sample will be used
	void GetDistanceMatrix(Matrix *matrix, SampleSet *baseSamples, SampleSet *testSamples, long dim = 0);
};

} //namespace

