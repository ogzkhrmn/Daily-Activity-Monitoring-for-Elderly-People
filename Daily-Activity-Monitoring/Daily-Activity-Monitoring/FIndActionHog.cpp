//opencv
#define NOMINMAX
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2\core.hpp>
#include <opencv2/opencv.hpp>
//C++
#include <sstream>
#include <iostream>
#include <sstream>
#include <OpenNI.h>
#include <string> 
#include <dirent.h>
#include <Windows.h>
   


using namespace openni;
using namespace cv;
using namespace std;

int computeHog(Mat im1,Mat im2) {
	HOGDescriptor d1(Size(64, 8), Size(8, 8), Size(4, 4), Size(4, 4), 9);
	HOGDescriptor d2(Size(64, 8), Size(8, 8), Size(4, 4), Size(4, 4), 9);

	vector< float> descriptorsValues1;
	vector< Point> locations1;
	d1.compute(im1, descriptorsValues1, Size(32, 32), Size(0, 0), locations1);

	vector< float> descriptorsValues2;
	vector< Point> locations2;
	d2.compute(im2, descriptorsValues2, Size(32, 32), Size(0, 0), locations2);
	Mat A(descriptorsValues1.size(), 1, CV_32FC1);
	//copy vector to mat  
	memcpy(A.data, descriptorsValues1.data(), descriptorsValues1.size()*sizeof(float));
	//create Mat  
	Mat B(descriptorsValues2.size(), 1, CV_32FC1);
	//copy vector to mat  
	memcpy(B.data, descriptorsValues2.data(), descriptorsValues2.size()*sizeof(float));
	Mat C = A - B;
	C = C.mul(C);
	cv::sqrt(C, C);
	cv::Scalar rr = cv::sum(C);
	float rrr = rr(0);
	cout << "Distance: " << rrr << endl;


	return 0;
}

int main() {

	
	bool isfirst=true;
	Mat myimage, myimage2;
	String s;
	myimage = imread("C:/images/actions/image118.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	vector<string> names;
	WIN32_FIND_DATA FindFileData;
	string img = "C:\\images/actions/*.bmp";
	HANDLE hFind = FindFirstFile(img.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else do {
		if (isfirst) {
			isfirst = false;
		}
		else {
			s = FindFileData.cFileName;
			s = "C:/images/actions/"+s +"";
			cout << FindFileData.cFileName << endl;
			cout << s << endl;
			myimage2 = imread(s, CV_LOAD_IMAGE_GRAYSCALE);
			computeHog(myimage,myimage2);
		}
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	return 0;
}