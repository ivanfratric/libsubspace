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
#include <string.h>
#include <limits>

#include "matrix.h"
#include "sample.h"
#include "classifier.h"

namespace LibSubspace {

double OneNNClassifier::DotProduct(double *v1, double *v2, long n) {
	double dot = 0;
	for(long i=0;i<n;i++) {
		dot += v1[i]*v2[i];
	}
	return dot;
}

double OneNNClassifier::Norm(double *v, long n) {
	double dot = DotProduct(v,v,n);
	return sqrt(dot);
}

double OneNNClassifier::EuclideanDistance(double *v1, double *v2, long n) {
	double sum = 0,d;
	for(long i=0;i<n;i++) {
		d = v2[i]-v1[i];
		sum += d*d;
	}
	return sqrt(sum);
}

double OneNNClassifier::HammingDistance(double *v1, double *v2, long n) {
	double sum = 0;
	for(long i=0;i<n;i++) {
		if(((v1[i]<0)&&(v2[i]>0))||((v1[i]>0)&&(v2[i]<0))) sum = sum+1;
	}
	return sum/n;
}

double OneNNClassifier::CosineDistance(double *v1, double *v2, long n) {
	return 1-(DotProduct(v1,v2,n)/(Norm(v1,n)*Norm(v2,n)));
}

double OneNNClassifier::Distance(Sample *sample1, Sample *sample2, long dim) {
	double dist;

	if(!dim) dim = sample1->Size();

	if(distanceMeasure == DISTANCE_EUCLIDEAN) {
		dist = EuclideanDistance(sample1->GetData(), sample2->GetData(), dim);
	} else if (distanceMeasure == DISTANCE_COSINE) {
		dist = CosineDistance(sample1->GetData(), sample2->GetData(), dim);			
	} else if (distanceMeasure == DISTANCE_HAMMING) {
		dist = HammingDistance(sample1->GetData(), sample2->GetData(), dim);			
	} else dist = 0;

	return dist;
}

char *OneNNClassifier::ClassifySample(Sample *testSample, SampleSet *baseSamples, long dim) {
	int i;
	Sample *closestsample = NULL;

	double dist,mindist;

	if((!dim)||(dim>(*baseSamples)[0].Size())) dim = (*baseSamples)[0].Size();

	mindist = std::numeric_limits<double>::max();

	for(i = 0; i < baseSamples->Size(); i++) {
		if(testSample == baseSamples->GetSample(i)) continue; //never compare sample to itself, enables leave-one-out tests

		dist = Distance(baseSamples->GetSample(i),testSample,dim);

		if(dist < mindist) {
			mindist = dist;
			closestsample = baseSamples->GetSample(i);
		}
	}

	return closestsample->GetClassname();
}

float OneNNClassifier::ClassificationTest(SampleSet *baseSamples, SampleSet *testSamples, long dim) {
	int i;
	long numOK=0;
	char *claimedClass;

	for(i = 0; i < testSamples->Size(); i++) {
	
		claimedClass = ClassifySample(testSamples->GetSample(i), baseSamples, dim);

		if(strcmp(testSamples->GetSample(i)->GetClassname(),claimedClass)==0) {
			numOK ++;
		} else {
			if(verbose) {
				printf("Incorrect classification: %s <---> %s\n",testSamples->GetSample(i)->GetFilename(), claimedClass);
			}
		}
	}
	return ((float)numOK)/(testSamples->Size());
}

void OneNNClassifier::GetDistanceMatrix(Matrix *matrix, SampleSet *baseSamples, SampleSet *testSamples, long dim) {
	int i,j;
	double dist;

	matrix->Init(testSamples->Size(), baseSamples->Size());

	if((!dim)||(dim>(*baseSamples)[0].Size())) dim = (*baseSamples)[0].Size();

	for(i = 0; i < testSamples->Size(); i++) {
		for(j = 0; j < baseSamples->Size(); j++) {

			dist = Distance(baseSamples->GetSample(j),testSamples->GetSample(i),dim);

			(*matrix)[i][j] = dist;
		}
	}
}

} //namespace
