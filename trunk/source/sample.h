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


#define TYPE_CHAR 0
#define TYPE_UCHAR 1
#define TYPE_INT 2
#define TYPE_UINT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_IMAGE 6

#define SAMPLE_FILE_SIZE 1000
#define SAMPLE_CLASS_SIZE 20

namespace LibSubspace {

extern long typesize[];

class Matrix;

//implements a single sample
//the sample contains a feature vector (array of double), class name and file name
class Sample {
protected:
	long size;	//size of the feature vector
	double* data;	//feature vector
	char filename[SAMPLE_FILE_SIZE];	//filename, used for loading and storing, useful for i.e. seeing where the classification fails
	char classname[SAMPLE_CLASS_SIZE];	//name of the class of the current sample
	
public:

	//constructors/destructor
	Sample();
	Sample(int size);
	~Sample();
	Sample(const Sample& src);
	Sample& operator=(const Sample &src);

	//property getters and setters
	long Size() {
		return size;
	}

	double* GetData() {
		return data;
	}

	double GetData(int i) {
		return data[i];
	}

	double &operator[] (int i) {
		return data[i];
	}

	char *GetFilename() {
		return filename;
	}

	char *GetClassname() {
		return classname;
	}

	void SetFilename(char *filename);
	void SetClassname(char *classname);

	//creates a sample with 'size' features, all initially set to 0
	void Init(int size);

	//sets a sample feature vector
	//a vecror is of size 'size', copied from 'data'
	void SetData(int size, double *data);

	//sets the feature vector as gray pixel values from an image in file 'filename'
	//image data is stored in a vector in a row-oriented manner
	//returns 1 on success, 0 on failure
	int ReadImageData(char *filename);

	//loads a sample from file. A file is assumed to be an array of type denoted by the 'type' param
	//params:
	//	filename : a name of the file to load a sample from
	//	classname : name of the sample class
	//	type : denotes the type of the data stored in file, for example TYPE_CHAR assumes signed 8-bit values
	//	size : the number of features to be read
	//returns 1 on success, 0 on failure
	int Load(char *filename, char *classname, int type, long size);

	//saves the sample data to file
	//the file is saved in folder passed via 'folder' param
	//the filename is copied from the 'filename' property
	void Save(char *folder);
};


//implements a collection of samples, for example a training or a test set
class SampleSet {
protected:
	long numsamples;	//number of samples in the set
	Sample **samples;	//samples

public:
	//constructor/destructor
	SampleSet();
	~SampleSet();

	//creates the sample set with 'numsamples' samples, all of the size 'dim'
	void Init(long numsamples, int dim = 0);

	//clears the sample set (deletes all samples)
	void Clear();

	//property getters
	long Size() {
		return numsamples;
	}

	Sample &operator[] (int i) {
		return *(samples[i]);
	}

	Sample* GetSample(int i) {
		return samples[i];
	}

	//loads a sample set described in a text file, organized as
	//<sample file name 1> <class of sample 1>
	//<sample file name 2> <class of sample 2>
	//...
	//Parameters:
	//	filename : the name of the text file as described above
	//	type : the individual sample files are assumed to contain araay of this type, see Sample.Load()
	//	size : the feature vector length of each sample
	long Load(char *filename,int type = TYPE_DOUBLE, long size = 0);
	
	//saves all the samples in folder given as 'folder' param
	//all samples are stored in files containing an array of double
	//filenames are obtained from the properties of individual samples
	void Save(char *folder);

	//gets the average (mean) sample
	Sample GetAvgSample();

	//gets the average (mean) of samples with class given as 'classname' parameter 
	Sample GetAvgSampleOfClass(char *classname);

	//creates a matrix composed of samples
	//each COLUMN of a matrix is one sample
	Matrix GetAsMatrix();

	//creates a within-class variance matrix of samples
	Matrix GetWithinClassVariance();

	//creates a between-class variance matrix of samples
	Matrix GetBetweenClassVariance();

	//gets the number of different classess in the sample set
	long GetNumberOfClasses();

	//merges the sample set with another sample set ('other')
	//all samples from 'other' are added to this sample set
	//other sample set remains intact
	void Merge(SampleSet *other);
};

} //namespace

