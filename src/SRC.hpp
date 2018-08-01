
#pragma once

//#include <opencv\cxcore.hpp>
#include <opencv\cxcore.h>
#include <vector>
#include <string>

using namespace std;
using namespace cv;
// max

// square of x
#ifndef SQ
#define SQ(x) ((x)*(x))
#endif // SQ

// abs
#define ABS(x) ((x)<0?-(x):(x))

// ALM Stopping Criteria
enum ALMStoppingCriteria {
	ALMSTOPPING_GROUND_TRUTH = -1,
	ALMSTOPPING_DUALITY_GAP = 1,
	ALMSTOPPING_SPARSE_SUPPORT = 2,
	ALMSTOPPING_OBJECTIVE_VALUE = 3,
	ALMSTOPPING_SUBGRADIENT = 4,
	ALMSTOPPING_INCREMENTS = 5,
	ALMSTOPPING_DEFAULT = ALMSTOPPING_INCREMENTS
}; // enum ALMStoppingCriteria

     /// \file

     /// \brief  FastDALM function
    ///  \param  x: array of floating values which holds the Linear coefficients
     ///  \param  b: array of floating values which holds the test sample pixels
    ///  \param  A: array of floating values which holds the trained samples
    ///  \param  lamda: floating value which holds lambda value
     ///  \param  tol: floating value which tolerance value
     ///  \param  maxIter: maximum no of iterations
     ///  \param  m: sample_size.w*sample_size.h
      ///  \param  n: total no of samples
      ///  \param  xG: temporary array of NULL
      ///  \param  verbose: for status 
void FastDALM(
	double *const &x,
	int&  nIter,
	double *b,
	double *A,
	double lambda,
	double tol,
	int maxIter,
	int m,
	int n,
	ALMStoppingCriteria stop,
	double *xG,
	bool verbose = false
	);

        /*       // load sample list 
void LoadSampleList(
	const string &list_file,	        // sample list file
	std::vector<string> *sample_list   // sample list
	);
	*/
      /// \brief Delta Function to calculate delta_i(x)= coefficients corresponding to subject i
      ///  \param  x: Mat object which holds the Linear coefficients
      /// \param  n_subject_samples: size of no of samples for each subject
       /// \param  i: ith subject
       ///\param  delta_i_x: Mat object which holds the Linear coefficients of ith subject
            // calculate delta_i(x)
void DeltaFunction(
	 Mat &x,              // x, 1*n, n=k*n_subject_samples, k= n_subjects
	size_t n_subject_samples,      // sample count per subject
	size_t i,                 // subject id: 0, 1, ..., k-1
	Mat &delta_i_x                 // delta_i(x)
	);

           /// \brief Residuals Function to calculate square of residuals r_i(y)= ||y-A * delta_i(x)||_2
           /// \param  A: Mat object that has trained data
         ///  \param  x: Mat object which holds the Linear coefficients
        /// \param  y: Mat object that has test sample
//calculates square of residuals r_i(y)= ||y-A * delta_i(x)||_2
double Residuals(
	Mat &y,	// test sample
	CvMat &A, // train samples
	 CvMat &delta_i_x // delta_i(x)
	);
/// \brief  SCI function to calculate sparsity concentration index
      ///  \param  x: Mat object which holds the Linear coefficients
       /// \param  n_subject_samples: size of no of samples for each subject
       /// \return Floating value of SCI for the given test sample
//returns sparsity concentration index
double SCI(
	Mat &x,
	size_t n_subject_samples
	);

  

       /// \brief  Identity function
      /// \param  A: Mat object that has trained data
      ///  \param  x: Mat object which holds the Linear coefficients
      /// \param  y: Mat object that has test sample
      /// \param  sci_t: Threshold value for classification
      ///  \param  n_subject_samples: size of no of samples for each subject
      /// \return integer value:  Id of the test sample
int Identity(
	 Mat &A, // train samples
	Mat &x, // 
	Mat &y, // test sample
	double sci_t, // if SCI(x)<sci_t, return -1
	size_t n_subject_samples,
	std::vector<double> *r = NULL
	);

        /// \file

       /// \brief  Recognize function
      /// \param  A: Mat object that has trained data  
      /// \param  y: Mat object that has test sample
      /// \return integer value:  Id of the test sample


// recognize test sample
int Recognize(
	Mat A,      // Trained matrix
	 Mat y, // test sample
	double sci_t // if SCI(x)<sci_t, return "Unknown"
	);