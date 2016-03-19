#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <OpenNI.h>
#include <iostream>
#include <Windows.h>
#include <Ole2.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#define width 640
#define height 480			
HANDLE depthStream;
INuiSensor* sensor;
#define width 640
#define height 480
// OpenGL Variables
GLuint textureId;              // ID of the texture to contain Kinect RGB Data
GLubyte data[width*height * 4];
HANDLE rgbStream;
using namespace std;
using namespace cv;
using namespace openni;
bool initKinect() {
	// Get a working kinect sensor
	int numSensors;
	if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return false;
	if (NuiCreateSensorByIndex(0, &sensor) < 0) return false;

	// Initialize sensor
	sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);

	// --------------- START CHANGED CODE -----------------
	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,                     // Depth camera or rgb camera?
		NUI_IMAGE_RESOLUTION_640x480,             // Image resolution
		NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE,   // Image stream flags, e.g. near mode
		2,      // Number of frames to buffer
		NULL,   // Event handle
		&depthStream);
	// --------------- END CHANGED CODE -----------------
	return sensor;
}

void getKinectData() {
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(rgbStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	const USHORT* curr = (const USHORT*)LockedRect.pBits;
	const USHORT* dataEnd = curr + (width*height);
	if (LockedRect.Pitch != 0)
	{
		const BYTE* curr = (const BYTE*)LockedRect.pBits;
		const BYTE* dataEnd = curr + (width*height) * 4;
		while (curr < dataEnd) {
			// Get depth in millimeters
			USHORT depth = NuiDepthPixelToDepth(*curr++);
			depth = depth;
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}


int mainaaaa()
{
	initKinect();
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
	puts("Depth sensor ok");
	VideoMode paramvideo;
	paramvideo.setResolution(640, 480);
	paramvideo.setFps(30);
	paramvideo.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
	depth.setVideoMode(paramvideo);
	paramvideo.setPixelFormat(PIXEL_FORMAT_RGB888);
	color.setVideoMode(paramvideo);
	puts("Réglages des flux vidéos ok");

	// If the depth/color synchronisation is not necessary, start is faster :
	device.setDepthColorSyncEnabled(false);

	// Otherwise, the streams can be synchronized with a reception in the order of our choice :
	//device.setDepthColorSyncEnabled( true );
	//device.setImageRegistrationMode( openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR );

	VideoStream** stream = new VideoStream*[2];
	stream[0] = &depth;
	stream[1] = &color;
	puts("Kinect initialization completed");


	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		VideoFrameRef depthFrame, colorFrame;
		IplImage* colorcv = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_8U, 3);
		IplImage* depthcv = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_16U, 1);
		cvNamedWindow("RGB", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("Depth", CV_WINDOW_AUTOSIZE);

		int changedIndex;
		while (device.isValid())
		{
			OpenNI::waitForAnyStream(stream, 2, &changedIndex);
			switch (changedIndex)
			{
			case 0:
				depth.readFrame(&depthFrame);

				if (depthFrame.isValid())
				{
					depthcv->imageData = (char*)depthFrame.getData();
					cvShowImage("Depth", depthcv);
				}
				break;

			case 1:
				color.readFrame(&colorFrame);

				if (colorFrame.isValid())
				{
					colorcv->imageData = (char*)colorFrame.getData();
					cvCvtColor(colorcv, colorcv, CV_BGR2RGB);
					cvShowImage("RGB", colorcv);
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

