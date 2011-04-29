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

#include "sample.h"
#include "subspace.h"
#include "local.h"
#include "image.h"

namespace LibSubspace {

LocalDescriptor::LocalDescriptor() {
	size = 0;
	indices = NULL;
}

LocalDescriptor::~LocalDescriptor() {
	if(indices) free(indices);
}

void LocalDescriptor::Init(long originalwidth, long originalheight, long localwidth, long posx, long posy) {
	long minx,miny,maxx,maxy;
	minx = posx;
	miny = posy;
	maxx = posx+localwidth;
	maxy = posy+localwidth;

	if(minx<0) minx = 0;
	if(miny<0) miny = 0;
	if(maxx>originalwidth) maxx = originalwidth;
	if(maxy>originalheight) maxy = originalheight;

	size = (maxx-minx)*(maxy-miny);
	if(indices) free(indices);
	indices = (long *)malloc(size*sizeof(long));

	long x,y;
	long k=0;
	for(y=miny;y<maxy;y++) {
		for(x=minx;x<maxx;x++) {
			indices[k] = y*originalwidth+x;
			k++;
		}
	}
}

LocalSubspace::LocalSubspace() {
	localDescriptors = NULL;
	localSubspaces = NULL;
	features = NULL;
	numLocalDescriptors = 0;
	numLocalSubspaces = 0;
	numFeatures = 0;
}

LocalSubspace::~LocalSubspace() {
	Clear();
}

void LocalSubspace::Clear() {
	if(localDescriptors) delete [] localDescriptors;
	if(localSubspaces) delete [] localSubspaces;
	if(features) delete [] features;
	localDescriptors = NULL;
	localSubspaces = NULL;
	features = NULL;
	numLocalDescriptors = 0;
	numLocalSubspaces = 0;
	numFeatures = 0;
}

LocalSubspaceGenerator::LocalSubspaceGenerator(SubspaceGenerator *subspaceGenerator, long imageWidth, long imageHeight, long windowWidth, long windowStep, long numFeatures) {
	this->originalWidth = imageWidth;
	this->originalHeight = imageHeight;
	this->windowWidth = windowWidth;
	this->windowStep = windowStep;
	this->subspaceGenerator = subspaceGenerator;
	this->numFeatures = numFeatures;
}


LocalSubspaceGenerator::~LocalSubspaceGenerator() {
}

void LocalSubspaceGenerator::InitLocalDescriptors(LocalSubspace *localSubspace) {
	long nstepsx = (originalWidth-windowWidth)/windowStep + 1;
	long nstepsy = (originalHeight-windowWidth)/windowStep + 1;
	
	localSubspace->numLocalDescriptors = nstepsx*nstepsy;
	localSubspace->localDescriptors = new LocalDescriptor[localSubspace->numLocalDescriptors];
	
	long x,y;
	y = 0;

	long i,j;
	long k=0;
	for(i=0;i<nstepsy;i++) {
		x = 0;
		for(j=0;j<nstepsx;j++) {
			localSubspace->localDescriptors[k].Init(originalWidth,originalHeight,windowWidth,x,y);
			k++;
			x+=windowStep;
		}
		y += windowStep;
	}
}

void CreateLocalSample(Sample *localSample, Sample * originalSample, LocalDescriptor *localDescriptor) {
	localSample->Init(localDescriptor->Size());
	localSample->SetFilename(originalSample->GetFilename());
	localSample->SetClassname(originalSample->GetClassname());
	for(long i=0;i<localDescriptor->Size();i++) {
		if(localDescriptor->GetIndices()[i]>=0) {
			(*localSample)[i] = (*originalSample)[localDescriptor->GetIndices()[i]];
		} else {
			(*localSample)[i] = 0;
		}
	}
}

void LocalSubspaceGenerator::CreateLocalSubspaces(SampleSet *originalSampleSet, LocalSubspace *localSubspace) {
	long numlocalsamplesets = localSubspace->numLocalDescriptors;
	localSubspace->numLocalSubspaces = numlocalsamplesets;
	localSubspace->localSubspaces = new Subspace[localSubspace->numLocalSubspaces];

	SampleSet localsampleset;

	for(long i=0;i<localSubspace->numLocalDescriptors;i++) {
		localsampleset.Init(originalSampleSet->Size());
		for(long j=0;j<originalSampleSet->Size();j++) {
			CreateLocalSample(localsampleset.GetSample(j),originalSampleSet->GetSample(j),&localSubspace->localDescriptors[i]);
		}

		printf("Generating local subspace.\n");

		subspaceGenerator->GenerateSubspace(&localsampleset,&localSubspace->localSubspaces[i]);
	}
}

int LocalSubspaceGenerator::FeatureCompare(const void *f1, const void *f2) {
	LocalFeature *feature1,*feature2;
	feature1 = (LocalFeature *)f1;
	feature2 = (LocalFeature *)f2;
	if(feature1->value > feature2->value) return -1;
	if(feature1->value < feature2->value) return 1;
	return 0;
}

void LocalSubspaceGenerator::MergeSubspaces(LocalSubspace *localSubspace) {
	long total = 0;

	long numfeatures;
	LocalFeature *features;

	for(long i=0;i<localSubspace->numLocalSubspaces;i++) {
		total += localSubspace->localSubspaces[i].GetSubspaceDim();
	}

	numfeatures = total;
	features = (LocalFeature *)malloc(numfeatures*sizeof(LocalFeature));

	long k=0;
	for(long i=0;i<localSubspace->numLocalSubspaces;i++) {
		for(long j=0;j<localSubspace->localSubspaces[i].GetSubspaceDim();j++) {
			features[k].descriptorindex = i;
			features[k].subspaceindex = i;
			features[k].axisindex = j;
			features[k].value = abs(localSubspace->localSubspaces[i].GetAxesCriterionFn()[j]);
			k++;
		}
	}

	qsort(features,numfeatures,sizeof(LocalFeature),FeatureCompare);

	if(this->numFeatures < numfeatures) numfeatures = this->numFeatures;
	features = (LocalFeature *)realloc(features,numfeatures*sizeof(LocalFeature));

	localSubspace->numFeatures = numfeatures;
	localSubspace->features = features;
}

void LocalDescriptor::Save(FILE *fp) {
	fwrite(&size,sizeof(long),1,fp);
	fwrite(indices,size*sizeof(long),1,fp);
}

void LocalDescriptor::Load(FILE *fp) {
	fread(&size,sizeof(long),1,fp);
	indices = (long *)malloc(size*sizeof(long));
	fread(indices,size*sizeof(long),1,fp);
}

void LocalSubspace::Save(char *filename) {
	FILE *fp = fopen(filename,"wb");
	if(!fp) return;

	fwrite(&numLocalDescriptors,sizeof(long),1,fp);
	fwrite(&numLocalSubspaces,sizeof(long),1,fp);
	fwrite(&numFeatures,sizeof(long),1,fp);
	
	for(long i=0;i<numLocalDescriptors;i++) {
		localDescriptors[i].Save(fp);
	}
	for(long i=0;i<numLocalSubspaces;i++) {
		localSubspaces[i].Save(fp);
	}
	fwrite(features,sizeof(LocalFeature)*numFeatures,1,fp);

	fclose(fp);
}

int LocalSubspace::Load(char *filename) {
	Clear();

	FILE *fp = fopen(filename,"rb");
	if(!fp) return 0;

	fread(&numLocalDescriptors,sizeof(long),1,fp);
	fread(&numLocalSubspaces,sizeof(long),1,fp);
	fread(&numFeatures,sizeof(long),1,fp);
	
	localDescriptors = new LocalDescriptor[numLocalDescriptors];
	localSubspaces = new Subspace[numLocalDescriptors];
	features = (LocalFeature *)malloc(sizeof(LocalFeature)*numFeatures);

	for(long i=0;i<numLocalDescriptors;i++) {
		localDescriptors[i].Load(fp);
	}
	for(long i=0;i<numLocalSubspaces;i++) {
		localSubspaces[i].Load(fp);
	}
	fread(features,sizeof(LocalFeature)*numFeatures,1,fp);

	fclose(fp);

	return 1;
}

void LocalSubspace::ToImage(int width, int height, int ncomponents, Image *image) {
	int size = width*height;
	long *buf = (long*)malloc(size*sizeof(long));
	memset(buf,0,size*sizeof(long));
	
	int i,j;
	for(i=0;i<ncomponents;i++) {
		int descriptorindex;
		LocalDescriptor *localdescriptor;

		descriptorindex = features[i].descriptorindex;
		localdescriptor = &localDescriptors[descriptorindex];

		for(j=0;j<localdescriptor->Size();j++) {
			if(localdescriptor->GetIndices()[j]>0) {
				buf[localdescriptor->GetIndices()[j]]++;
			}
		}
	}

	long max = 0;
	for(i=0;i<size;i++) {
		if(buf[i]>max) max = buf[i];
	}
	for(i=0;i<size;i++) {
		buf[i] = (buf[i]*255)/max;
	}

	image->Init(width,height);
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			image->SetPixelGray(j,i,(unsigned char)(buf[i*width+j]));
		}
	}

	free(buf);
}


void LocalSubspaceGenerator::GenerateSubspace(SampleSet *samples, LocalSubspace *subspace) {
	InitLocalDescriptors(subspace);
	CreateLocalSubspaces(samples,subspace);
	MergeSubspaces(subspace);
}

void LocalSubspaceProjector::ProjectSample(Sample *originalSample, Sample *projectedSample, int dim) {
	if(dim == 0) dim = subspace->numFeatures;
	projectedSample->Init(dim);

	SampleSet localSamples;
	localSamples.Init(subspace->numLocalDescriptors);
	for(long i=0;i<subspace->numLocalDescriptors;i++) {
		CreateLocalSample(localSamples.GetSample(i),originalSample,&subspace->localDescriptors[i]);
	}
	
	long areaindex, subspaceindex, axisindex, originaldim;
	double sum;
	Subspace *cursubspace;
	Sample *cursample;
	for(long i=0;i<dim;i++) {
		areaindex = subspace->features[i].descriptorindex;
		subspaceindex = subspace->features[i].subspaceindex;
		axisindex = subspace->features[i].axisindex;

		cursubspace = &subspace->localSubspaces[subspaceindex];
		cursample = localSamples.GetSample(areaindex);

		originaldim = cursubspace->GetOriginalDim();

		sum = 0;
		for(long j=0;j<originaldim;j++) {
			sum += ((*cursample)[j] - cursubspace->GetCenterOffset()[j]) * cursubspace->GetSubspaceAxes()[axisindex*originaldim+j];
		}
		(*projectedSample)[i] = sum;
	}

	projectedSample->SetClassname(originalSample->GetClassname());
	projectedSample->SetFilename(originalSample->GetFilename());
}

void LocalSubspaceProjector::ProjectSampleSet(SampleSet *originalSamples, SampleSet *projectedSamples, int dim) {
	long n = originalSamples->Size();
	projectedSamples->Init(n);
	for(long i=0;i<n;i++) {
		ProjectSample(originalSamples->GetSample(i),projectedSamples->GetSample(i), dim);
	}
}

} //namespace
