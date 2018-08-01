


#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>

#include <iostream>
#include <fstream>
#include <windows.h>
//#include <dirent.h>
//#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{  

   
		const std::string facesdirectory  =  "C:\\Users\\GUGULOTHU PANDU\\Documents\\temp1\\examples\\temp";
           
  
     
        dlib::array<array2d<unsigned char> > images_train, images_test;
        std::vector<std::vector<rectangle> > face_boxes_train, face_boxes_test;

        
        load_image_dataset(images_train, face_boxes_train, facesdirectory+"\\lables2.xml");
        //load_image_dataset(images_test, face_boxes_test, faces_directory+"/testing.xml");
        
        upsample_image_dataset<pyramid_down<2> >(images_train, face_boxes_train);
      
        add_image_left_right_flips(images_train, face_boxes_train);
        cout << "num training images: " << images_train.size() << endl;
        //cout << "num testing images:  " << images_test.size() << endl;


       
        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
        image_scanner_type scanner;
        // The sliding window detector will be 80 pixels wide and 80 pixels tall.
        scanner.set_detection_window_size(80,80); 
	//	unsigned long a = get_fhog_window_width()const;
        structural_object_detection_trainer<image_scanner_type> trainer(scanner);
		 scanner.set_padding(0); 
		// unsigned long y = scanner.get_feature_extactor();
		//  cout<<y<<endl;
		unsigned long x = scanner.get_num_dimensions(); 
		  cout<<x<<endl;
		unsigned long a = scanner.get_fhog_window_width();
		cout<<a<<endl;
		unsigned long b = scanner.get_fhog_window_height();
		cout<<b<<endl;
		unsigned long c = scanner.get_cell_size();
		cout<<c<<endl;
		unsigned long d = scanner.get_detection_window_height();
		cout<<d<<endl;
		system("pause");
        // Set this to the number of processing cores on your machine.
     trainer.set_num_threads(4);  
        
    trainer.set_c(1.1);
        // We can tell the trainer to print it's progress to the console if we want.  
    trainer.be_verbose();
       
      trainer.set_epsilon(0.01);

	 remove_unobtainable_rectangles(trainer, images_train, face_boxes_train);

       
     object_detector<image_scanner_type> detector = trainer.train(images_train, face_boxes_train);

       
	 //  detector = threshold_filter_singular_values(detector,0);
    cout << "training results: " << test_object_detection_function(detector, images_train, face_boxes_train) << endl;
        
     //  cout << "testing results:  " << test_object_detection_function(detector, images_test, face_boxes_test) << endl;
	unsigned long k = detector.get_w(0).size();
	cout <<k<<endl;
	unsigned long l = detector.get_w(1).size();
	cout <<l<<endl;
	unsigned long m = detector.get_w(1).size();
	cout <<m<<endl;
      //  This next line creates a
        // window with such a visualization of our detector.  It should look somewhat like
        // a face.
     image_window hogwin(draw_fhog(detector), "Learned fHOG detector");
		  
        
   image_window win; 
   std::vector<rectangle> dets ;
   unsigned long z=0;
        for (unsigned long i = 0; i<10; ++i)
        {
            // Run the detector and get the face detections.
           dets = detector(images_train[i]);
		  
           win.clear_overlay();
           win.set_image(images_train[i]);
           win.add_overlay(dets, rgb_pixel(255,0,0));
           cout << "Hit enter to process the next image..." << endl;
          cin.get();
        }

      serialize("face_detector2.svm") << detector;

        // recall it using the deserialize() function.
   //     object_detector<image_scanner_type> detector2;
    //    deserialize("face_detector.svm") >> detector2;
	system("pause");

     
       // We can see how many separable filters are inside your detector like so:
     cout << "num filters: "<< num_separable_filters(detector) << endl;
	 system("pause");
   
}

// ----------------------------------------------------------------------------------------

