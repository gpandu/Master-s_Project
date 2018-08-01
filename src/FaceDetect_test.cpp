//#include "stdafx.h" 
//#include <WinSock.h>
#include "opencv2\highgui\highgui.hpp" 
#include <opencv2\imgproc\imgproc.hpp>
//#include<vector>
#include <opencv\highgui.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>



 
using namespace dlib;
using namespace std;
using namespace cv;

// ----------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{

	//for (int j =1;j<6;j++){
	cv::VideoCapture cap(0); // open the video camera no. 0
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 460);
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}
	cv::Mat temp;
	cap.read(temp);

	//cv::waitKey(100);
	//temp = cv::imread("lena.bmp");
	//image_window win;
	typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
	image_scanner_type scanner;
	// The sliding window detector will be 80 pixels wide and 80 pixels tall.
	scanner.set_detection_window_size(80, 80);
	object_detector<image_scanner_type> detector;
	deserialize("face_detector50.svm") >> detector;
	// Loop over all the images provided on the command line.
	for (int i =0; i<10; i++)
	{
		cap.read(temp);
		resize(temp, temp, Size(640, 460), 0, 0, INTER_LINEAR);
		cv_image<bgr_pixel> cimg(temp);
		cv::Mat roi;
		cv::Rect rec;
		
		std::vector<dlib::rectangle> faces = detector(cimg);
		cout << "Number of faces detected: " << faces.size() << endl;
		
		Mat image = temp.clone();
		
		for (int j = 0; j < faces.size(); j++)
		{
			//if(faces[j].top()<0)
				//faces[j].top() = 0;
			cv::Point pt1(faces[j].left(), faces[j].top());
			cv::Point pt2(faces[j].right(), faces[j].bottom());
			cv::rectangle(image, pt1, pt2, cvScalar(0, 255, 0), 2, 8, 0);
			cout<< faces[j].left()<<endl;
			cout << faces[j].top()<<endl;
			cout << faces[j].right()<<endl;
			cout << faces[j].bottom()<<endl;
		//rec = cv::Rect(faces[i].left(),faces[i].top(), faces[i].right(), faces[i].bottom());
			rec = cv::Rect(pt1, pt2);
			cout<< rec.size();
			//cout << rec<<endl;
			roi = temp(rec);
		
	    	sprintf(filename, "test_%d.jpg", i);
	        cv::imwrite(filename,roi);  */
	        imshow("temp",image);
		    imshow("result",roi);
		    cv::waitKey(1);
		}
		
	
	}

	cv::waitKey();
	system("pause");
	return 0;

}

// ----------------------------------------------------------------------------------------

