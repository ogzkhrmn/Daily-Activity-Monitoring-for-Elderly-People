#include "ImagesGetting.h"
#include <OpenNI.h>

using namespace cv;
using namespace std;
using namespace ml;
using namespace openni;
ImagesGetting::ImagesGetting()
{
}


int ImagesGetting::getImages()
{
	Mat dest;
	ExtractSilhouette es;
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
	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
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
				color.readFrame(&colorFrame);
				if (colorFrame.isValid())
				{
					//getting rgb images
					colorcv->imageData = (char*)colorFrame.getData();
					cvCvtColor(colorcv, colorcv, CV_BGR2RGB);
					Mat colorcvframe = cvarrToMat(colorcv);
					//getting silhouette
					extractedSilhoutte = es.getSilhouette(colorcvframe);

					////system starting after 6th frame.
					if (frameCounter == 6) {
						extractedSilhoutte.copyTo(dest);
					}
					if (frameCounter > 6) {
						//adding frames
						bitwise_or(dest, extractedSilhoutte, dest);
						imshow("ORIMAGE", dest);
						whichFrame++;

						if (whichFrame == 20) {
							//if we add 20  frames saving image
							whichFrame = 0;
							imwrite("C:/images/image" + to_string(counter) + ".bmp", dest);
							counter++;
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
		cvReleaseImageHeader(&colorcv);
		cvReleaseImageHeader(&depthcv);
		cvDestroyWindow("RGB");
		cvDestroyWindow("Depth");
	}
	depth.stop();
	depth.destroy();
	color.stop();
	color.destroy();
	device.close();
	OpenNI::shutdown();
	return 0;
}
