#include < stdio.h>
#include < opencv2\opencv.hpp>
#include <opencv2\ml\ml.hpp>
//#include < opencv2\gpu\gpu.hpp>

using namespace cv;
using namespace std;

int svMmain() {
	//action mats
	Mat yurumeMat;
	Mat ziplamaMat;
	Mat elMat;
	Mat kolMat;
	int yurumeRow, yurumeCol;
	int ziplamaRow, ziplamaCol;
	int elRow, elCol;
	int kolRow, kolCol;
	//Read Hog feature from XML file
	///////////////////////////////////////////////////////////////////////////
	//create xml to read

	//reading first action
	FileStorage read_yurume("C:\\images/Yurume.xml", FileStorage::READ);
	read_yurume["Descriptor_of_images"] >> yurumeMat;
	//Read Row, Cols
	yurumeRow = yurumeMat.rows; yurumeCol = yurumeMat.cols;
	read_yurume.release();

	//reading second action
	FileStorage read_ziplama("C:\\images/Ziplama.xml", FileStorage::READ);
	read_ziplama["Descriptor_of_images"] >> ziplamaMat;
	//Read Row, Cols
	ziplamaRow = ziplamaMat.rows; ziplamaCol = ziplamaMat.cols;
	read_ziplama.release();

	//reading third action
	FileStorage read_el("C:\\images/el sallama.xml", FileStorage::READ);
	read_el["Descriptor_of_images"] >> elMat;
	//Read Row, Cols
	elRow = elMat.rows; elCol = elMat.cols;
	read_el.release();

	//reading fourth action
	FileStorage read_kol("C:\\images/kol.xml", FileStorage::READ);
	read_kol["Descriptor_of_images"] >> kolMat;
	//Read Row, Cols
	kolRow = kolMat.rows; kolCol = kolMat.cols;
	read_kol.release();




	//Make training data for SVM
	/////////////////////////////////////////////////////////////////////////////////
	printf("2. Make training data for SVM\n");
	//descriptor data set
	Mat PN_Descriptor_mtx(elRow + kolRow+ziplamaRow+yurumeRow, elCol, CV_32FC1); //in here pCol and nCol is descriptor number, so two value must be same;

	memcpy(PN_Descriptor_mtx.data, yurumeMat.data, sizeof(float) * yurumeMat.cols * yurumeMat.rows);

	int startP = sizeof(float) * yurumeMat.cols * yurumeMat.rows;
	memcpy(&(PN_Descriptor_mtx.data[startP]), ziplamaMat.data, sizeof(float) * ziplamaMat.cols * ziplamaMat.rows);

	startP = startP+ sizeof(float) *ziplamaMat.cols * ziplamaMat.rows;
	memcpy(&(PN_Descriptor_mtx.data[startP]), elMat.data, sizeof(float) * elMat.cols * elMat.rows);

	startP = sizeof(float) * elMat.cols * elMat.rows;
	memcpy(&(PN_Descriptor_mtx.data[startP]), kolMat.data, sizeof(float) * kolMat.cols * kolMat.rows);
	//data labeling
	Mat labels(elRow + kolRow + ziplamaRow + yurumeRow, 1, CV_32S, Scalar(-1.0));
	labels.rowRange(0, yurumeRow) = Scalar(1.0);
	labels.rowRange(yurumeRow, ziplamaRow) = Scalar(2.0);
	labels.rowRange(ziplamaRow , elRow) = Scalar(3.0);
	labels.rowRange(elRow , kolRow) = Scalar(4.0);
	/////////////////////////////////////////////////////////////////////////////////

	//Set svm parameter
	/////////////////////////////////////////////////////////////////////////////////
	printf("4. SVM training\n");
	Ptr<ml::SVM> svm = ml::SVM::create();
	svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(
		TermCriteria::Type::MAX_ITER,
		10000,
		1e-6
		));
	/////////////////////////////////////////////////////////////////////////////////

	//Training
	/////////////////////////////////////////////////////////////////////////////////
	svm->train(PN_Descriptor_mtx, ml::ROW_SAMPLE, labels);
	//Trained data save
	/////////////////////////////////////////////////////////////////////////////////

	svm->save("C:\\images/trainedSVM.xml");

	return 0;
}