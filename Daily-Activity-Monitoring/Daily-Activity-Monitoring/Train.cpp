/*
This class uses for training and creating SVM features
*/

#include "Train.h"

using namespace cv;

Train::Train(string getPath)
{
	filepath = getPath;
}

void Train::traineSVM(string output)
{
	//positive and negative mats
	Mat pMat;
	Mat nMat;
	int pRow, pCol;
	//Read Hog feature from XML file
	///////////////////////////////////////////////////////////////////////////
	//create xml to read
	FileStorage read_PositiveXml(filepath + "\\Positive\\Positive.xml", FileStorage::READ);
	read_PositiveXml["Descriptor_of_images"] >> pMat;
	//Read Row, Cols
	pRow = pMat.rows; pCol = pMat.cols;
	read_PositiveXml.release();

	FileStorage read_NegativeXml(filepath + "\\Negative\\Negative.xml", FileStorage::READ);
	//reading negative hog features
	read_NegativeXml["Descriptor_of_images"] >> nMat;
	int nRow, nCol;
	nRow = nMat.rows; nCol = nMat.cols;
	read_NegativeXml.release();
	/////////////////////////////////////////////////////////////////////////////////

	//Make training data for SVM
	/////////////////////////////////////////////////////////////////////////////////
	//descriptor data set
	Mat PN_Descriptor_mtx(pRow + nRow, pCol, CV_32FC1); //in here pCol and nCol is descriptor number, so two value must be same;
	memcpy(PN_Descriptor_mtx.data, pMat.data, sizeof(float) * pMat.cols * pMat.rows);
	int startP = sizeof(float) * pMat.cols * pMat.rows;
	memcpy(&(PN_Descriptor_mtx.data[startP]), nMat.data, sizeof(float) * nMat.cols * nMat.rows);
	//data labeling
	Mat labels(pRow + nRow, 1, CV_32S, Scalar(-1.0));
	labels.rowRange(0, pRow) = Scalar(1.0);
	/////////////////////////////////////////////////////////////////////////////////

	//Set svm parameter
	/////////////////////////////////////////////////////////////////////////////////
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
	svm->save("C:\\images\\"  + output + ".xml");
	svm->clear();
	PN_Descriptor_mtx.release();
	cout << endl << "SVM is created at "+ filepath + "\\" + output + ".xml" << endl;
}


