#include "ekranayazdir.h"
#include <string>
#include <iostream>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
using namespace std;
using namespace cv;
ekranayazdir::ekranayazdir(int sayi)
{
	hour =sayi;
}

void ekranayazdir::yazdir() {
	cout << "OpenCV Version: " << CV_VERSION << endl;
	cout << hour;
}

int main() {
	int x;
	cin >> x;
	ekranayazdir ey(x);
	ey.yazdir();
	cout << endl;
	cin >> x;
	return 0; 
}