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
#include "sample.h"
#include "eigen.h"

#include "subspace.h"
#include "image.h"
#include "imageio.h"

namespace LibSubspace {

Subspace::Subspace() {
	type = -1;
	subspaceDim = 0;
	originalDim = 0;
	centerOffset = NULL;
	subspaceAxes = NULL;
	axesCriterionFn = NULL;
};

Subspace::~Subspace() {
	if(centerOffset) free(centerOffset);
	if(subspaceAxes) free(subspaceAxes);
	if(axesCriterionFn) free(axesCriterionFn);
}

void Subspace::SetData(long type, long subspaceDim, long originalDim, double *centerOffset, 
	double *subspaceAxes, double *axesCriterionFn) 
{
	this->type = type;
	this->subspaceDim = subspaceDim;
	this->originalDim = originalDim;
	if(this->centerOffset) free(this->centerOffset);
	if(this->subspaceAxes) free(this->subspaceAxes);
	if(this->axesCriterionFn) free(this->axesCriterionFn);
	this->centerOffset = (double*)malloc(originalDim*sizeof(double));
	this->subspaceAxes = (double*)malloc(originalDim*subspaceDim*sizeof(double));
	this->axesCriterionFn = (double*)malloc(subspaceDim*sizeof(double));
	if(centerOffset) memcpy(this->centerOffset, centerOffset, originalDim*sizeof(double));
	else memset(this->centerOffset,0,originalDim*sizeof(double));
	memcpy(this->subspaceAxes, subspaceAxes, originalDim*subspaceDim*sizeof(double));
	if(axesCriterionFn) memcpy(this->axesCriterionFn, axesCriterionFn, subspaceDim*sizeof(double));
	else memset(this->centerOffset,0,subspaceDim*sizeof(double));
}

void Subspace::Save(FILE *fp) {
	fwrite(&type,1,sizeof(long),fp);
	fwrite(&subspaceDim,1,sizeof(long),fp);
	fwrite(&originalDim,1,sizeof(long),fp);
	fwrite(centerOffset,originalDim,sizeof(double),fp);
	fwrite(subspaceAxes,originalDim*subspaceDim,sizeof(double),fp);
	fwrite(axesCriterionFn,subspaceDim,sizeof(double),fp);
}

int Subspace::Save(char *filename) {
	FILE *fp;
	fp = fopen(filename,"wb");
	if(!fp) return 0;
	Save(fp);
	fclose(fp);
	return 1;
}

void Subspace::Load(FILE *fp) {
	fread(&type,1,sizeof(long),fp);
	fread(&subspaceDim,1,sizeof(long),fp);
	fread(&originalDim,1,sizeof(long),fp);
	if(centerOffset) free(centerOffset);
	if(subspaceAxes) free(subspaceAxes);
	if(axesCriterionFn) free(axesCriterionFn);
	this->centerOffset = (double*)malloc(originalDim*sizeof(double));
	this->subspaceAxes = (double*)malloc(originalDim*subspaceDim*sizeof(double));
	this->axesCriterionFn = (double*)malloc(subspaceDim*sizeof(double));
	fread(centerOffset,originalDim,sizeof(double),fp);
	fread(subspaceAxes,originalDim*subspaceDim,sizeof(double),fp);
	fread(axesCriterionFn,subspaceDim,sizeof(double),fp);
}

int Subspace::Load(char *filename) {
	FILE *fp;
	fp = fopen(filename,"rb");
	if(!fp) return 0;
	Load(fp);
	fclose(fp);
	return 1;	
}

void Subspace::ReorderDescending() {
	long i,j;
	double max,tmpd;
	long maxi;
	double *tmp = (double *)malloc(originalDim*sizeof(double));
	for(i=0;i<subspaceDim;i++) {
		max = axesCriterionFn[i];
		maxi = i;
		for(j=i+1;j<subspaceDim;j++) {
			if(axesCriterionFn[j]>max) {
				max = axesCriterionFn[j];
				maxi = j;
			}
		}
		if(maxi!=i) {
			memcpy(tmp,&(subspaceAxes[i*originalDim]),originalDim*sizeof(double));
			memcpy(&(subspaceAxes[i*originalDim]),&(subspaceAxes[maxi*originalDim]),originalDim*sizeof(double));
			memcpy(&(subspaceAxes[maxi*originalDim]),tmp,originalDim*sizeof(double));
			tmpd = axesCriterionFn[i];
			axesCriterionFn[i] = axesCriterionFn[maxi];
			axesCriterionFn[maxi] = tmpd;
		}
	}
	free(tmp);
}

void Subspace::ReorderAbsDescending() {
	long i,j;
	double max,tmpd;
	long maxi;
	double *tmp = (double *)malloc(originalDim*sizeof(double));
	for(i=0;i<subspaceDim;i++) {
		max = axesCriterionFn[i];
		maxi = i;
		for(j=i+1;j<subspaceDim;j++) {
			if(abs(axesCriterionFn[j])>abs(max)) {
				max = axesCriterionFn[j];
				maxi = j;
			}
		}
		if(maxi!=i) {
			memcpy(tmp,&(subspaceAxes[i*originalDim]),originalDim*sizeof(double));
			memcpy(&(subspaceAxes[i*originalDim]),&(subspaceAxes[maxi*originalDim]),originalDim*sizeof(double));
			memcpy(&(subspaceAxes[maxi*originalDim]),tmp,originalDim*sizeof(double));
			tmpd = axesCriterionFn[i];
			axesCriterionFn[i] = axesCriterionFn[maxi];
			axesCriterionFn[maxi] = tmpd;
		}
	}
	free(tmp);
}

Matrix Subspace::ToMatrix() {
	Matrix m(subspaceDim, originalDim);
	memcpy(m.GetData(),subspaceAxes,originalDim*subspaceDim*sizeof(double));
	return m;
}

void Subspace::ToImages(long n, char *basename, long sizex, long sizey) {
	long i,j,k;	
	char filename[1024];
	Image img;
	ImageIO io;

	if((sizex<=0)||(sizey<=0)) {
		sizex = (int)sqrt((double)originalDim);
		sizey = originalDim/sizex;
	}

	img.Init(sizex,sizey);

	for(i=0;i<n;i++) {
		if(n>=subspaceDim) break;

		sprintf(filename,"%s%03ld.bmp",basename,i);

		double min,max;
		min = subspaceAxes[i*originalDim];
		max = min;
		for(j=1;j<originalDim;j++) {
			if(subspaceAxes[i*originalDim+j]>max) max = subspaceAxes[i*originalDim+j];
			if(subspaceAxes[i*originalDim+j]<min) min = subspaceAxes[i*originalDim+j];		
		}

		double tmp;
		for(j=0;j<sizey;j++) {
			for(k=0;k<sizex;k++) {
				tmp = (subspaceAxes[i*originalDim+j*sizex+k]-min)/(max-min);
				tmp *=256;
				if(tmp<0) tmp = 0;
				if(tmp>255) tmp = 255;
				img.SetPixelGray(k,j,(unsigned char)tmp);
			}
		}

		io.SaveImage(filename,&img);
	}
}

int LDASubspaceGenerator::GenerateSubspace(SampleSet* sampleSet, Subspace* subspace) {
	long N,n,Nc;
	//getting the problem dimensionality
	N = sampleSet->Size();				//number of samples
	n = (*sampleSet)[0].Size();		//sample dimensionality
	Nc = sampleSet->GetNumberOfClasses();	//number of classes

	if((Npca<=0) && (n <= (N-Nc))) {
		//direct LDA
		Matrix E(1,n);
		Sample avg = sampleSet->GetAvgSample();

		if(verbose) printf("Getting beetween class variance matrix...\n");
		Matrix B = sampleSet->GetBetweenClassVariance();
		if(verbose) printf("Getting within class variance matrix...\n");
		Matrix W = sampleSet->GetWithinClassVariance();

		if(verbose) printf("Computing generalized eigenvectors...\n");
		if(!geneigen(B.GetData(),W.GetData(),n,E.GetData(),EIGEN_CHOL,verbose)) return 0;

		subspace->SetData(SUBSPACE_LDA, n, n, avg.GetData(), B.GetData(), E.GetData());
	} else {
		if(Npca == 0) Npca = N-Nc;
		//PCA has to be performed first
		if(verbose) printf("Performing PCA...\n");
		Subspace PCASubspace;
		Subspace LDASubspace;
		PCASubspaceGenerator PCAGen;
		PCAGen.verbose = verbose;
		PCAGen.GenerateSubspace(sampleSet,&PCASubspace);
		
		if(verbose) printf("Projecting samples into low-dimensional subspace...\n");
		SubspaceProjector projector(&PCASubspace);
		SampleSet transformedSamples;
		projector.ProjectSampleSet(sampleSet,&transformedSamples,Npca);

		if(verbose) printf("Getting beetween class variance matrix...\n");
		Matrix B = transformedSamples.GetBetweenClassVariance();
		if(verbose) printf("Getting within class variance matrix...\n");
		Matrix W = transformedSamples.GetWithinClassVariance();

		if(verbose) printf("Computing generalized eigenvectors...\n");
		Matrix E(1,Npca);
		if(!geneigen(B.GetData(),W.GetData(),Npca,E.GetData(),EIGEN_CHOL,verbose)) return 0;

		Matrix Avg(1,Npca);
		LDASubspace.SetData(SUBSPACE_LDA, Npca, Npca, Avg.GetData(), B.GetData(), E.GetData());
		LDASubspace.ReorderAbsDescending();

		if(verbose) printf("Computing final subspace...\n");
		Matrix MatLda(Npca,Npca);
		Matrix MatPca(Npca,n);
		Matrix MatFinal(Npca,n);
		memcpy(MatLda.GetData(), LDASubspace.GetSubspaceAxes(), (Npca)*(Npca)*sizeof(double));
		memcpy(MatPca.GetData(), PCASubspace.GetSubspaceAxes(), (Npca)*(n)*sizeof(double));
		MatFinal = MatLda*MatPca;

		subspace->SetData( SUBSPACE_LDA, Npca, n, PCASubspace.GetCenterOffset(), MatFinal.GetData(), LDASubspace.GetAxesCriterionFn());
	}

	subspace->ReorderAbsDescending();
	subspace->Normalize();

	return 1;
}


int PCASubspaceGenerator::GenerateSubspace(SampleSet* sampleSet, Subspace* subspace) {
	long i,j;
	long N,n;
	
	if(sampleSet->Size() == 0) return 0;

	//getting the problem dimensionality
	N = sampleSet->Size();					//number of samples
	n = (*sampleSet)[0].Size();			//sample dimensionality

	//getting the sample matrix
	Sample avg = sampleSet->GetAvgSample();
	Matrix X = sampleSet->GetAsMatrix();
	for(i=0;i<X.GetNumRows();i++) {
		for(j=0;j<X.GetNumCols();j++) {
			X[i][j] -= avg[i];
		}
	}

	Matrix XT = X.Transpose();

	if(n > N) {
		if(verbose) printf("Computing covariance matrix...\n");
		Matrix XTX = XT*X;
		XTX.scalarMultiply(1.00/sampleSet->Size());
		Matrix tmpv(XTX.GetNumRows(),XTX.GetNumRows());
		Matrix eigenvectors(N,n);
		Matrix eigenvalues(N,1);
		if(verbose) printf("Computing eigenvectors...\n");
		if(!eigen(XTX.GetData(),tmpv.GetData(),eigenvalues.GetData(),XTX.GetNumRows(),verbose)) return 0;
		if(verbose) printf("Computing actual eigenvectors...\n");			
		eigenvectors = tmpv*XT;
		if(verbose) printf("Saving subspace data...\n");
		subspace->SetData(SUBSPACE_PCA,N,n,avg.GetData(),eigenvectors.GetData(),eigenvalues.GetData());		
	} else {
		if(verbose) printf("Computing covariance matrix...\n");
		Matrix XXT = X*XT;
		XXT.scalarMultiply(1.00/sampleSet->Size());
		Matrix eigenvectors(n,n);
		Matrix eigenvalues(n,1);
		if(verbose) printf("Computing eigenvectors...\n");
		if(!eigen(XXT.GetData(),eigenvectors.GetData(),eigenvalues.GetData(),n,verbose)) return 0;
		subspace->SetData(SUBSPACE_PCA,n,n,avg.GetData(),eigenvectors.GetData(),eigenvalues.GetData());
	}

	subspace->Normalize();
	subspace->ReorderAbsDescending();

	return 1;
}


SubspaceProjector::SubspaceProjector(Subspace *subspace) {
	this->subspace = subspace;
}

SubspaceProjector::~SubspaceProjector() {
	//delete subspace;
}

void SubspaceProjector::ProjectSampleSet(SampleSet *originalSamples, SampleSet *projectedSamples, int dim) {
	long n = originalSamples->Size();
	projectedSamples->Init(n);
	for(long i=0;i<n;i++) {
		ProjectSample(originalSamples->GetSample(i),projectedSamples->GetSample(i),dim);
	}
};

void SubspaceProjector::ProjectSample(Sample *originalSample, Sample *projectedSample, int dim) {
	if(!dim) dim = subspace->subspaceDim;
	projectedSample->Init(dim);
	long i,j;
	double sum;
	for(i=0;i<dim;i++) {
		sum = 0;
		for(j=0;j<originalSample->Size();j++) {
			sum += ((*originalSample)[j] - subspace->centerOffset[j]) * subspace->subspaceAxes[i*subspace->originalDim+j];
		}
		(*projectedSample)[i] = sum;
	}
	projectedSample->SetClassname(originalSample->GetClassname());
	projectedSample->SetFilename(originalSample->GetFilename());
};

void Subspace::Normalize() {
	double norm;
	long i,j;
	for(i = 0; i<subspaceDim ; i++) {
		double sum = 0;
		for(j=0;j<originalDim;j++) {
			sum+=subspaceAxes[i*originalDim+j]*subspaceAxes[i*originalDim+j];
		}
		norm = sqrt(sum);
		if(norm==0) continue;
		for(j=0;j<originalDim;j++) {
			subspaceAxes[i*originalDim+j] = subspaceAxes[i*originalDim+j]/norm;
		}
	}
}

} //namespace
