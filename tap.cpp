#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<vector>
#include<Windows.h>
using namespace std;
using namespace cv;

char* getScreen(unsigned long& jpg_size) {
	HWND DeskWnd = ::GetDesktopWindow();//��ȡ���洰�ھ��
	RECT DeskRC;
	::GetClientRect(DeskWnd, &DeskRC);//��ȡ���ڴ�С
	HDC DeskDC = GetDC(DeskWnd);//��ȡ����DC
	HBITMAP DeskBmp = ::CreateCompatibleBitmap(DeskDC, DeskRC.right, DeskRC.bottom);//����λͼ
	HDC memDC = ::CreateCompatibleDC(DeskDC);//����DC
	SelectObject(memDC, DeskBmp);//�Ѽ���λͼѡ�����DC��
	BitBlt(memDC, 0, 0, DeskRC.right, DeskRC.bottom, DeskDC, 0, 0, SRCCOPY);
	BITMAP bmInfo;
	DWORD bmDataSize;
	char* bmData;//λͼ����
	GetObject(DeskBmp, sizeof(BITMAP), &bmInfo);//����λͼ�������ȡλͼ��Ϣ
	bmDataSize = bmInfo.bmWidthBytes * bmInfo.bmHeight;//����λͼ���ݴ�С
	bmData = new char[bmDataSize];//��������
	BITMAPFILEHEADER bfh;//λͼ�ļ�ͷ
	bfh.bfType = 0x4d42;
	bfh.bfSize = bmDataSize + 54;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = 54;
	BITMAPINFOHEADER bih;//λͼ��Ϣͷ
	bih.biSize = 40;
	bih.biWidth = bmInfo.bmWidth;
	bih.biHeight = bmInfo.bmHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = bmDataSize;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	::GetDIBits(DeskDC, DeskBmp, 0, bmInfo.bmHeight, bmData, (BITMAPINFO*)&bih, DIB_RGB_COLORS);//��ȡλͼ���ݲ���
	ReleaseDC(DeskWnd, DeskDC);
	DeleteDC(memDC);
	DeleteObject(DeskBmp);
	int sumSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmDataSize;
	char* stream = new char[sumSize];
	memcpy(stream, &bfh, sizeof(BITMAPFILEHEADER));
	memcpy(stream + sizeof(BITMAPFILEHEADER), &bih, sizeof(BITMAPINFOHEADER));
	memcpy(stream + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), bmData, bmDataSize);
	delete bmData;
	bmData = NULL;
	/*HANDLE hFile=CreateFile(_T("d:\\Screen.bmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
	FILE_ATTRIBUTE_NORMAL,0);//�����ļ�
	DWORD dwSize;
	WriteFile(hFile,(void *)stream,sumSize,&dwSize,0);//д��λͼ�ļ�ͷ
	::CloseHandle(hFile);//�ر��ļ����*/;
	//char* p1 = rgb_to_jpeg(reinterpret_cast<unsigned char*>(stream), jpg_size, bih.biWidth, bih.biHeight);//NEEDS EXHELP
	char* p1 = NULL;
	delete stream;
	stream = NULL;
	return p1;
}

Mat grabScreen() {
	HWND deskWnd = GetDesktopWindow();//��ȡ���洰�ھ��
	RECT deskRC;
	GetClientRect(deskWnd, &deskRC);//��ȡ���ڴ�С
	//cout << deskRC.right << deskRC.bottom;
	HDC deskDC = GetDC(deskWnd);//��ȡ����DC
	HBITMAP deskBmp = CreateCompatibleBitmap(deskDC, deskRC.right, deskRC.bottom);//��������λͼ
	HDC memDC = CreateCompatibleDC(deskDC);//��������DC
	SelectObject(memDC, deskBmp);//������λͼѡ�����DC��
	BitBlt(memDC, 0, 0, deskRC.right, deskRC.bottom, deskDC, 0, 0, SRCCOPY);//λͼ����
	LONG signedIntsArray = GetBitmapBits(deskBmp, 0, NULL);//32λ��СΪ1080*1920*32/8
	//cout << signedIntsArray;
	int imgSize = 1080 * 1920 * 32 / 8;
	BYTE *buf = new BYTE[imgSize];
	GetBitmapBits(deskBmp, signedIntsArray, buf);//��ȡλͼ�ֽ�
	Mat outImg(1080, 1920, CV_8UC4);
	memcpy(outImg.data, buf, imgSize);
	return outImg;
}

int main(void) {
	double threshold = 0.1;
	Sleep(3000);
	ULONGLONG tic, toc;
	tic = GetTickCount64();
	Mat outImg = grabScreen();
	toc = GetTickCount64();
	cout << 1000 / (toc - tic);
	//namedWindow("3", CV_WINDOW_NORMAL);
	//imshow("3", outImg);
	//waitKey();
	Mat templ = imread("./targetCai.bmp");
	vector<Mat> channels;
	split(templ, channels);
	Mat templBlueImg = channels.at(2);
	split(outImg, channels);
	Mat outImgBlueImg = channels.at(2);
	Mat result;
	matchTemplate(outImgBlueImg, templBlueImg, result, TM_CCOEFF_NORMED);
	//normalize(result, result, 0, 1, NORM_MINMAX);
	double minValue, maxValue;
	Point minLocation, maxLocation;
	minMaxLoc(result, &minValue, &maxValue, &minLocation, &maxLocation);
	if (maxValue > 1 - threshold)
		rectangle(outImg, maxLocation, Point(maxLocation.x + templ.cols, maxLocation.y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);
	namedWindow("MT", CV_WINDOW_NORMAL);
	imshow("MT", outImg);
	waitKey();
	return 0;
}