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
#include <string.h>
#include <stdlib.h>

#include "matrix.h"
#include "sample.h"
#include "subspace.h"
#include "classifier.h"
#include "local.h"

using namespace LibSubspace;

void PrintUsage(char *appName) {
	printf("Usage: %s [task] [options]\n\n",appName);
	printf(" If task is 'learn', learns the subspace based on the training data\n");
	printf(" If task is 'test', performs a classification experiment in the\n");
	printf(" specified subspace\n");
	printf("\nOptions:\n");
	printf(" -learnset learnset  specifies a text file with learning samples\n");
	printf("                     if testset is not specified, and the task is 'test'\n");
	printf("                     leave-one-out experiment is performed\n");
	printf("                     Both the learnset and the testset should be arranged\n");
	printf("                     in a text file where each line contains a sample\n");
	printf("                     filename and the class name separated by space\n");
	printf(" -testset testset    specifies a text file with test samples\n");
	printf("                     if learnset is not specified, and the task is 'test'\n");
	printf("                     leave-one-out experiment is performed\n");
	printf("                     Both the learnset and the testset should be arranged\n");
	printf("                     in a text file where each line contains a sample\n");
	printf("                     filename and the class name separated by space\n");
	printf(" -sampletype type    specifies the format of the samples in learn/test set\n");
	printf("                     'char' an array of 8-bit signed values\n");
	printf("                     'uchar' an array of 8-bit unsigned values\n");
	printf("                     'int' an array of 32-bit signed values\n");
	printf("                     'uint' an array of 32-bit unsigned values\n");
	printf("                     'float' an array of 32-bit floating point values\n");
	printf("                     'double' an array of 64-bit floating point values\n");
	printf("                     'img' samples are uncompressed image files in\n");
	printf("                           one of the following formats: .bmp, .ppm, .pgm\n");
	printf(" -samplesize size    specifies the dimensionality of the samples\n");
	printf("                     If unspecified, the dimensionality will be determined\n");
	printf("                     based on the sample type and sample file size of the\n");
	printf("                     first sample\n");
	printf("                     If sampletype is 'img', samplesize will be ignored\n");
	printf("                     and image width x image height will be used instead\n");
	printf(" -subtype type       specifies a subspace to be learnd\n");
	printf("                     if 'pca', learns a subspace based on the PCA\n");
	printf("                     if 'lda', learns a subspace based on the LDA\n");
	printf(" -npca npca          if subspace type is 'lda', this param specifies\n");
	printf("                     a dimensionality to which the samples will be reduced\n");
	printf("                     prior to performing LDA\n");
	printf("                     If not specified, npca is determined automatically\n");
	printf(" -subfile name       name of the file to store a subspace to if learning\n");
	printf("                     or a name of the file to read a subspace from if testing\n");
	printf(" -dim size           feature vector dimensionality to be used in experiments\n");
	printf("                     and when learning local subspaces\n");
	printf("                     if not specified, full dimensionality of the subspace\n");
	printf("                     will be used\n");
	printf(" -dist type          distance measure to be used in experiments\n");
	printf("                     'euclid' Euclidean distance\n");
	printf("                     'nc' normalized correlation\n");
	printf("                     'hamming' normalized Hamming distance, only signs of\n");
	printf("                               features will be used\n");
	printf("                     if not specified, euclidean distance will be used\n");
	printf(" -local              learns or tests feature obtained using local instead of\n");
	printf("                     global subspaces\n");
	printf(" -w width            width of images used for learning local subspaces\n");
	printf(" -h height           height of images used for learning local subspaces\n");
	printf(" -winsize size       size of the sliding window to be used when learning\n");
	printf("                     local subspaces\n");
	printf(" -winstep step       translation step of the sliding window to be used when\n");
	printf("                     learning local subspaces\n");
	printf(" -v                  Verbose, prints detailed error messages and progress\n");
	printf("                     information\n");
	printf("\nExamples:\n");
	printf("\nExample 1:\n");
	printf("subspace learn -learnset face_db.txt -sampletype uchar -samplesize 4096 -subtype pca -subfile subspace.dat\n");
	printf("   Learns a PCA subspace and stores it in subspace.dat\n");
	printf("   Sample information is in face_db.txt file\n");
	printf("   Samples are stored as 4096-component vectors of unsigned 8-bit values\n");
	printf("\nExample 2:\n");
	printf("subspace learn -learnset face_db.txt -sampletype img -subtype lda -npca 200 -subfile subspace.dat -v\n");
	printf("   Learns a LDA subspace and stores it in subspace.dat\n");
	printf("   Performs PCA prior to lda and reduces dimensionality to 200 components\n");
	printf("   Sample information is in face_db.txt file\n");
	printf("   Samples are stored as images\n");
	printf("   Outputs progress information and error messages\n");
	printf("\nExample 3:\n");
	printf("subspace test -learnset face_learn_db.txt -testset face_test_db.txt -sampletype img -subfile subspace.dat -dist nc -dim 70\n");
	printf("   Performs a classification experiment in subspace stored in subspace.dat\n");
	printf("   Learn samples are in face_learn_db.txt, test samples in face_test_db.txt\n");
	printf("   Samples are stored as images\n");
	printf("   Normalized correlation is used as the matching measure\n");
	printf("   Feature vector dimensionality used in the experiment is 70\n");
	printf("\nExample 4:\n");
	printf("subspace learn -local -learnset face_db.txt -sampletype img -w 64 -h 64 -winsize 16 -winstep 4 -subtype lda -npca 100 -dim 1500 -subfile subspace.dat -v\n");
	printf("   Learns a local LDA subspace and stores it in subspace.dat\n");
	printf("   Performs PCA prior to lda and reduces dimensionality to 100 components\n");
	printf("   Sample information is in face_db.txt file\n");
	printf("   Samples are stored as images of 64x64 pixels\n");
	printf("   Image is divided into regions using a 16x16 sliding window\n");
	printf("   which is moved over image in steps of 4 pixels\n");
	printf("   Subspace will contain 1500 most discriminatory features\n");
	printf("\nExample 5:\n");
	printf("subspace test -local -learnset face_learn_db.txt -testset face_test_db.txt -sampletype img -subfile subspace.dat -dist hamming -dim 1500\n");
	printf("   Performs a classification experiment with local subspace stored in\n");
	printf("   subspace.dat\n");
	printf("   Learn samples are in face_learn_db.txt, test samples in face_test_db.txt\n");
	printf("   Samples are stored as images\n");
	printf("   Hamming distance is used as the matching measure\n");
	printf("   Feature vector dimensionality used in the experiment is 1500\n");	
}

int GetOption(int argc, char* argv[], const char *optionName, char **optionValue) {
	int i;
	for(i=1;i<argc;i++) {
		if(strcmp(optionName, argv[i])==0) {
			if(optionValue) {
				if(i==(argc-1)) return 0;
				if(argv[i+1][0]=='-') return 0;
				*optionValue = argv[i+1];
				return 1;
			}
			return 1;
		}
	}
	return 0;
}

int LearnSubspace(int argc, char* argv[]) {
	SubspaceGenerator *subGen = NULL;
	SampleSet learnSamples;
	Subspace subspace;

	char *option;
	char learnsamplefilename[1024];
	char subspacefilename[1024];
	int sampletype;
	long samplesize = 0;
	int subspacetype;

	//get all relevant options
	if(!GetOption(argc,argv,"-learnset",&option)) {
		printf("Missing option, 'learnset'\n");
		return 0;
	}
	strncpy(learnsamplefilename,option,1024);
	learnsamplefilename[1023]=0;

	if(!GetOption(argc,argv,"-subfile",&option)) {
		printf("Missing option, 'subfile'\n");
		return 0;
	}
	strncpy(subspacefilename,option,1024);
	subspacefilename[1023]=0;

	if(!GetOption(argc,argv,"-sampletype",&option)) {
		printf("Warning: missing sample type, assumed 'double'\n");
		sampletype = TYPE_DOUBLE;
	} else {
		if(strcmp(option,"char")==0) sampletype = TYPE_CHAR;
		else if(strcmp(option,"uchar")==0) sampletype = TYPE_UCHAR;
		else if(strcmp(option,"int")==0) sampletype = TYPE_INT;
		else if(strcmp(option,"uint")==0) sampletype = TYPE_UINT;
		else if(strcmp(option,"float")==0) sampletype = TYPE_FLOAT;
		else if(strcmp(option,"double")==0) sampletype = TYPE_DOUBLE;
		else if(strcmp(option,"img")==0) sampletype = TYPE_IMAGE;
		else {
			printf("Invalid option, unknown sample type, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-samplesize",&option)) {
		samplesize = atol(option);
	}

	if(!GetOption(argc,argv,"-subtype",&option)) {
		printf("Missing option, subspace type\n");
		return 0;
	} else {
		if(strcmp(option,"pca")==0) subspacetype = SUBSPACE_PCA;
		else if(strcmp(option,"lda")==0) subspacetype = SUBSPACE_LDA;
		else {
			printf("Invalid option, unknown subspace type, exiting\n");
			return 0;
		}
	}

	//load the sample set
	learnSamples.Load(learnsamplefilename,sampletype,samplesize);
	if(GetOption(argc,argv,"-v",NULL)) {
		printf("Loaded %ld samples\n", learnSamples.Size());
	}
	
	//create a subspace generator object and set its options
	if(subspacetype == SUBSPACE_PCA) {
		subGen = new PCASubspaceGenerator;
	} else if(subspacetype == SUBSPACE_LDA) {
		subGen = new LDASubspaceGenerator;
		if(GetOption(argc,argv,"-npca",&option)) {
			((LDASubspaceGenerator*)subGen)->Npca = atol(option);
		}
	}
	if(GetOption(argc,argv,"-v",NULL)) {
		subGen->verbose = true;
	}

	//generate subspace
	subGen->GenerateSubspace(&learnSamples, &subspace);
	delete subGen;

	//store subspace
	subspace.Save(subspacefilename);

	return 1;
}

int LearnLocalSubspace(int argc, char* argv[]) {
	SubspaceGenerator *subGen = NULL;
	SampleSet learnSamples;
	LocalSubspace subspace;

	char *option;
	char learnsamplefilename[1024];
	char subspacefilename[1024];
	int sampletype;
	long samplesize = 0;
	int subspacetype;
	int w,h,winstep,winsize;
	long dim = 0;

	//get all relevant options
	if(!GetOption(argc,argv,"-learnset",&option)) {
		printf("Missing option, 'learnset'\n");
		return 0;
	}
	strncpy(learnsamplefilename,option,1024);
	learnsamplefilename[1023]=0;

	if(!GetOption(argc,argv,"-subfile",&option)) {
		printf("Missing option, 'subfile'\n");
		return 0;
	}
	strncpy(subspacefilename,option,1024);
	subspacefilename[1023]=0;

	if(!GetOption(argc,argv,"-sampletype",&option)) {
		printf("Warning: missing sample type, assumed 'double'\n");
		sampletype = TYPE_DOUBLE;
	} else {
		if(strcmp(option,"char")==0) sampletype = TYPE_CHAR;
		else if(strcmp(option,"uchar")==0) sampletype = TYPE_UCHAR;
		else if(strcmp(option,"int")==0) sampletype = TYPE_INT;
		else if(strcmp(option,"uint")==0) sampletype = TYPE_UINT;
		else if(strcmp(option,"float")==0) sampletype = TYPE_FLOAT;
		else if(strcmp(option,"double")==0) sampletype = TYPE_DOUBLE;
		else if(strcmp(option,"img")==0) sampletype = TYPE_IMAGE;
		else {
			printf("Invalid option, unknown sample type, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-samplesize",&option)) {
		samplesize = atol(option);
	}

	if(!GetOption(argc,argv,"-w",&option)) {
		printf("Missing option: image width\n");
		return 0;
	}
	w = atoi(option);

	if(!GetOption(argc,argv,"-h",&option)) {
		printf("Missing option: image height\n");
		return 0;
	}
	h = atoi(option);

	if(!GetOption(argc,argv,"-winsize",&option)) {
		printf("Missing option: sliding window size\n");
		return 0;
	}
	winsize = atoi(option);

	if(!GetOption(argc,argv,"-winstep",&option)) {
		printf("Missing option: sliding window translation step\n");
		return 0;
	}
	winstep = atoi(option);

	if(GetOption(argc,argv,"-dim",&option)) {
		dim = atol(option);
	} else {
		printf("Warning: No feature vector dimensionality specified\n   all possible features will be used\n");
	}

	if(!GetOption(argc,argv,"-subtype",&option)) {
		printf("Missing option, subspace type\n");
		return 0;
	} else {
		if(strcmp(option,"pca")==0) subspacetype = SUBSPACE_PCA;
		else if(strcmp(option,"lda")==0) subspacetype = SUBSPACE_LDA;
		else {
			printf("Invalid option, unknown subspace type, exiting\n");
			return 0;
		}
	}

	//load the sample set
	learnSamples.Load(learnsamplefilename,sampletype,samplesize);
	if(GetOption(argc,argv,"-v",NULL)) {
		printf("Loaded %ld samples\n", learnSamples.Size());
	}
	
	//create a subspace generator object and set its options
	if(subspacetype == SUBSPACE_PCA) {
		subGen = new PCASubspaceGenerator;
	} else if(subspacetype == SUBSPACE_LDA) {
		subGen = new LDASubspaceGenerator;
		if(GetOption(argc,argv,"-npca",&option)) {
			((LDASubspaceGenerator*)subGen)->Npca = atol(option);
		}
	}
	if(GetOption(argc,argv,"-v",NULL)) {
		subGen->verbose = true;
	}

	LocalSubspaceGenerator localGen(subGen, w, h, winsize, winstep, dim);

	//generate subspace
	localGen.GenerateSubspace(&learnSamples, &subspace);
	delete subGen;

	//store subspace
	subspace.Save(subspacefilename);

	return 1;
}

int TestSubspace(int argc, char* argv[]) {
	SampleSet *learnSet = NULL, *testSet = NULL;
	SampleSet *projectedSamplesLearn = NULL, *projectedSamplesTest = NULL;

	char *option;
	char samplefilename[1024];
	char subspacefilename[1024];
	int sampletype;
	long samplesize = 0;
	long dim = 0;
	int dist = DISTANCE_EUCLIDEAN;
	float result;

	//get all relevant options
	if(!GetOption(argc,argv,"-subfile",&option)) {
		printf("Missing option, 'subfile'\n");
		return 0;
	}
	strncpy(subspacefilename,option,1024);
	subspacefilename[1023]=0;

	if(!GetOption(argc,argv,"-sampletype",&option)) {
		printf("Warning: missing sample type, assumed 'double'\n");
		sampletype = TYPE_DOUBLE;
	} else {
		if(strcmp(option,"char")==0) sampletype = TYPE_CHAR;
		else if(strcmp(option,"uchar")==0) sampletype = TYPE_UCHAR;
		else if(strcmp(option,"int")==0) sampletype = TYPE_INT;
		else if(strcmp(option,"uint")==0) sampletype = TYPE_UINT;
		else if(strcmp(option,"float")==0) sampletype = TYPE_FLOAT;
		else if(strcmp(option,"double")==0) sampletype = TYPE_DOUBLE;
		else if(strcmp(option,"img")==0) sampletype = TYPE_IMAGE;
		else {
			printf("Invalid option, unknown sample type, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-dist",&option)) {
		if(strcmp(option,"euclid")==0) dist = DISTANCE_EUCLIDEAN;
		else if(strcmp(option,"nc")==0) dist = DISTANCE_COSINE;
		else if(strcmp(option,"hamming")==0) dist = DISTANCE_HAMMING;
		else {
			printf("Invalid option, unknown distance measure, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-samplesize",&option)) {
		samplesize = atol(option);
	}

	if(GetOption(argc,argv,"-dim",&option)) {
		dim = atol(option);
	}

	//read samples
	if(GetOption(argc,argv,"-learnset",&option)) {
		strncpy(samplefilename,option,1024);
		samplefilename[1023]=0;
		learnSet = new SampleSet;
		learnSet->Load(samplefilename,sampletype,samplesize);
	}
	if(GetOption(argc,argv,"-testset",&option)) {
		strncpy(samplefilename,option,1024);
		samplefilename[1023]=0;
		testSet = new SampleSet;
		testSet->Load(samplefilename,sampletype,samplesize);
	}
	if(!learnSet&&!testSet) {
		printf("Error: neither learn set nor test set specified\n");
		return 0;
	} else if(!learnSet) {
		learnSet = testSet;
	} else if(!testSet) {
		testSet = learnSet;
	}

	//load subspce
	Subspace subspace;
	if(subspace.Load(subspacefilename)) {
		//project samples into subspace
		SubspaceProjector proj(&subspace);
		if(learnSet == testSet) {
			projectedSamplesLearn = new SampleSet;
			proj.ProjectSampleSet(learnSet,projectedSamplesLearn);
			projectedSamplesTest = projectedSamplesLearn;
		} else {
			projectedSamplesLearn = new SampleSet;
			projectedSamplesTest = new SampleSet;
			proj.ProjectSampleSet(learnSet,projectedSamplesLearn);
			proj.ProjectSampleSet(testSet,projectedSamplesTest);
		}

		//perform classification experiment
		OneNNClassifier classifier;
		classifier.distanceMeasure = dist;

		if(GetOption(argc,argv,"-v",NULL)) {
			classifier.verbose = true;
		}

		result = classifier.ClassificationTest(projectedSamplesLearn,projectedSamplesTest,dim);
		printf("Classification accuracy: %g%%\n",result*100);
	} else {
		printf("Error loading subspace\n");
	}	

	//cleanup
	if(learnSet == testSet) {
		if(learnSet) delete learnSet;
	} else {
		if(learnSet) delete learnSet;
		if(testSet) delete testSet;
	}
	if(projectedSamplesLearn == projectedSamplesTest) {
		if(projectedSamplesLearn) delete projectedSamplesLearn;
	} else {
		if(projectedSamplesLearn) delete projectedSamplesLearn;
		if(projectedSamplesTest) delete projectedSamplesTest;
	}

	return 1;
}

int TestLocalSubspace(int argc, char* argv[]) {
	SampleSet *learnSet = NULL, *testSet = NULL;
	SampleSet *projectedSamplesLearn = NULL, *projectedSamplesTest = NULL;

	char *option;
	char samplefilename[1024];
	char subspacefilename[1024];
	int sampletype;
	long samplesize = 0;
	long dim = 0;
	int dist = DISTANCE_EUCLIDEAN;
	float result;

	//get all relevant options
	if(!GetOption(argc,argv,"-subfile",&option)) {
		printf("Missing option, 'subfile'\n");
		return 0;
	}
	strncpy(subspacefilename,option,1024);
	subspacefilename[1023]=0;

	if(!GetOption(argc,argv,"-sampletype",&option)) {
		printf("Warning: missing sample type, assumed 'double'\n");
		sampletype = TYPE_DOUBLE;
	} else {
		if(strcmp(option,"char")==0) sampletype = TYPE_CHAR;
		else if(strcmp(option,"uchar")==0) sampletype = TYPE_UCHAR;
		else if(strcmp(option,"int")==0) sampletype = TYPE_INT;
		else if(strcmp(option,"uint")==0) sampletype = TYPE_UINT;
		else if(strcmp(option,"float")==0) sampletype = TYPE_FLOAT;
		else if(strcmp(option,"double")==0) sampletype = TYPE_DOUBLE;
		else if(strcmp(option,"img")==0) sampletype = TYPE_IMAGE;
		else {
			printf("Invalid option, unknown sample type, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-dist",&option)) {
		if(strcmp(option,"euclid")==0) dist = DISTANCE_EUCLIDEAN;
		else if(strcmp(option,"nc")==0) dist = DISTANCE_COSINE;
		else if(strcmp(option,"hamming")==0) dist = DISTANCE_HAMMING;
		else {
			printf("Invalid option, unknown distance measure, exiting\n");
			return 0;
		}
	}

	if(GetOption(argc,argv,"-samplesize",&option)) {
		samplesize = atol(option);
	}

	if(GetOption(argc,argv,"-dim",&option)) {
		dim = atol(option);
	}

	//read samples
	if(GetOption(argc,argv,"-learnset",&option)) {
		strncpy(samplefilename,option,1024);
		samplefilename[1023]=0;
		learnSet = new SampleSet;
		learnSet->Load(samplefilename,sampletype,samplesize);
	}
	if(GetOption(argc,argv,"-testset",&option)) {
		strncpy(samplefilename,option,1024);
		samplefilename[1023]=0;
		testSet = new SampleSet;
		testSet->Load(samplefilename,sampletype,samplesize);
	}
	if(!learnSet&&!testSet) {
		printf("Error: neither learn set nor test set specified\n");
		return 0;
	} else if(!learnSet) {
		learnSet = testSet;
	} else if(!testSet) {
		testSet = learnSet;
	}

	//load subspce
	LocalSubspace subspace;
	if(subspace.Load(subspacefilename)) {
		//project samples into subspace
		LocalSubspaceProjector proj(&subspace);
		if(learnSet == testSet) {
			projectedSamplesLearn = new SampleSet;
			proj.ProjectSampleSet(learnSet,projectedSamplesLearn);
			projectedSamplesTest = projectedSamplesLearn;
		} else {
			projectedSamplesLearn = new SampleSet;
			projectedSamplesTest = new SampleSet;
			proj.ProjectSampleSet(learnSet,projectedSamplesLearn);
			proj.ProjectSampleSet(testSet,projectedSamplesTest);
		}

		//perform classification experiment
		OneNNClassifier classifier;
		classifier.distanceMeasure = dist;

		if(GetOption(argc,argv,"-v",NULL)) {
			classifier.verbose = true;
		}

		result = classifier.ClassificationTest(projectedSamplesLearn,projectedSamplesTest,dim);
		printf("Classification accuracy: %g%%\n",result*100);
	} else {
		printf("Error loading subspace\n");
	}	

	//cleanup
	if(learnSet == testSet) {
		if(learnSet) delete learnSet;
	} else {
		if(learnSet) delete learnSet;
		if(testSet) delete testSet;
	}
	if(projectedSamplesLearn == projectedSamplesTest) {
		if(projectedSamplesLearn) delete projectedSamplesLearn;
	} else {
		if(projectedSamplesLearn) delete projectedSamplesLearn;
		if(projectedSamplesTest) delete projectedSamplesTest;
	}

	return 1;}


int main(int argc, char* argv[])
{
	if(argc<=1) {
		PrintUsage(argv[0]);
		return 0;
	}

	if(strcmp(argv[1],"learn")==0) {
		if(GetOption(argc, argv, "-local", NULL)) {
			LearnLocalSubspace(argc,argv);
		} else {
			LearnSubspace(argc,argv);
		}
	} else if(strcmp(argv[1],"test")==0) {
		if(GetOption(argc, argv, "-local", NULL)) {
			TestLocalSubspace(argc,argv);
		} else {
			TestSubspace(argc,argv);
		}
	} else {
		PrintUsage(argv[0]);
	}

	return 0;
}

