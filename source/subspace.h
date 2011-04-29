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


#define SUBSPACE_PCA 0
#define SUBSPACE_LDA 1

namespace LibSubspace {

//contains all information about a subspace
class Subspace {
	friend class SubspaceProjector;

protected:
	long type;	//indicates a subspace type
	long subspaceDim;	//dimensionality of the subspace
	long originalDim;	//original dimensionality of samples
	double *centerOffset;	//the offset between the center of original space and the subspace, it is to be substracted prior to projection
	double *subspaceAxes;	//row-ordered array containing a subspaceDim x originalDim matrix of subspace axis
	double *axesCriterionFn;		//values giving hints of relevance of the axis, for example the value of LDA criterion function of axis
	
public:
	//constructor/destructor
	Subspace();
	~Subspace();

	//property getters
	long GetType() {
		return type;
	}

	long GetSubspaceDim() {
		return subspaceDim;
	}

	long GetOriginalDim() {
		return originalDim;
	}

	double *GetCenterOffset() {
		return centerOffset;
	}

	double *GetSubspaceAxes() {
		return subspaceAxes;
	}

	double *GetAxesCriterionFn() {
		return axesCriterionFn;
	}
	
	//sets all of the subspace data, see above for description of params
	void SetData(long type, long subspaceDim, long originalDim, double *centerOffset, 
		double *subspaceAxes, double *axesCriterionFn);

	//saves a subspace to a file given as filename
	int Save(char *filename);

	//saves a subspace to a file given as file pointer
	void Save(FILE *fp);

	//loads a subspace from file given as filename
	int Load(char *filename);

	//loads a subspace from file given as file pointer
	void Load(FILE *fp);

	//normalizes subspace axis to unit length
	void Normalize();

	//reoders the axis in the falling order of axesCriterionFn
	void ReorderDescending();

	//reoders the axis in the absolute falling order of axesCriterionFn
	void ReorderAbsDescending();

	//returns subspaceAxes as a matrix
	//each row of a matrix will be a single axis
	Matrix ToMatrix();

	//visualizes subspace axes as images (for example, eigenfaces)
	//parameters:
	//	n : the number of subspace axes to be visualized
	//	basename : the image filename for axis i will be formed as basename + i.bmp
	//	sizex & sizey : image height and width, if 0 the image is assumed to be square, with dimension sqrt(originalDim)
	void ToImages(long n, char *basename, long sizex = 0, long sizey = 0);
};

//interface for generating subspaces
class SubspaceGenerator {
public:
	bool verbose; //if true, prints information about subspace generation, including error messages

	SubspaceGenerator() {
		verbose = false;
	}

	//generates a subspace based on the training data contained in the sampleSet
	virtual int GenerateSubspace(SampleSet* sampleSet, Subspace* subspace)=0;
};

//generates a PCA subspace based on the training data
class PCASubspaceGenerator : public SubspaceGenerator {
public:
	//generates a PCA subspace based on the training data contained in the sampleSet
	int GenerateSubspace(SampleSet* sampleSet, Subspace* subspace);
};


//generates a LDA subspace based on the training data
class LDASubspaceGenerator : public SubspaceGenerator {
public:
	//PCA can (and sometimes must) be performed prior to LDA in order to reduce sample dimensionality
	//this parameter controlls the dimensionality to which the samples will be reduced prior to LDA
	//if Npca is 0 and (sample dimensionality)>(number of samples)-(number of classes), PCA has to be performed
	//and Npca will be set to (number of samples)-(number of classes)
	//otherwise, if Npca is 0 and (sample dimensionality)<=(number of samples)-(number of classes), PCA will not be performed
	long Npca;

	//constructor
	LDASubspaceGenerator() {Npca = 0;}

	//generates a LDA subspace based on the training data contained in the sampleSet
	int GenerateSubspace(SampleSet* sampleSet, Subspace* subspace);
};

//projects a sample set into a subspace
class SubspaceProjector {
protected:
	Subspace *subspace; //subspace to be used for projection
public:
	//constructor, sets the subspace to be used for projection
	SubspaceProjector(Subspace *subspace);

	//destructor
	~SubspaceProjector();

	//projects a single sample into subspace
	//params:
	//	originalSample : sample to be projected
	//	projectedSample : resulting sample in a new subspace
	//	dim : a dimensionality to which originalSample will be reduced
	//	      if set to 0, all available subspace axis will be used
	void ProjectSample(Sample *originalSample, Sample *projectedSample, int dim = 0);

	//projects a sample set into subspace
	//params:
	//	originalSamples : samples to be projected
	//	projectedSamples : resulting samples in a new subspace
	//	dim : a dimensionality to which originalSamples will be reduced
	//	      if set to 0, all available subspace axis will be used
	void ProjectSampleSet(SampleSet *originalSamples, SampleSet *projectedSamples, int dim = 0);
};

} //namespace

