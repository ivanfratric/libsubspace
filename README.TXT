##################
LibSubspace readme
##################


#################
Table of contents
#################

1. Overview
2. Dependancies
3. Overview of Libsubspace classes
4. Using the library
5. Example application
6. License


###########
1. Overview
###########

LibSubspace is a C++ library for pattern recognition in subspaces. The library implements classes and functions for reading and writing of samples, learning subspaces based on training data and preforming classification tests. Currently, subspace generation based on principal component analysis (PCA) and linear discriminant analysis (LDA) is implemented. Given a set of image samples, the library also supports learning and testing of local subspaces, for example Local Binary LDA (LBLDA) subspace, as was described in the paper:
I. Fratric, S. Ribaric, "Local Binary LDA for Face Recognition", Lecture notes in Computer science, Vol. 6583, 2011, pp. 144-155. (http://bib.irb.hr/datoteka/511724.lblda.pdf).


###############
2. Dependancies
###############

LibSubspace uses LAPACK (http://www.netlib.org/lapack/) routines to perform eigenanalysis of matrices. Appropriate LAPACK and BLAS libraries should be included in order for the LibSubspace to be built successfully.
For example, under Linux, the following command line options should be added in gcc (assuming lapack, lapack_atlas and blas libraries have been successfully installed):
-llapack_atlas -llapack -lblas
For example, under windows, compiled clapack libraries (available at http://www.netlib.org/clapack/), should be included in the project.


###############################
Overview of Libsubspace classes
###############################

Libsubspace provides the following basic classes (and some other, helper classes). For the full description of interface of each class, please refer to the corresponding header files.
All classes are defined in the LibSubspace namespace.

Image
A simple image class with some basic image processing capabilities. Contains various GetPixel methods for accessing image elements and SetPixel methods for writing image elements.

ImageIO
Implements image file input/output operations. Use LoadImage(char *filename, Image *image) to load an image from file and SaveImage(char *filename, Image *image) to store an image from file. Currently, the following uncompressed image formats are supported: .bmp, .pgm, .ppm, .raw.

Matrix
Implements the basix matrix operations

Sample
Contains an information about a single sample: feature vector, sample (file)name and sample class. Basic file IO operations are also provided

SampleSet
A collection of samples, for example a training set to be used for learning the subspace. A SampleSet can be read from a text file organized as follows
<sample file name 1> <class of sample 1>
<sample file name 2> <class of sample 2>
<sample file name 3> <class of sample 3>
...
The SampleSet class also provides other methods, for example for accessing the matrix of the contained samples and computing their between-class and within-class variance matices

Subspace
Contains all information about a subspace

SubspaceGenerator
Interface for generating a subspace based on the training samples

PCASubspaceGenerator
Implements SubspaceGenerator, performs subspace generation based on principal component analysis

LDASubspaceGenerator
Implements SubspaceGenerator, performs subspace generation based on linear discriminant analysis

SubspaceProjector
Projects a SampleSet into subspace

LocalSubspace
Contains all information about a local subspace

LocalSubspaceGenerator
Creates a local subspace based on the SubspaceGenerator and a set of training samples. It is assumed the samples are images. LocalSubspaceGenerator first divides an image into regions based on the sliding window approach and then performs subspace generation for each region separately. Finally, local basis from all regions are sorted in the descending value of their criterion function. For more detailed description of local subspace generation, see for example the paper
I. Fratric, S. Ribaric, "Local Binary LDA for Face Recognition", Lecture notes in Computer science, Vol. 6583, 2011, pp. 144-155.

LocalSubspaceProjector
Extracts features from a sample set using a local subspace

OneNNClassifier
Performs classification experiments with sample sets based on the minimum distance classifier


####################
4. Using the library
####################

Example 1: Learning a PCA subspace based on the training data. The training data is given in files organized as vectors of unsigned 8-bit values. The sample file and class names are given in the learn_db.txt file

using namespace LibSubspace;
SampleSet learnsamples;
learnsamples.Load("learn_db.txt",TYPE_UCHAR,4096);
PCASubspaceGenerator generator;
Subspace subspace;
generator.GenerateSubspace(&learnsamples,&subspace);
subspace.Save("subspace.dat");


Example 2: Learning a LDA subspace based on the training data. The training data samples are stored as images. The sample file and class names are given in the learn_db.txt file. Performs PCA prior to LDA and reduces the sample dimensionality to 500

using namespace LibSubspace;
SampleSet learnsamples;
learnsamples.Load("learn_db.txt",TYPE_IMAGE);
LDASubspaceGenerator generator;
generator.Npca = 500;
Subspace subspace;
generator.GenerateSubspace(&learnsamples,&subspace);
subspace.Save("subspace.dat");


Example 3: Performing a classification experiment in the subspace stored in the file "subspace.dat". The samples are stored as images. Extracts first 100 features from samples using the given subspace. Normalized correlation is used as a measure of sample matching.

using namespace LibSubspace;
SampleSet learnsamples, testsamples;
learnsamples.Load("learn_db.txt",TYPE_IMAGE);
testsamples.Load("test_db.txt",TYPE_IMAGE);
SampleSet projectedLearnsamples, projectedTestsamples;
Subspace subspace;
subspace.Load("subspace.dat");
SubspaceProjector projector(&subspace);
projector.ProjectSampleSet(&learnsamples,&projectedLearnsamples);
projector.ProjectSampleSet(&testsamples,&projectedTestsamples);
OneNNClassifier classifier;
classifier.distanceMeasure = DISTANCE_COSINE;
float accuracy = classifier.ClassificationTest(&projectedLearnsamples,&projectedTestsamples,100);


Example 4: Learning a local subspace based on the LDA.
Parameters are:
Image width: 64 pixels
Image height: 64 pixels
Sliding window size: 16 pixels
Sliding window translation step: 4 pixels
Number of PCA components: 100
Number of local features: 1500

using namespace LibSubspace;
SampleSet learnsamples;
learnsamples.Load("learn_db.txt",TYPE_IMAGE);
LDASubspaceGenerator generator;
generator.Npca = 100;
LocalSubspaceGenerator localGen(&generator, 64, 64, 16, 4, 1500);
LocalSubspace subspace;
localGen.GenerateSubspace(&learnsamples, &subspace);
subspace.Save("subspace.dat");


Example 5: Performing a classification experiment with a local subspace learned as in the previous example. Hamming distance is used (only the signs of features are used for distance computation)

using namespace LibSubspace;
SampleSet learnsamples, testsamples;
learnsamples.Load("learn_db.txt",TYPE_IMAGE);
testsamples.Load("test_db.txt",TYPE_IMAGE);
SampleSet projectedLearnsamples, projectedTestsamples;
LocalSubspace subspace;
subspace.Load("subspace.dat");
LocalSubspaceProjector projector(&subspace);
projector.ProjectSampleSet(&learnsamples,&projectedLearnsamples);
projector.ProjectSampleSet(&testsamples,&projectedTestsamples);
OneNNClassifier classifier;
classifier.distanceMeasure = DISTANCE_HAMMING;
float accuracy = classifier.ClassificationTest(&projectedLearnsamples,&projectedTestsamples,100);


######################
5. Example application
######################

An example command-line application that uses LibSubspace is provided in the library. The application supports learning subspaces and performing classification experiments in subspaces. For the instructions on using the application, invoke it without parameters.


##########
6. License
##########

The library is given under the MIT license.

