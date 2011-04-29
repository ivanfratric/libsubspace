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

class Image;

//describes an image region/blob/patch
class LocalDescriptor {
protected:
	long size; //number of pixels contained in the region
	long *indices; //for each pixel in the region, index of that pixel in the feature vector

public:
	//constructor/destructor
	LocalDescriptor();
	~LocalDescriptor();

	//property getters
	long Size() {
		return size;
	}

	long *GetIndices() {
		return indices;
	}

	//creates a descriptor for a square patch
	//parameters:
	//	originalWidth : image width
	//	originalHeight : image height
	//	localwidth : patch width and height
	//	posx & posy : coordinates of the upper-left corner of the patch
	void Init(long originalWidth, long originalHeight, long localwidth, long posx, long posy);

	//saves a descriptor to a file
	void Save(FILE *fp);

	//loads descriptor from a file
	void Load(FILE *fp);
};

//describes a local feature
struct LocalFeature {
	long descriptorindex; //index of image region/blob/patch
	long subspaceindex; //subspace index corresponding to the image region
	long axisindex; //index of the basis vector isnide the subspace
	double value; //value  giving indication about the goodness of the feature
};

//contains all information about a local subspace
class LocalSubspace {
	friend class LocalSubspaceGenerator;
	friend class LocalSubspaceProjector;

protected:
	long numLocalDescriptors; //number of image regions/patches/blobs
	LocalDescriptor *localDescriptors; //descriptors for each image region/patch/blob

	long numLocalSubspaces; //number of local subspaces (possibly one for each image region, but does not have to be so)
	Subspace *localSubspaces; //local subspace (axis) data

	long numFeatures; //number of features fo be extracted
	LocalFeature *features; //descriptor for each feature

public:
	//constructor
	LocalSubspace();
	
	//destructor
	~LocalSubspace();

	//clears all data
	void Clear();

	//saves local subspace data to a file named 'filename'
	void Save(char *filename);

	//loads local subspace data from a file named 'filename'
	int Load(char *filename);

	//creates a visualization of local subspace,
	//cretes an image with lighter areas corresponding to the regions from which more features are taken
	//parameters:
	//   width : image width, must be the same as in the images used to create the local subspace
	//   height : image height, must be the same as in the images used to create the local subspace
	//   ncomponents : total number of features to take into account when creating the visualization
	//                 if less than numFeatures, only the first ncomponents will be used
	//   image : out parameter used to return the image
	void ToImage(int width, int height, int ncomponents, Image *image);
};


//creates a local subspace from a sample set
class LocalSubspaceGenerator {
protected:
	SubspaceGenerator *subspaceGenerator; //subspace generator to be used to obtain local features
	long originalWidth; //width of the images in the sample set
	long originalHeight; //height of the images in the sample set
	long windowWidth; //width of the sliding window used to create image patches
	long windowStep; //translation step of the sliding window
	long numFeatures; //maximum number of local features, if larger than the maximum possible number of feaures, features will be trimmed according to corresponding criterion function

	//creates local descriptors for localSubspace by using a sliding window approach
	void InitLocalDescriptors(LocalSubspace *localSubspace);

	//creates a subspace for each image region based on the learn set (originalSampleSet) and stores it into localSubspace
	void CreateLocalSubspaces(SampleSet *originalSampleSet, LocalSubspace *localSubspace);

	//merges all local subspace data and creates an array of LocalFeature that will be used for feature extraction
	void MergeSubspaces(LocalSubspace *localSubspace);

	//comparator for local features
	static int FeatureCompare(const void *f1, const void *f2);

public:

	//constructor
	//parameters:
	//   subspaceGenerator : subspace generator to be used to obtain local features
	//   originalWidth : width of the images in the sample set
	//   originalHeight : height of the images in the sample set
	//   windowWidth : width of the sliding window used to create image patches
	//   windowStep : translation step of the sliding window
	//   numFeatures : maximum number of local features, if larger than the maximum possible number of feaures, features will be trimmed according to corresponding criterion function
	LocalSubspaceGenerator(SubspaceGenerator *subspaceGenerator, long imageWidth, long imageHeight, long windowWidth, long windowStep, long numFeatures);

	//destructor
	~LocalSubspaceGenerator();

	//creates local subspace (subspace) from the sample set (samples)
	void GenerateSubspace(SampleSet *samples, LocalSubspace *subspace);
};

//extracts local features from samples using local subspaces
class LocalSubspaceProjector {
	LocalSubspace *subspace;
public:
	//constructor, sets the subspace to be used for feature extraction
	LocalSubspaceProjector(LocalSubspace *subspace) {
		this->subspace = subspace;
	}

	//extracts local features from a single sample
	//params:
	//	originalSample : sample from which the featues will be extracted
	//	projectedSample : resulting feature vector
	//	dim : number of features to be extracted
	//	      if set to 0, all available features in the local subspace will be used
	void ProjectSample(Sample *originalSample, Sample *projectedSample, int dim = 0);

	//extracts local features from a sample set
	//params:
	//	originalSamples : samples from which the featues will be extracted
	//	projectedSamples : resulting feature vectors
	//	dim : number of features to be extracted
	//	      if set to 0, all available features in the local subspace will be used
	void ProjectSampleSet(SampleSet *originalSamples, SampleSet *projectedSamples, int dim = 0);
};

} //namespace

