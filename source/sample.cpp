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

#include "sample.h"
#include "matrix.h"
#include "image.h"
#include "imageio.h"

namespace LibSubspace {

long typesize[] = {sizeof(char), sizeof(unsigned char), sizeof(long), sizeof(unsigned long), sizeof(float),sizeof(double)};

Sample::Sample() {
	data = NULL;
	filename[0] = 0;
	classname[0] = 0;
}

Sample::Sample(int size) {
	this->size = size;
	filename[0] = 0;
	classname[0] = 0;
	data = (double *)malloc(size*sizeof(double));
	memset(data,0,size*sizeof(double));
}

void Sample::Init(int size) {
	if(data) free(data);
	this->size = size;
	filename[0] = 0;
	classname[0] = 0;
	data = (double *)malloc(size*sizeof(double));
	memset(data,0,size*sizeof(double));
}

void Sample::SetData(int size, double *data) {
	this->size = size;
	if(this->data) free(this->data);
	this->data = (double *)malloc(size*sizeof(double));
	memcpy(this->data,data,size*sizeof(double));
}

Sample::Sample(const Sample& src) {
	this->size = src.size;
	this->data = (double *)malloc(src.size*sizeof(double));
	memcpy(this->data,src.data,size*sizeof(double));
	strncpy(this->filename,src.filename,SAMPLE_FILE_SIZE);
	strncpy(this->classname,src.classname,SAMPLE_CLASS_SIZE);
}

Sample& Sample::operator=(const Sample &src) {
	if(this->data) free(this->data);
	this->size = src.size;
	this->data = (double *)malloc(src.size*sizeof(double));
	memcpy(this->data,src.data,size*sizeof(double));
	strncpy(this->filename,src.filename,SAMPLE_FILE_SIZE);
	strncpy(this->classname,src.classname,SAMPLE_CLASS_SIZE);
	return *this;
}

Sample::~Sample() {
	if(data) free(data);
}

void Sample::SetFilename(char *filename) {
	strncpy(this->filename,filename,SAMPLE_FILE_SIZE);
	this->filename[SAMPLE_FILE_SIZE-1] = 0; //just in case
}

void Sample::SetClassname(char *classname) {
	strncpy(this->classname,classname,SAMPLE_CLASS_SIZE);
	this->classname[SAMPLE_CLASS_SIZE-1] = 0; //just in case
}

int Sample::ReadImageData(char *filename) {
	Image img;
	ImageIO io;
	int x,y,w,h;

	io.LoadImage(filename,&img);

	w = img.GetWidth();
	h = img.GetHeight();

	if((w==0)||(h==0)) {
		printf("Error reading image %s\n",filename);
		return 0;
	}

	size = w*h;

	if(data) free(data);
	data = (double *)malloc(size*sizeof(double));

	for(y=0;y<h;y++) {
		for(x=0;x<w;x++) {
			data[y*w+x]=(double)img.GetPixelGray(x,y);
		}
	}

	return 1;
}

int Sample::Load(char *filename, char *classname, int type, long size = 0) {
	strcpy(this->filename,filename);
	strcpy(this->classname,classname);

	if(type == TYPE_IMAGE) return ReadImageData(filename);

	FILE *fp;
	fp = fopen(filename,"rb");
	if(!fp) return 0;
	if(!size) {
		fseek(fp,0,SEEK_END);
		size = ftell(fp)/typesize[type];
		fseek(fp,0,SEEK_SET);
	}
	this->size = size;
	if(data) free(data);
	data = (double *)malloc(size*sizeof(double));
	switch(type) {
		case TYPE_CHAR:
			{
				char *tmp = (char *)malloc(size*sizeof(char));
				fread(tmp,size,sizeof(char),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
		case TYPE_UCHAR:
			{
				unsigned char *tmp = (unsigned char *)malloc(size*sizeof(unsigned char));
				fread(tmp,size,sizeof(unsigned char),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
		case TYPE_INT:
			{
				long *tmp = (long *)malloc(size*sizeof(long));
				fread(tmp,size,sizeof(long),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
		case TYPE_UINT:
			{
				unsigned long *tmp = (unsigned long *)malloc(size*sizeof(unsigned long));
				fread(tmp,size,sizeof(unsigned long),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
		case TYPE_FLOAT:
			{
				float *tmp = (float *)malloc(size*sizeof(float));
				fread(tmp,size,sizeof(float),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
		case TYPE_DOUBLE:
			{
				double *tmp = (double *)malloc(size*sizeof(double));
				fread(tmp,size,sizeof(double),fp);
				for(long i=0;i<size;i++) {
					data[i] = (double)tmp[i];
				}
				free(tmp);
			}
			break;
	}
	fclose(fp);
	return 1;
}

void Sample::Save(char *folder) {
	char filename[1024];
	strcpy(filename,folder);
	if(filename[strlen(filename)-1]!='\\') {
		strcat(filename,"\\");
	}
	char *filename2,*filename22;
	filename2 = strrchr(this->filename,'\\');
	filename22 = strrchr(this->filename,'/');
	if(filename22 > filename2) filename2 = filename22;
	if(!filename2) {
		filename2 = this->filename;
	} else {
		filename2++;
	}
	strcat(filename,filename2);
	FILE *fp;
	fp = fopen(filename,"wb");
	fwrite(data,size,sizeof(double),fp);
	fclose(fp);
}

SampleSet::SampleSet() {
	numsamples = 0;
	samples = NULL;
};

SampleSet::~SampleSet() {
	if(samples) {
		for(long i=0;i<numsamples;i++) {
			if(samples[i]) delete samples[i];
		}
		free(samples);
	}
}

void SampleSet::Merge(SampleSet *other) {
	int i,oldsize;
	oldsize = numsamples;
	numsamples += other->numsamples;
	samples = (Sample **)realloc(samples,numsamples * sizeof(Sample *));

	for(i=0;i<other->numsamples;i++) {
		samples[i+oldsize] = new Sample(other->samples[i]->Size());

		samples[i+oldsize]->SetClassname(other->samples[i]->GetClassname());
		samples[i+oldsize]->SetFilename(other->samples[i]->GetFilename());
	
		samples[i+oldsize]->SetData(other->samples[i]->Size(), other->samples[i]->GetData());
	}
}

void SampleSet::Init(long numsamples, int dim) {
	Clear();
	this->numsamples = numsamples;
	samples = (Sample **)malloc(numsamples * sizeof(Sample *));
	for(long i=0;i<numsamples;i++) {
		if(dim)	samples[i] = new Sample(dim);
		else samples[i] = new Sample();
	}
}

long SampleSet::Load(char *filename,int type, long size) {
	FILE *fp;
	fp = fopen(filename,"r");
	if(!fp) return 0;
	long N = 0;
	char line[SAMPLE_FILE_SIZE+SAMPLE_CLASS_SIZE+2];
	while(fgets(line,SAMPLE_FILE_SIZE+SAMPLE_CLASS_SIZE+2,fp)) N++;
	Clear();
	numsamples = N;
	samples = (Sample **)malloc(N * sizeof(Sample *));
	fseek(fp,0,SEEK_SET);
	long i = 0;
	while(fgets(line,SAMPLE_FILE_SIZE+SAMPLE_CLASS_SIZE+2,fp)) {
		samples[i] = new Sample;
		char filename2[SAMPLE_FILE_SIZE+SAMPLE_CLASS_SIZE+2];
		char classname[SAMPLE_FILE_SIZE+SAMPLE_CLASS_SIZE+2] = "";
		sscanf(line,"%s %s",filename2,classname);
		samples[i]->Load(filename2,classname,type,size);
		i++;
	}
	fclose(fp);
	return N;
}

void SampleSet::Save(char *folder) {
	long i;
	for(i=0;i<numsamples;i++) {
		samples[i]->Save(folder);
	}
}

Sample SampleSet::GetAvgSample() {
	long i,j;
	long size = samples[0]->Size();
	Sample avg(size);
	for(i=0;i<numsamples;i++) {
		for(j=0;j<size;j++) {
			avg[j] += (*samples[i])[j];
		}
	}
	for(j=0;j<size;j++) {
		avg[j] /= numsamples;
	}
	return avg;
}

Sample SampleSet::GetAvgSampleOfClass(char *classname) {
	long i,j,k = 0;
	long size = samples[0]->Size();
	Sample avg(size);
	avg.SetClassname(classname);
	for(i=0;i<numsamples;i++) {
		if(strcmp(samples[i]->GetClassname(),classname)!=0) continue;
		for(j=0;j<size;j++) {
			avg[j] += (*samples[i])[j];
		}
		k++;
	}
	for(j=0;j<size;j++) {
		avg[j] /= k;
	}
	return avg;
}

//collumns are samples
Matrix SampleSet::GetAsMatrix() {
	long i,j,m,n;
	m = samples[0]->Size();		//rows
	n = numsamples;				//collumns
	Matrix mat(m,n);
	for(i=0;i<m;i++) {
		for(j=0;j<n;j++) {
			mat[i][j] = (*samples[j])[i];
		}
	}
	return mat;
}

Matrix SampleSet::GetWithinClassVariance() {
	long i,j,k;
	long n,N;
	n = samples[0]->Size();
	N = numsamples;

	Matrix W(n,n);

	//get number of classes and class averages
	long numclasses=GetNumberOfClasses();
	Sample *avgClassSamples = new Sample[numclasses];
	long tmpnumclasses = 0;
	for(i=0;i<N;i++) {
		bool found = false;
		for(j=i-1;j>=0;j--) {
			if(strcmp(samples[i]->GetClassname(),samples[j]->GetClassname())==0) found = true;
		}
		if(!found) {
			avgClassSamples[tmpnumclasses] = GetAvgSampleOfClass(samples[i]->GetClassname());
			tmpnumclasses++;
		}
	}

	//get the within-class variance matrix
	long classno = 0;
	double *tmp = (double *)malloc(n*sizeof(double));
	for(i=0;i<N;i++) {
		for(j=0;j<numclasses;j++) {
			if(strcmp(samples[i]->GetClassname(),avgClassSamples[j].GetClassname())==0) {
				classno = j;
				break;
			}
		}
		for(j=0;j<n;j++) {
			tmp[j] = ((*samples[i])[j]) - (avgClassSamples[classno][j]);
		}
		for(j=0;j<n;j++) {
			for(k=0;k<j;k++) {
				W[j][k] += tmp[j]*tmp[k];
				W[k][j] += tmp[j]*tmp[k];
			}
		}
		for(j=0;j<n;j++) W[j][j] += tmp[j]*tmp[j];
	}

	free(tmp);
	delete [] avgClassSamples;
	return W;
}

Matrix SampleSet::GetBetweenClassVariance() {
	long i,j,k;
	long n,N;
	n = samples[0]->Size();
	N = numsamples;

	Matrix B(n,n);

	//get number of classes and class averages
	long numclasses=GetNumberOfClasses();
	Sample avg = this->GetAvgSample();
	Sample *avgClassSamples = new Sample[numclasses];
	long *classno = (long *)malloc(numclasses*sizeof(long));
	long tmpnumclasses = 0;
	for(i=0;i<N;i++) {
		bool found = false;
		for(j=i-1;j>=0;j--) {
			if(strcmp(samples[i]->GetClassname(),samples[j]->GetClassname())==0) {
				found = true;
			}
		}
		if(!found) {
			avgClassSamples[tmpnumclasses] = GetAvgSampleOfClass(samples[i]->GetClassname());
			tmpnumclasses++;
		}
	}
	for(i=0;i<numclasses;i++) {
		classno[i]=0;
		for(j=0;j<N;j++) {
			if(strcmp(avgClassSamples[i].GetClassname(),samples[j]->GetClassname())==0) {
				classno[i]++;
			}
		}
	}

	//get the between-class variance matrix
	double *tmp = (double *)malloc(n*sizeof(double));
	for(i=0;i<numclasses;i++) {
		for(j=0;j<n;j++) {
			tmp[j] = (avgClassSamples[i][j]) - (avg[j]);
		}
		for(j=0;j<n;j++) {
			for(k=0;k<j;k++) {
				B[j][k] += classno[i]*tmp[j]*tmp[k];
				B[k][j] += classno[i]*tmp[j]*tmp[k];
			}
		}
		for(j=0;j<n;j++) B[j][j] += classno[i]*tmp[j]*tmp[j];
	}

	free(tmp);
	free(classno);
	delete [] avgClassSamples;
	return B;
}

long SampleSet::GetNumberOfClasses() {
	long i,j,numclasses = 0;
	long N = numsamples;
	for(i=0;i<N;i++) {
		bool found = false;
		for(j=i-1;j>=0;j--) {
			if(strcmp(samples[i]->GetClassname(),samples[j]->GetClassname())==0) {
				found = true;
			}
		}
		if(!found) {
			numclasses++;
		}
	}
	return numclasses;
}

void SampleSet::Clear() {
	if(samples) {
		for(long i=0;i<numsamples;i++) {
			if(samples[i]) delete samples[i];
		}
		free(samples);
	}
}

} //namespace
