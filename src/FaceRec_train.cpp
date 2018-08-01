/**
 * @file Facerec_train.cpp
 * @brief 
 * This is a part of Implementation of Face recognition using Sparse representation Classiffier 
 * Given all training samples,returns trained data into a matrix 'A'.
 * @mainpage  Face recongntion
*/

//#include "stdafx.h" 
//#include <WinSock.h>
#include <opencv2\highgui\highgui.hpp> 
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv\highgui.h>
//#include<vector>
#include <opencv\highgui.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include "jni.h"
#include "Create_Model.h"
#include <windows.h>
#include<vector>
#include <fstream>
#include <string>
#include <new>

using namespace cv;
using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------


Mat Train(std::vector<string> &sample_list);
cv::Mat temp;
cv::Mat roi;
cv::Mat images[10];
int images_count =0;
JNIEXPORT void JNICALL Java_project_Create_1Model_Enable_1Camera(JNIEnv *, jobject)

{
	cv::VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the video cam" << endl;
		return -1;
	}
	cap.read(temp);

	//cv::waitKey(100);
	//temp = cv::imread("lena.bmp");
	//image_window win;
	typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
	image_scanner_type scanner;
	// The sliding window detector will be 80 pixels wide and 80 pixels tall.
	scanner.set_detection_window_size(80, 80);
	object_detector<image_scanner_type> detector;
	deserialize("C://Users//dell//Documents//DataCollectionGUI//Backend//GMM_Modelling//GMM_Modelling//face_detector50.svm") >> detector;
	std::cout<<"File opened:";
	// Loop over all the images provided on the command line.
	cvNamedWindow("Face Detection",1);
	HWND hwnd = (HWND)cvGetWindowHandle("Face Detection");
 while(IsWindowVisible(hwnd)&&(images_count<10)) 
	{
		cap.read(temp);
		array2d<rgb_pixel> cimg;
		array2d<rgb_pixel>  emo;
		assign_image(cimg, cv_image<bgr_pixel>(temp));

		cv::Rect rec;
		std::vector<dlib::rectangle> faces = detector(cimg);
		std::cout << "Number of faces detected: " << faces.size() << endl;
		
		for (int j = 0; j < faces.size(); j++)
		{ 
			if( faces[j].top()<0){
			         faces[j].top() = 0;
			}
			if(faces[j].bottom()>temp.rows){
			       faces[j].bottom() =temp.rows;
			    
			}
			if( faces[j].left()<0){
			         faces[j].left() = 0;
			}
			if(faces[j].right()>temp.cols){
			       faces[j].right()=temp.cols;
			    
			}
			cv::Point pt1(faces[j].left(), faces[j].top());
			cv::Point pt2(faces[j].right(), faces[j].bottom());
			cv::rectangle(temp, pt1, pt2, cvScalar(0, 255, 0), 2, 8, 0);
			//rec = cv::Rect(faces[i].left(),faces[i].top(), faces[i].right(), faces[i].bottom());
			rec = cv::Rect(pt1,pt2);
			//cout << rec<<endl;
			roi = temp(rec);
			
	
	    IplImage ipl_from_mat((IplImage)temp);
	    cvShowImage("Face Detection", &ipl_from_mat); 
		}
		  if( cvWaitKey(50) == 27 ) break;
	
	}
   //cvReleaseImage(ipl_from_mat);
    cvDestroyWindow("Face Detection");
    images_count=0;
    cout<<"opencv video capture ended:"<<endl;
	//cv::waitKey();
	//system("pause");

}


JNIEXPORT void JNICALL Java_project_Create_1Model_Capture_1Image (JNIEnv *, jobject){

	     
	       for(int i= 0;i<10;++i){
			   if(roi.data!=NULL){
			       images[i]= roi;
		     IplImage ipl_from_mat((IplImage)images[i]);
		cvShowImage("Image captured", &ipl_from_mat);
		cvWaitKey(200);
		             
		   }
			   else  {
		                  i=i-1;
			   }
		 }
		    cvWaitKey();
}




JNIEXPORT void JNICALL Java_project_Create_1Model_Save_1Image
  (JNIEnv *env, jobject obj, jstring name,jstring no, jint count,jint train_or_test){
	  jboolean blnIsCopy1 , blnIsCopy2 ;
	const char *username= (env)->GetStringUTFChars(name , &blnIsCopy1);
	const char *roll_no = (env)->GetStringUTFChars(no, &blnIsCopy2);
	char make_dir_loc[200],str[30];
	       std::cout <<"user_name:"<<username<<endl;
		   std::cout <<"count:"<<count<<endl;
		   if(!train_or_test){
			   if(count<30){
		   for(int i=count;i<10+count;i++){
		           IplImage ipl_from_mat((IplImage)images[i-count]);
	  sprintf(make_dir_loc ,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/train_data/%d.bmp" , username , roll_no,i); 
				   cvSaveImage(make_dir_loc, &ipl_from_mat);
		   ++images_count;
		   }
		       cout<<"training samples are collected:"<<endl;
		   }
		   else if(count>=30){
		             for(int i=0;i<10;i++){
		           IplImage ipl_from_mat((IplImage)images[i]);
	  sprintf(make_dir_loc ,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/train_data/%d.bmp" , username , roll_no,i); 
				   cvSaveImage(make_dir_loc, &ipl_from_mat);
		   ++images_count;
					 }
					 cout<<"training samples are collected:"<<endl;
		   }
		}  
		   else {
			    for(int i=0;i<10;i++){
		           IplImage ipl_from_mat((IplImage)images[i]);
	  sprintf(make_dir_loc ,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/test_data/%d.bmp" , username , roll_no,i); 
				   cvSaveImage(make_dir_loc, &ipl_from_mat);
		   ++images_count;

		   }
				 cout<<"test samples are collected:"<<endl;
		   }

		      
		   (env)->ReleaseStringUTFChars(name , username);
		   (env)->ReleaseStringUTFChars(no , roll_no);
		  
}


 /// \brief  LoadSamples function
    /// \param  list_file: A string argument to store sample_list file name
    /// \param  sample_list:  A vector of strings to hold image file names

void LoadSamples(
	const string &list_file,	// sample list file
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
	//cout << sample_list->size();
}
    
int Loaded(string check_name,std::vector<string> &sample_list){
	std::cout << "identity:"<<check_name<<endl;;
	int count = 0;
	   for(int i=0;i<sample_list.size();i++){
		   if(check_name==sample_list[i]){
		   
		   count = count+1;
		   }
	   }
	   return count;
}


/// \brief  Train function for Face recontion
      /// \param  sample_list:  A vector of strings that has image file names
      /// \return  A:  Mat object that has trained data  

Mat Train(std::vector<string> &sample_list){
	unsigned int list_size = sample_list.size();
	cout <<"new_list_size:"<<list_size<<endl;
	size_t x = 14;     
	size_t y = 16;

	Size sample_size(x, y);         // size of each image to be resized  
	Mat  A = Mat(Size(sample_list.size(),224), CV_64FC1);            // Matrix that stores the trained data
	cout << "new size of A:" << A.size() << endl;
	Mat outimage;
	for (int i = 0; i < sample_list.size(); i++){
		                                                                //Loads each image file and adds it to trained data
		Mat image = imread(sample_list[i].c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		                                                                  
		resize(image,image,sample_size, 0, 0, CV_INTER_LINEAR);      // resizes the given image to  appropriate sample size      
		
	Mat temp = image.clone();
		
		temp = temp.reshape(0, image.rows*image.cols);          //forms image pixels into a coloumn vector
		temp.assignTo(temp, CV_64FC1);
		
		Mat roi = A(Range::all(), Range(i, i + 1));    //takes one coloumn of Matrix A 
		
		temp.copyTo(roi);                                // copy image coloumn vector into the coloum of A
		normalize(roi, roi, 1.0, CV_L2);                    //Normalize vector for minization problem of SRC
		
	}

	
	double *k = A.ptr<double>();


	FileStorage fs;
	fs.open("C://Users/dell/Documents/DataCollectionGUI/Face_Model/Final1.yml", FileStorage::WRITE);
	if (fs.isOpened())
	{
		std::cout << "model File is opened\n"<<endl;
	}
	// Write to file!
	fs << "datapart" << A;
	fs.release();
	//cout << A.size() << endl;
	std::cout << "Model file is created"<<endl;
	//Mat final = A.clone();
	//normalize(final,final, 0, 1, NORM_MINMAX);
	//namedWindow("large", CV_WINDOW_AUTOSIZE);
	//imshow("large", final);
	return A;
}

 
// ----------------------------------------------------------------------------------------

/// \brief  Model Face make_MOdel function for Face recognition
      /// \param  name:  name of the person
      /// \param  no: roll no of the person
     /// \param  count: no of images already present in database


JNIEXPORT void JNICALL Java_project_Create_1Model_Face_1Make_1Model(JNIEnv *env, jobject obj, jstring name,jstring no,jint count){

	                       //Load Frames from Camera
	 jboolean blnIsCopy1 , blnIsCopy2 ;
	const char *username= (env)->GetStringUTFChars(name , &blnIsCopy1);
	const char *roll_no = (env)->GetStringUTFChars(no, &blnIsCopy2);
	char check_name[200],str[30];
	       std::cout <<"user_name:"<<username<<endl;
	 sprintf(check_name,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/train_data/0.bmp" , username , roll_no);
	string sample_list_file = "C://Users/dell/Documents/DataCollectionGUI/Face_Model/yaleb001.txt";
	std::vector<std::string> sample_list;
	//string user_images= ; 
	                                                 // loads file containing image file names
	LoadSamples(sample_list_file, &sample_list);
	cout << "training_list_size:"<<sample_list.size() << endl;
	std::cout <<"taining_images_count:"<<count<<endl;
	if(count>=30){
		                                             // checks if already loaded in database
		         int match_count = Loaded(check_name,sample_list);
	              cout << "match_count:"<<match_count<<endl;
				  if(match_count<1){
		for(int i=0;i<30;i++){
			sprintf(check_name,"C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Face Database/train_data/%d.bmp" , username , roll_no,i);
		       ofstream ofile;
					ofile.open(sample_list_file, ios::out | ios::app);
					ofile << "\n";
					ofile << check_name;
					sample_list.push_back(check_name);
	
	}
				  } 
				                   // function call to train all the data
		Mat A = Train(sample_list);
		cout<<"Mat A trained:"<<endl;
	}

	else{
		cout <<"Face_Model can not be created with less no of samples:"<<endl;
	              
	}
}



// ----------------------------------------------------------------------------------------

