#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <fstream>
#include <direct.h>
#include "jni.h"
#include "Create_Model.h"
#include <iostream>
#include <iomanip>

#define MAX_CLUSTERS 2
#define MAX_ITER 200
#define EPS 0.0000001
using namespace std;
using namespace cv;

void Extract_Key_Features(int , char[] , char[] , int);
void Get_Train_Samples(char[] , Mat *);
void Save_Model(EM , char[]);
double Calculate_Threshold(const char * , const char * , char[] , int);
double Get_Mean(double* , int);
double Get_Std_Deviation(double* , double , int);
void Get_Key_Details(struct key *);

ifstream read_file;
CvCapture* capture;
IplImage *frame, *img;

struct key
{
	char name;
	int day;
	int month;
	int year;
	int key_hr;
	int key_mnts;
	double key_sec;
};

JNIEXPORT void JNICALL Java_project_Create_1Model_Make_1Model(JNIEnv *env, jobject obj, jstring name , jstring no , jint count)
{
	jboolean blnIsCopy1 , blnIsCopy2 ;
	const char *username= (env)->GetStringUTFChars(name, &blnIsCopy1);
	const char *roll_no = (env)->GetStringUTFChars(no, &blnIsCopy2);
	char make_dir_loc[200], str[30],str1[30],read_loc[200],write_loc[200];
	struct CvTermCriteria criteria;
	int file_count = count;
	printf("%d\n",file_count);
	double threshold;
	ofstream threshold_file;


	sprintf(make_dir_loc ,"C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s" , username , roll_no); 
	mkdir(make_dir_loc);

	sprintf(make_dir_loc ,"C://Users//dell//Documents//DataCollectionGUI//User Models/%s_%s" , username , roll_no);
	mkdir(make_dir_loc);

	strcpy(str , "name");
	strcpy(str1,username);

	for(int i = 1 ; i <=3 ; i++)
	{
		sprintf(make_dir_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s",username , roll_no , str);
		mkdir(make_dir_loc);

		sprintf( read_loc , "C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Keyboard Database/%s/Two Hand/" , username , roll_no , str);
		sprintf( write_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/",username , roll_no , str);
		printf("leaving to extract fetures fn frm make model \n");
		Extract_Key_Features(strlen(str1) , read_loc , write_loc , file_count);
		printf("we are back from extract features fn into make model\n");
		sprintf( read_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/", username , roll_no , str);
		/* just for checking*/
		printf("\n");
		int temp=strlen(str);
		printf("%d\n",temp);
		printf("\n");
		temp=strlen(str1);
		printf("\n%d\n",temp);
		printf("\n");
		//end of checking
		Mat train_samples(file_count ,  (2 * strlen(str1) - 1)  , CV_64F);
		printf("leaving to get train samples fn frm make model \n");
		Get_Train_Samples(read_loc , &train_samples);
		printf("we are back from get train samples fn into make model\n");
		criteria.type = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS ;
		criteria.max_iter = MAX_ITER;
		criteria.epsilon = EPS;
		EM user(MAX_CLUSTERS , EM::COV_MAT_DIAGONAL , criteria);
		user.train( train_samples);

		sprintf(write_loc ,"C://Users//dell//Documents//DataCollectionGUI//User Models/%s_%s/%s.yml" , username , roll_no , str);
		Save_Model(user , write_loc);

		sprintf(write_loc ,"C://Users//dell//Documents//DataCollectionGUI//User Models/%s_%s/%s_threshold.txt" , username , roll_no , str);
		threshold = Calculate_Threshold(username , roll_no , str , file_count);
		threshold_file.open(write_loc);
		threshold_file << threshold;
		threshold_file.close();
		if(i==1)
		{
			strcpy(str , "roll no");
			strcpy(str1,roll_no);
		}
		else
		{
			strcpy(str,"psswd_electronics");
			strcpy(str1,"electronics");
		}
	}
	(env)->ReleaseStringUTFChars(name , username);
	(env)->ReleaseStringUTFChars(no , roll_no);
}

void Extract_Key_Features(int data_length , char read_loc[] , char write_loc[] , int file_count)
{

	ofstream write_file;
	int file_no , count;
	char loc[50] , file_loc[200] , key;
	string keyevent;
	struct key keydown1 , keydown2 , keydown3 , keyup1 , keyup2 , keyup3;
	int status1_down = false , status2_down = false , status1_up = false , status2_up = false , mnts , status3_down = false , status3_up = false;

	for ( file_no = 1 ; file_no <= file_count ; file_no++ )
	{
		strcpy(file_loc , read_loc);
		sprintf( loc , "%d.txt" ,file_no);
		strcat(file_loc , loc);
		read_file.open(file_loc);
		if(!read_file.is_open())
		{
			cout << "not able to open the read file in extract features fn" << endl;
			exit(1);
		}

		strcpy(file_loc , write_loc);
		sprintf( loc , "%d.txt",file_no);
		strcat(file_loc , loc);
		write_file.open(file_loc);

		if(!write_file.is_open())
		{
			cout << "not able to open the write file in extract features fn" << endl;
			exit(1);
		}

		while(read_file >> keyevent)
		{
			if(keyevent == "KeyDown")
			{
				if(status2_down == true)
				{
					read_file >> keydown3.name;
					Get_Key_Details(&keydown3);
					status3_down = true;
				}
				else if(status1_down == true)
				{
					read_file >> keydown2.name;
					Get_Key_Details(&keydown2);

					status2_down = true;
				}
				else
				{
					read_file >> keydown1.name;
					Get_Key_Details(&keydown1);
					//cout << "keydown1 = " << keydown1.name << endl;
					status1_down = true;
				}
			}
			else if(keyevent =="KeyUp")
			{
				read_file >> key;
				//cout << "key = " << key << endl;
				if(key == keydown3.name)
				{
					keyup3.name = keydown3.name;
					Get_Key_Details(&keyup3);
					status3_up = true;
				}
				else if(key == keydown2.name)
				{
					keyup2.name = keydown2.name;
					Get_Key_Details(&keyup2);
					//cout << "keyup2 = " << keyup2.key_sec << endl;
					status2_up = true;
				}
				else if(key == keydown1.name)
				{
					keyup1.name = keydown1.name;
					Get_Key_Details(&keyup1);
					//cout << "key up1 = " << keyup1.key_sec << endl;
					status1_up = true;
				}
			}

			if(status1_down == true && status2_down == true && status1_up == true && status2_up == true)
			{
				cout << "keydown2 = " << keydown2.key_sec << endl;
				cout << "keydown1 = " << keydown1.key_sec << endl;
				cout << "key up1 = " << keyup1.key_sec << endl;
				cout << "key up2 = " << keyup2.key_sec << endl;
				write_file << fixed << setprecision(3);

				mnts = (keyup1.key_hr - keydown1.key_hr) * 60 + keyup1.key_mnts;
				double holdtime1 = ((mnts - keydown1.key_mnts) * 60 + keyup1.key_sec) - keydown1.key_sec;

				mnts = (keydown2.key_hr - keydown1.key_hr) * 60 + keydown2.key_mnts;
				double latency = ((mnts - keydown1.key_mnts) * 60 + keydown2.key_sec) - keydown1.key_sec;

				cout << holdtime1 << "\t" << latency <<  "\t" << endl;
				write_file << holdtime1 << "\t" << latency <<  "\t";

				keydown1 = keydown2;
				keyup1 = keyup2;
				keydown2.name = ' ';
				keyup2.name = ' ';
				status2_down = false;
				status2_up = false;
				if(status3_down == true)
				{
					keydown2 = keydown3;
					keydown3.name = ' ';
					status2_down = true;
					status3_down = false;

				}
				if(status3_up == true)
				{
					keyup2 = keyup3;
					keyup3.name = ' ';
					status2_up = true;
					status3_up = false;
				}
			}
		}

		if(status1_down == true && status2_down == false && status1_up == true && status2_up == false)
		{
			cout << "keydown2 = " << keydown2.key_sec << endl;
			cout << "keydown1 = " << keydown1.key_sec << endl;
			cout << "key up1 = " << keyup1.key_sec << endl;
			cout << "key up2 = " << keyup2.key_sec << endl;
			write_file << fixed << setprecision(3);

			mnts = (keyup1.key_hr - keydown1.key_hr) * 60 + keyup1.key_mnts;
			double holdtime1 = ((mnts - keydown1.key_mnts) * 60 + keyup1.key_sec) - keydown1.key_sec;
			cout << holdtime1 << "\t" ;
			write_file << holdtime1 << "\t";
		}

		else if(status1_down == true && status2_down == true && status1_up == true && status2_up == true)
		{
			cout << "keydown2 = " << keydown2.key_sec << endl;
			cout << "keydown1 = " << keydown1.key_sec << endl;
			cout << "key up1 = " << keyup1.key_sec << endl;
			cout << "key up2 = " << keyup2.key_sec << endl;
			write_file << fixed << setprecision(3);

			mnts = (keyup1.key_hr - keydown1.key_hr) * 60 + keyup1.key_mnts;
			double holdtime1 = ((mnts - keydown1.key_mnts) * 60 + keyup1.key_sec) - keydown1.key_sec;

			mnts = (keydown2.key_hr - keydown1.key_hr) * 60 + keydown2.key_mnts;
			double latency = ((mnts - keydown1.key_mnts) * 60 + keydown2.key_sec) - keydown1.key_sec;

			mnts = (keyup2.key_hr - keydown2.key_hr) * 60 + keyup2.key_mnts;
			double holdtime2 = ((mnts - keydown2.key_mnts) * 60 + keyup2.key_sec) - keydown2.key_sec;

			cout << holdtime1 << "\t" << latency <<  "\t";
			write_file << holdtime1 << "\t" << latency <<  "\t" << holdtime2 << "\t";
		}
		read_file.close();
		write_file.close();
		status1_down = status2_down = status3_down = status1_up = status2_up = status3_up = false;
	}
}

void Get_Train_Samples(char read_loc[] , Mat *train_samples)
{
	ifstream read_file;
	char loc[100] , file_loc[200];
	double number;

	for(int i = 0; i < train_samples->rows ; i++)
	{
		strcpy(file_loc , read_loc);
		sprintf( loc , "%d.txt", i+1);
		strcat(file_loc , loc);
		read_file.open(file_loc);
		if(!read_file.is_open())
		{
			cout << "not able to open the read file in Get train samples fn" << endl;
			exit(1);
		}
		for(int j = 0; j < train_samples->cols; j++)
		{
			read_file >> number ;
			train_samples->at<double>(i,j) = number;
		}
		read_file.close();
	}
}

void Save_Model(EM user , char loc[])
{
	FileStorage fs(loc,FileStorage::WRITE);

	if((user.isTrained()))
	{
		user.write(fs);
		fs.release();
	}
	else
	{
		cout << "Model cannot be saved" << endl;
		exit(1);
	}
}

double Calculate_Threshold(const char * username , const char *roll_no , char str[] , int file_count)
{
	struct CvTermCriteria criteria;
	ifstream read_file;
	double number , *result_buf , mean , std_dev;
	Mat Predict_Sample(1 , (2 * strlen(str) - 1) , CV_64F);
	Mat train_samples(file_count - 1 ,  (2 * strlen(str) - 1)  , CV_64F);
	Vec2d pred_res;
	char loc[200];

	result_buf = (double *) malloc(file_count * sizeof(double));
	if(result_buf == NULL)
	{
		cout << "memory cannot be allocated in claculate thresholod fn" << endl;
		exit(1);
	}

	criteria.type = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS ;
	criteria.max_iter = MAX_ITER;
	criteria.epsilon = EPS;
	EM user(MAX_CLUSTERS , EM::COV_MAT_DIAGONAL , criteria);

	for ( int iter = 1 ; iter <= file_count ; iter++ )
	{	
		for ( int i = 1 ; i <= file_count ; i++)
		{
			sprintf( loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/%d.txt", username , roll_no , str , i);
			read_file.open(loc);
			if(!read_file.is_open())
			{
				cout << "not able to open the file in calculate threshold fn" << endl;
				exit(1);
			}
			for(int j = 0; j < train_samples.cols; j++)
			{
				read_file >> number;

				if(i == iter)
					Predict_Sample.at<double>(0,j) = number;

				else if( i < iter)
					train_samples.at<double>(i-1,j) = number;
				else
					train_samples.at<double>(i-2,j) = number;
			}
			read_file.close();
		}
		cout << "train_samples=" << train_samples << endl;
		user.train( train_samples);

		pred_res = user.predict(Predict_Sample);
		cout << "thr_res=" << pred_res(0) << endl;
		result_buf[iter - 1] = pred_res(0);
	}

	mean = Get_Mean(result_buf , file_count);
	std_dev = Get_Std_Deviation(result_buf , mean , file_count);
	cout << "std dev = " << std_dev << endl;
	return (mean - std_dev);
}

double Get_Mean(double* result_buf , int no_samples)
{
	double Sum = 0.0 , Mean;
	int Count = 0;

	for(int k=0 ; k < no_samples ; k++)
	{
		//if(result_buf[k] > 0.0)
		//{
		Count++;
		Sum = Sum + result_buf[k];
		//}
	}
	Mean = Sum / Count;
	return Mean;
}

double Get_Std_Deviation(double* result_buf , double Mean , int no_samples)
{
	double Std_Dev = 0.0;
	int Count = 0;

	for(int k=0 ; k < no_samples ; k++)
	{
		//if(result_buf[k] > 0.0)
		//{
		Std_Dev = Std_Dev + ( (result_buf[k]- Mean) * (result_buf[k]- Mean) ) ;
		Count++;
		//}
	}
	Std_Dev = sqrt ( Std_Dev / Count ) ;
	return Std_Dev;
}

JNIEXPORT jint JNICALL Java_project_Create_1Model_Test_1Model(JNIEnv * env, jobject obj, jstring name, jstring no,jint count)
{
	jboolean blnIsCopy1 , blnIsCopy2 ;
	const char *username= (env)->GetStringUTFChars(name , &blnIsCopy1);
	const char *roll_no = (env)->GetStringUTFChars(no , &blnIsCopy2);
	char read_loc[200] , write_loc[200] , str[30],str1[30];
	struct CvTermCriteria criteria;
	double threshold , sum1 =0.0 , result = 0.0;
	ifstream threshold_file;
	Vec2d pred_res;
	int file_count=count;
	strcpy(str , "name");
	strcpy(str1,username);

	cout << "rahul" << endl;
	for(int i = 1 ; i <=3 ; i++)
	{
		sprintf( read_loc , "C://Users//dell//Documents//DataCollectionGUI//New Data Collection/%s_%s/Keyboard Database/%s/test" , username , roll_no , str);
		sprintf( write_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/test",username , roll_no , str);
		printf("leaving to extract fetures fn frm test model \n");
		Extract_Key_Features(strlen(str1) , read_loc , write_loc , 1);
		printf("we are back from extract features fn into test model\n");
		sprintf( read_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/", username , roll_no , str);
		Mat train_samples(file_count ,  (2 * strlen(str1) - 1)  , CV_64F);
		
		printf("leaving to get train samples fn frm test model \n");
		Get_Train_Samples(read_loc , &train_samples);
		printf("we are back from get train samples fn into test model\n");
		sprintf( read_loc , "C://Users//dell//Documents//DataCollectionGUI//Key Features/%s_%s/%s/test", username , roll_no , str);
		Mat test_sample(1 ,  (2 * strlen(str1) - 1)  , CV_64F);
		printf("leaving to get train samples fn frm test model \n");
		Get_Train_Samples(read_loc , &test_sample);
		printf("we are back from get train samples fn into test model\n");
		cout << "test sample = " << test_sample << endl;

		criteria.type = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS ;
		criteria.max_iter = MAX_ITER;
		criteria.epsilon = EPS;
		EM user(MAX_CLUSTERS , EM::COV_MAT_DIAGONAL , criteria);
		user.train( train_samples);

		sprintf(read_loc ,"C://Users//dell//Documents//DataCollectionGUI//User Models/%s_%s/%s_threshold.txt" , username , roll_no , str);
		threshold_file.open(read_loc);
		threshold_file >> threshold;
		threshold_file.close();

		sum1 = sum1 + threshold;

		pred_res = user.predict(test_sample);
		result = result + pred_res(0);
		cout <<"result=" <<result << endl;
		if(i==1)
		{
			strcpy(str , "roll no");
			strcpy(str1,roll_no);
		}
		else
		{
			strcpy(str,"psswd_electronics");
			strcpy(str1,"electornics");
		}
	}
	(env)->ReleaseStringUTFChars(name , username);
	(env)->ReleaseStringUTFChars(no , roll_no);

	threshold = sum1 / 3;
	result = result / 3;
	cout <<"result=" <<result << endl;
	cout << "thresold=" << threshold << endl; 

	if(result >= threshold)
		return 1;
	else
		return 0;
}

void Get_Key_Details(struct key *key_pressed)
{
	char ch ;
	int sec , msec;

	read_file >> key_pressed->day >> ch;
	//cout << "day = " << key_pressed->day << endl;
	read_file >> key_pressed->month >> ch;
	//cout << "month = " << key_pressed->month << endl;
	read_file >> key_pressed->year >> ch;
	//cout << "year =" << key_pressed->year << endl;
	read_file >> key_pressed->key_hr >> ch;
	//cout << "hr = " << key_pressed->key_hr << endl;
	read_file >> key_pressed->key_mnts >> ch;

	read_file >> sec >> ch;

	read_file >> fixed >> setprecision(3);
	read_file >> msec ;

	key_pressed->key_sec = (sec *1000 + msec)/1000.0;
	//cout << "sec =" << key_pressed->key_sec << endl;
}



/*JNIEXPORT void JNICALL Java_project_Create_1Model_Enable_1Camera(JNIEnv *, jobject)
{
	 capture = cvCaptureFromCAM( CV_CAP_ANY );
      
	 if ( !capture ) 
	 {
        fprintf( stderr, "ERROR: capture is NULL \n" );
        getchar();
//        return -1;
     }
    
    cvNamedWindow( "mywindow",CV_WINDOW_FREERATIO);					// Create a window in which the captured images will be presented 
	int count = 0;
    while (count < 50) 
	{                                                              // Show the image captured from the camera in the window and repeat																// Get one frame
        frame = cvQueryFrame( capture );
        if ( !frame ) 
		{
            fprintf( stderr, "ERROR: frame is null...\n" );
            getchar();
            break;
        }
        cvShowImage( "mywindow", frame );							
		//resizeWindow("mywindow" , 450 , 450);
		//moveWindow("mywindow", 800 ,25);
		if ( (cvWaitKey(10) & 255) == 's' ) {
            CvSize size = cvGetSize(frame);
			
            img= cvCreateImage(size, IPL_DEPTH_16S, 1);
            img = frame;
            cvSaveImage("matteo.jpg",img);
			cout << "rahul" << endl;
			
            }
        if ( (cvWaitKey(10) & 255) == 27 ) break;
		count++;
	}
	// cvDestroyWindow( "mywindow1" );
	cvNamedWindow( "mywindow1",CV_WINDOW_FREERATIO);	
	cvShowImage( "mywindow1", img );
	cvDestroyWindow( "mywindow1" );
	cvReleaseCapture( &capture );
    cvDestroyWindow( "mywindow" );
}
        if ( (cvWaitKey(10) & 255) == 's' ) {
            CvSize size = cvGetSize(frame);
			
            IplImage* img= cvCreateImage(size, IPL_DEPTH_16S, 1);
            img = frame;
            cvSaveImage("matteo.jpg",img);
			cout << "rahul" << endl;
			
            }
     if ( (cvWaitKey(10) & 255) == 27 ) break;
    }
    // Release the capture device housekeeping
    cvReleaseCapture( &capture );
    cvDestroyWindow( "mywindow" );
}
JNIEXPORT void JNICALL Java_project_Create_1Model_Capture_1Image(JNIEnv *, jobject)
{
	CvSize size = cvGetSize(frame);
	img= cvCreateImage(size, IPL_DEPTH_16S, 1);
	img = frame;
	cvNamedWindow( "mywindow1",CV_WINDOW_FREERATIO);
	cvShowImage( "mywindow1", img );
}*/