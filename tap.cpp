#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui_c.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<vector>
#include<Windows.h>
using namespace std;
using namespace cv;

char* getScreen(unsigned long& jpg_size) {
	HWND DeskWnd = ::GetDesktopWindow();//获取桌面窗口句柄
	RECT DeskRC;
	::GetClientRect(DeskWnd, &DeskRC);//获取窗口大小
	HDC DeskDC = GetDC(DeskWnd);//获取窗口DC
	HBITMAP DeskBmp = ::CreateCompatibleBitmap(DeskDC, DeskRC.right, DeskRC.bottom);//兼容位图
	HDC memDC = ::CreateCompatibleDC(DeskDC);//兼容DC
	SelectObject(memDC, DeskBmp);//把兼容位图选入兼容DC中
	BitBlt(memDC, 0, 0, DeskRC.right, DeskRC.bottom, DeskDC, 0, 0, SRCCOPY);
	BITMAP bmInfo;
	DWORD bmDataSize;
	char* bmData;//位图数据
	GetObject(DeskBmp, sizeof(BITMAP), &bmInfo);//根据位图句柄，获取位图信息
	bmDataSize = bmInfo.bmWidthBytes * bmInfo.bmHeight;//计算位图数据大小
	bmData = new char[bmDataSize];//分配数据
	BITMAPFILEHEADER bfh;//位图文件头
	bfh.bfType = 0x4d42;
	bfh.bfSize = bmDataSize + 54;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = 54;
	BITMAPINFOHEADER bih;//位图信息头
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
	::GetDIBits(DeskDC, DeskBmp, 0, bmInfo.bmHeight, bmData, (BITMAPINFO*)&bih, DIB_RGB_COLORS);//获取位图数据部分
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
	FILE_ATTRIBUTE_NORMAL,0);//创建文件
	DWORD dwSize;
	WriteFile(hFile,(void *)stream,sumSize,&dwSize,0);//写入位图文件头
	::CloseHandle(hFile);//关闭文件句柄*/;
	//char* p1 = rgb_to_jpeg(reinterpret_cast<unsigned char*>(stream), jpg_size, bih.biWidth, bih.biHeight);//NEEDS EXHELP
	char* p1 = NULL;
	delete stream;
	stream = NULL;
	return p1;
}

Mat grabScreen() {
	HWND deskWnd = GetDesktopWindow();//获取桌面窗口句柄
	RECT deskRC;
	GetClientRect(deskWnd, &deskRC);//获取窗口大小
	//cout << deskRC.right << deskRC.bottom;
	HDC deskDC = GetDC(deskWnd);//获取窗口DC
	HBITMAP deskBmp = CreateCompatibleBitmap(deskDC, deskRC.right, deskRC.bottom);//创建兼容位图
	HDC memDC = CreateCompatibleDC(deskDC);//创建兼容DC
	SelectObject(memDC, deskBmp);//将兼容位图选入兼容DC中
	BitBlt(memDC, 0, 0, deskRC.right, deskRC.bottom, deskDC, 0, 0, SRCCOPY);//位图复制
	LONG signedIntsArray = GetBitmapBits(deskBmp, 0, NULL);//32位大小为1080*1920*32/8
	//cout << signedIntsArray;
	int imgSize = 1080 * 1920 * 32 / 8;
	BYTE *buf = new BYTE[imgSize];
	GetBitmapBits(deskBmp, signedIntsArray, buf);//获取位图字节
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