/*
this class uses for extraction and saving hog features
*/

#include "HogExtraction.h"
using namespace cv;
vector< vector < float> > v_descriptorsValues;
vector< vector < cv::Point> > v_locations;
HogExtraction::HogExtraction(string getPath,string getType)
{
	filepath = getPath;
	type = getType;
}

void HogExtraction::computeHogandSave(Mat im1)
{
	//hog feature extraction
	vector< float> descriptorsValues;
	vector< Point> locations;
	HOGDescriptor hog(Size(320, 240), Size(8, 8), Size(4, 4), Size(4, 4), 9);
	hog.compute(im1, descriptorsValues, Size(0, 0), Size(0, 0), locations);
	//adding hog features to our feature vectors
	v_descriptorsValues.push_back(descriptorsValues);
	v_locations.push_back(locations);
}

void HogExtraction::saveXml()
{
	//saving extracted hog features
	FileStorage hogXml(filepath + "\\" + type + "/"+type+".xml", FileStorage::WRITE);
	int row = v_descriptorsValues.size(), col = v_descriptorsValues[0].size();
	Mat M(row, col, CV_32F);
	for (int i = 0; i < row; ++i)
		memcpy(&(M.data[col * i * sizeof(float)]), v_descriptorsValues[i].data(), col*sizeof(float));
	//saving hog features
	write(hogXml, "Descriptor_of_images", M);
	hogXml.release();
	v_descriptorsValues.clear();
	v_locations.clear();
	cout << endl << "Hog is created at location: " + filepath + "\\" + type + "\\"  + type + ".xml" << endl;
}

void HogExtraction::extractHog()
{
	Mat myimage;
	String s;
	WIN32_FIND_DATA FindFileData;
	string img = filepath + "\\" + type + "/*.bmp";
	HANDLE hFind = FindFirstFile(img.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
	}
	//getting files in our directory
	else do {
		s = FindFileData.cFileName;
		s = filepath + "\\" + type + "/"+ s + "";
		cout << FindFileData.cFileName << endl;
		myimage = imread(s, CV_LOAD_IMAGE_GRAYSCALE);
		//adding hog features orur vector
		computeHogandSave(myimage);
	} while (FindNextFile(hFind, &FindFileData));
	cout << endl;
	saveXml();
	FindClose(hFind);
}


