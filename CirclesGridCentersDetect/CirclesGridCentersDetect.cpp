// circlesgridcenter.cpp : �w�q�D���x���ε{�����i�J�I�C
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <io.h>
#include <direct.h>
#include <string>  
#include <vector>

using namespace cv;
using namespace std;

void getFiles(string path, string format, vector<string>& filenames, vector<string>& files);

int main()
{
	/*Ū���C�@�T�Ϲ��A�q�������X���I�A�M��﨤�I�i��ȹϤ���T��*/
	string filePath;							// ��J��󧨥ؿ�
	string format = "JPG";						// ���w�榡
	vector<string> files;						// ����m�ΦW��
	vector<string> filenames;					// ���W��
	bool circlesGridType = 0;					// �����ƦC�覡
	int szr;									// �Щw���C���I��
	int szc;									// �Щw���樤�I��
	cout << "Enter the folder directory : ";	// ��J�v����󧨦�m
	cin >> filePath;
	//cout << "Enter the image format (ex:.jpg) : ";	// ��J�v���榡
	//cin >> format;
	cout << "Please enter the circles grid type ((0) symmetric grid  (1) asymmetric grid) : ";
	cin >> circlesGridType;
	cout << "Please enter the size of board : " << endl;
	cout << "row : ";
	cin >> szr;
	cout << "column : ";
	cin >> szc;

	int image_count = 0;						// �Ϲ��ƶq
	Size image_size;							// �Ϲ����ؤo
	Size board_size = Size(szr, szc);			// �Щw�O�W�C��B�C�����I��
	vector<Point2f> image_points_buf;			// �w�s�C�T�Ϲ��W�˴��쪺���I

	getFiles(filePath, format, filenames, files);
	cout << endl << "There are " << files.size() << " images." << endl << endl;

	string subfilePath = filePath + "\\circlesgridcenter";			//�Ыؤl��Ƨ�
	_mkdir(subfilePath.c_str());

	filePath += "\\";
	ofstream fout(subfilePath + "\\circlesgridcenter.txt");			// �ЫثO�s�Щw���G����r��

	for (int i = 0; i < files.size(); i++)
	{
		Mat imageInput = imread(files[i]);
		Mat view_gray_temp, view_gray;
		cvtColor(imageInput, view_gray_temp, CV_RGB2GRAY);
		equalizeHist(view_gray_temp, view_gray);
		image_count++;

		if (image_count == 1)								//Ū�J�Ĥ@�i�Ϥ�������Ϲ��e����T  
		{
			cout << "Start to get image corners..." << endl << endl;
		}
		cout << "No." << image_count << "\t" << filenames[i] << " start..." << endl;		// �Ω��[�������X  

																							/* �������I */
		SimpleBlobDetector::Params params;
		params.thresholdStep = 10;
		params.minThreshold = 50;
		params.maxThreshold = 255;
		params.minRepeatability = 1;
		params.maxArea = 400000; // 100 * 100
		params.minArea = 50000; // 100 * 100
		Ptr<FeatureDetector> blobDetector = new SimpleBlobDetector(params);
		bool getcorners;

		if (circlesGridType)
			getcorners = findCirclesGrid(view_gray, board_size, image_points_buf, CALIB_CB_ASYMMETRIC_GRID | CALIB_CB_CLUSTERING, blobDetector);
		else
			getcorners = findCirclesGrid(view_gray, board_size, image_points_buf, CALIB_CB_SYMMETRIC_GRID | CALIB_CB_CLUSTERING, blobDetector);

		if (getcorners == 0)
		{
			cout << "corner detect failed!" << endl;		//�䤣�쨤�I
			fout << filenames[i] << " :\t" << endl;
		}
		else
		{
			cout << "corner detect succeed!" << endl;
			/* �b�Ϲ��W��ܨ��I��m */
			drawChessboardCorners(imageInput, board_size, image_points_buf, true);	//�Ω�b�Ϥ����аO���I
			string findpoint = subfilePath + "\\";
			findpoint.append(filenames[i]);
			imwrite(findpoint, imageInput);											//�s���Хܫ᪺�Ϥ�
			fout << filenames[i] << " :\t";
			for (int i = 0; i < image_points_buf.size(); i++)
			{
				fout << image_points_buf[i].x << "\t";
				fout << image_points_buf[i].y << "\t";
			}
			fout << endl;
		}
	}
	std::cout << endl << "Results are saved!" << endl;
	return 0;
}

/*����S�w�榡���ɮצW*/
// �Ĥ@�Ӭ����|�r��(string����) 
// �ĤG�ӰѼƬ����w�榡(string����) 
// �ĤT�ӰѼƬ��ɮצW�٦s�x�ܼ�(vector����,�ޥζǻ�)
// �ĥ|�ӰѼƬ��ɮצ�m�s�x�ܼ�(vector����,�ޥζǻ�)

void getFiles(string path, string format, vector<string>& filenames, vector<string>& files)
{
	//�ɮױ���N�X    
	intptr_t   hFile = 0;
	//�ɸ�T    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), format, filenames, files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				filenames.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}


