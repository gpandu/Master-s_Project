/**
 * @file Facerec_test.cpp
 * @brief 
 * This is a part of Implementation of Face recognition using Sparse representation Classiffier. 
 * Given a test sample, It will return id of the person if that subject is already stored 
 * in database otherwise returns 'unknown'.
 * @mainpage  Face recongntion
*/


#include <opencv2\highgui\highgui.hpp> 
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv\highgui.h>
//#include<vector>
//#include <opencv\highgui.h>
#include "jni.h"
#include "Create_Model.h"
#include <windows.h>
#include<vector>
#include <fstream>
#include <string>
#include <new>
#include<iostream>
#include "SRC.hpp"
//#include "lbp.hpp"

extern cv::Mat images[10];
unsigned int samples_per_subject=30;
//using namespace dlib;
using namespace std;
using namespace cv;
char claimed_identity[200]; 


int correct_identity(char test_name[], char claimed_identity[] ){
	int i =0;
	int count = 0;
	while(i<strlen(test_name)){
		if(test_name[i]== claimed_identity[i]){
		     ++count;
		}
		++i;
	}

	if(count==i){
	     return 1;
	}
	else
		return 0;
}

void Identity_claim(string identityofi){
	int i=0;
	while((identityofi[i]!='_')&&(i<200)){
		claimed_identity[i]= identityofi[i];
		i++;
	}


}

     /// \brief  LoadSamples function
    /// \param  list_file: A string argument to store sample_list file name
    /// \param  sample_list:  A vector of strings to hold image file names

void LoadSamples2(
	const string &list_file,	          // sample list file
	std::vector<std::string> *sample_list // sample list
	) {
	ifstream fin(list_file.c_str());
	string line;
	while (getline(fin, line,'\n')) {
		//line = "";
		//getline(fin, line, '\n');
		//if (line == "") { break; }
		sample_list->push_back(line);
	}
	fin.close();
}


      /// \brief  Test function for Face recognition
      /// \param  sample_list:  A vector of strings that has image file names
      /// \param  A:  Mat object that has trained data  
      /// \param  sci_t: Threshold value for classification
      /// \return an integer(identity of a person)
int Test(Mat image,Mat A,double sci_t){
	//unsigned int list_size = sample_list.size();           //no of image files in the list
	size_t x = 14;
	size_t y = 16;
	int id;
	Size sample_size(x, y);                         // size of each image to be resized
	
	Mat outimage;          
	resize(image, image, sample_size, 0, 0, CV_INTER_LINEAR);       // Resizes the images into required sample_size using linea interpolation                             
	Mat temp = image.clone();         //temporary Mat object to hold resized image
		                                            
	temp = temp.reshape(0, image.rows*image.cols); //forms image pixels into a coloumn vector
	temp.assignTo(temp, CV_64FC1);                //assigns pixels to floating point that suitable for SRC
	normalize(temp, temp, 1.0, CV_L2);               //Normalize vector for minization problem of SRC
	cv::waitKey(50);
	transpose(temp, temp);        
	std::cout << "y size:" << temp.size() << endl;
		             
   //actual function for Face Recogntion present in SRC.cpp					 
	 int person_id = Recognize(         
			A,  // Trained Matrix
			temp, // test sample
			sci_t); // Classification threshold
		
	
	return person_id;
}



/// \brief  Model Face Test_MOdel function for Face recognition
      /// \param  name:  name of the person
      /// \param  no: roll no of the person
      /// \return an integer(identity of a person)

JNIEXPORT jint JNICALL Java_project_Create_1Model_Face_1Test_1Model
	(JNIEnv *env, jobject obj, jstring name,jstring no){

		 jboolean blnIsCopy1 , blnIsCopy2 ;
	const char *username= (env)->GetStringUTFChars(name , &blnIsCopy1);
	const char *roll_no = (env)->GetStringUTFChars(no, &blnIsCopy2);
	int unknown =-1;
		Mat A;                            // Matrix to store the trained images
	                         // Load the file containing trained data
	FileStorage fs;
	fs.open("C:/Users/dell/Documents/DataCollectionGUI/Face_Model/Final1.yml", FileStorage::READ);
	if(!fs.isOpened())
	{
		cout << "File is not opened\n";
	}
	                     // Write to file!
	fs["datapart"] >> A;
	transpose(A, A);
	cout << "A size:" << A.size() << endl;
	 
	double sci_t = 0.3;
	                                       // loads file containing image file names
	string sample_list_file = "C://Users/dell/Documents/DataCollectionGUI/Face_Model/yaleb001.txt";
	std::vector<std::string> sample_list;
	char test_name[200];
	sprintf(test_name,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s" , username );
	cout <<"Entered into Loading samples:"<<endl;
	LoadSamples2(sample_list_file, &sample_list);                // Load all image file names into the vector of strings
	
	cout << "sample_list size:" << sample_list.size() << endl;
	if((sample_list.size()>A.cols)&&((A.rows%30)==0)){
		char test_sample[200];
		int id =-1;
		for(int i=0;i<10;i++){                        
		    sprintf(test_sample,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/test_data/%d.bmp" , username , roll_no,i);
		    Mat image = imread(test_sample, CV_LOAD_IMAGE_GRAYSCALE);           // load test images from directory
	        cout <<"Entered into test_model:"<<endl;
				if(image.data!=NULL){
	               int inter_id = Test(image,A,sci_t);           // function call to test face recognition
				   id = (inter_id>=0)?inter_id:id;
				   cout <<" test_model finished:"<<endl;
				}
		}
		
		if (id != (-1)){
			Identity_claim(sample_list[id*30]);   //Returns actual identity of the person   
			std::cout << "claimed_identiry:" <<claimed_identity<<endl;
			std::cout << "test_name:" <<test_name<<endl;
			cout << id<<endl;
			if(correct_identity(test_name,claimed_identity)){
			                                        //verfication of the claimed identity
			    std::cout << "Identity:" << id << endl;
			    std::cout << "name:" <<claimed_identity<<endl;
		        return 1;
			  }
			else{
			    std::cout <<"Not a claimed Identity"<<endl;
			    return -1;
			}
	    }
	    else{	
		    std::cout <<"Not a claimed Identity"<<endl;
	        return -1;
			}
			
	}
	else                                   //testing not possible for following cases
	{
		cout << "test can not be performed: sample_list_file.size()<A.rows or samples_per_subject<30"<<endl;
		return -1;
	}

}