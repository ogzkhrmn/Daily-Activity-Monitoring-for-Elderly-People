#include "ActionRecognition.h"
#include "ExtractSilhouette.h"
#include <OpenNI.h>
/*
This calss is using for finding actions.
With this function we will detect actions.
*/

using namespace cv;
using namespace std;
using namespace ml;
using namespace openni;

ActionRecognition::ActionRecognition()
{
}

int ActionRecognition::startActionRecognition() {

	ExtractSilhouette es;
	Mat dest;
	//Load trained SVM xml data
	cout << "Opening SVM files" << endl;
	Ptr<ml::SVM> svmyur = ml::SVM::create();
	svmyur = SVM::load<SVM>("C:\\images\\yurumeSVM.xml");
	Ptr<ml::SVM> svmel = ml::SVM::create();
	svmel = SVM::load<SVM>("C:\\images\\elSVM.xml");
	Ptr<ml::SVM> svmkol = ml::SVM::create();
	svmkol = SVM::load<SVM>("C:\\images\\kolSVM.xml");
	Ptr<ml::SVM> svmzip = ml::SVM::create();
	svmzip = SVM::load<SVM>("C:\\images\\ziplamaSVM.xml");


	//Kinext initilization starting

	OpenNI::initialize();
	puts("Kinect initialization...");
	Device device;
	if (device.open(openni::ANY_DEVICE) != 0)
	{
		puts("Kinect not found !");
		return -1;
	}
	puts("Kinect opened");
	VideoStream depth, color;
	color.create(device, SENSOR_COLOR);
	color.start();
	puts("Camera ok");
	depth.create(device, SENSOR_DEPTH);
	depth.start();
	int frameCounter = 1;
	puts("Depth sensor ok");
	VideoMode paramvideo;
	paramvideo.setResolution(640, 480);
	paramvideo.setFps(30);
	paramvideo.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
	depth.setVideoMode(paramvideo);
	paramvideo.setPixelFormat(PIXEL_FORMAT_RGB888);
	color.setVideoMode(paramvideo);
	puts("Reglages des flux videos ok");
	// If the depth/color synchronisation is not necessary, start is faster :
	device.setDepthColorSyncEnabled(false);
	VideoStream** stream = new VideoStream*[2];
	stream[0] = &depth;
	stream[1] = &color;
	puts("Kinect initialization completed");
	//kinect initilization finishhhing

	//result 
	int result;
	//HogFeature variables
	HOGDescriptor d(Size(640, 480), Size(8, 8), Size(4, 4), Size(4, 4), 9);
	vector< float> descriptorsValues;
	vector< Point> locations;


	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		stringstream file;
		VideoFrameRef depthFrame, colorFrame;
		IplImage* colorcv = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_8U, 3);
		IplImage* depthcv = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_16U, 1);
		int changedIndex;
		Mat extractedSilhoutte;
		int counter = 0;
		int whichFrame = 0;
		while (device.isValid())
		{
			OpenNI::waitForAnyStream(stream, 2, &changedIndex);
			switch (changedIndex)
			{
			case 0:
				depth.readFrame(&depthFrame);
				break;
			case 1:
				//getting rgb frames
				color.readFrame(&colorFrame);
				if (colorFrame.isValid())
				{
					colorcv->imageData = (char*)colorFrame.getData();
					cvCvtColor(colorcv, colorcv, CV_BGR2RGB);
					Mat colorcvframe = cvarrToMat(colorcv);
					imshow("RGB", colorcvframe);
					//getting silhouette
					extractedSilhoutte = es.getSilhouette(colorcvframe);
					//system starting after 6th frame.
					if (frameCounter == 6) {
						extractedSilhoutte.copyTo(dest);
					}
					if (frameCounter > 6) {
						//adding frames
						bitwise_or(dest, extractedSilhoutte, dest);
						whichFrame++;
						if (whichFrame == 20) {
							//if we add 20 frames we trying to detection an action
							whichFrame = 0;
							counter++;
							d.compute(dest, descriptorsValues, Size(0, 0), Size(0, 0), locations);

							//checking all svmdetectors
							result = svmyur->predict(descriptorsValues);
							if (result == -1) {
								result = svmel->predict(descriptorsValues);
								if (result == -1) {
									result = svmkol->predict(descriptorsValues);
									if (result == -1) {
										result = svmzip->predict(descriptorsValues);
										if (result == -1) {
											printf("Tanimadim\n");
										}
										else {
											printf("Ziplama\n");
										}
									}
									else {
										printf("kol\n");
									}
								}
								else {
									printf("El\n");
								}
							}
							else {
								printf("Yurume\n");
							}
							extractedSilhoutte.copyTo(dest);
						}

					}
					frameCounter++;
					imshow("DPETH", extractedSilhoutte);

				}
				break;
			default:
				puts("Error retrieving a stream");
			}
			cvWaitKey(1);
		}
	}

}
