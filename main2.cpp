
/********************************************

2017.6.19测试zzuyuting

使用三帧差算法

*******************************************/
#include<iostream>  
using namespace std;
#include<opencv2\core\core.hpp>  
#include<opencv2\highgui\highgui.hpp>  
#include<opencv2\imgproc\imgproc.hpp>  
#include <windows.h> 
using namespace cv;
const int A1 = 131;
const int A2 = 147;
const int A3 = 165;
const int A4 = 175;
const int A5 = 196;
const int A6 = 220;
const int A7 = 247;

//定义中音  
const int B1 = 262;
const int B2 = 296;
const int B3 = 330;
const int B4 = 349;
const int B5 = 392;
const int B6 = 440;
const int B7 = 494;

//定义高音  
const int C1 = 523;
const int C2 = 587;
const int C3 = 659;
const int C4 = 698;
const int C5 = 784;
const int C6 = 880;
const int C7 = 988;

//定义高二度  
const int D1 = 1047;
const int D2 = 1175;
const int D3 = 1319;
const int D4 = 1397;
const int D5 = 1568;
const int D6 = 1760;
const int D7 = 1976;

//定义节拍  
const int OneBeat = 600;
const int HalfBeat = 300;
void SendOff();

const unsigned char FORE_GROUD = 255;
int thresh = 8;

int main(int argc, char*argv[])
{

	VideoCapture video("D:/1fall.avi");

	//判断如果video是否可以打开  
	if (!video.isOpened())
		return -1;

	//用于保存当前帧的图片  
	Mat currentBGRFrame;

	//用来保存上一帧和当前帧的灰度图片  
	Mat previousSecondGrayFrame;
	Mat previousFirstGrayFrame;
	Mat currentGaryFrame;

	//保存两次的帧差  
	Mat previousFrameDifference;//previousFrameFirst - previousFrameSecond的差分  
	Mat currentFrameDifference;//currentFrame - previousFrameFirst;  

	//用来保存帧差的绝对值  
	Mat absFrameDifferece;

	//用来显示前景  
	Mat previousSegmentation;
	Mat currentSegmentation;
	Mat segmentation;


	//显示前景  
	namedWindow("segmentation", 1);
	createTrackbar("阈值:", "segmentation", &thresh, FORE_GROUD, NULL);

	//帧数  
	int numberFrame = 0;
	//摔倒标志
	int flag = 0;
	int voice = 0;
	//形态学处理用到的算子  
	Mat morphologyKernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

	for (;;)
	{
		//读取当前帧  
		video >> currentBGRFrame;

		//判断当前帧是否存在  
		if (!currentBGRFrame.data)
			break;

		numberFrame++;
		//颜色空间的转换  
		cvtColor(currentBGRFrame, currentGaryFrame, COLOR_BGR2GRAY);

		if (numberFrame == 1)
		{
			//保存当前帧的灰度图  
			previousSecondGrayFrame = currentGaryFrame.clone();

			//显示视频  
			imshow("video", currentBGRFrame);
			continue;
		}
		else if (numberFrame == 2)
		{
			//保存当前帧的灰度图  
			previousFirstGrayFrame = currentGaryFrame.clone();

			//previousFirst - previousSecond  
			subtract(previousFirstGrayFrame, previousSecondGrayFrame, previousFrameDifference, Mat(), CV_16SC1);

			//取绝对值  
			absFrameDifferece = abs(previousFrameDifference);

			//位深的改变  
			absFrameDifferece.convertTo(absFrameDifferece, CV_8UC1, 1, 0);

			//阈值处理  
			threshold(absFrameDifferece, previousSegmentation, double(thresh), double(FORE_GROUD), THRESH_BINARY);

			//显示视频  
			imshow("video", currentBGRFrame);
			continue;
		}

		else
		{
			//src1-src2  
			subtract(currentGaryFrame, previousFirstGrayFrame, currentFrameDifference, Mat(), CV_16SC1);

			//取绝对值  
			absFrameDifferece = abs(currentFrameDifference);

			//位深的改变  
			absFrameDifferece.convertTo(absFrameDifferece, CV_8UC1, 1, 0);

			//阈值处理  
			threshold(absFrameDifferece, currentSegmentation, double(thresh), double(FORE_GROUD), THRESH_BINARY);

			//与运算  
			bitwise_and(previousSegmentation, currentSegmentation, segmentation);

			//中值滤波  
			medianBlur(segmentation, segmentation, 3);

			//形态学处理(开闭运算)  
			//morphologyEx(segmentation,segmentation,MORPH_OPEN,morphologyKernel,Point(-1,-1),1,BORDER_REPLICATE);  
			morphologyEx(segmentation, segmentation, MORPH_CLOSE, morphologyKernel, Point(-1, -1), 2, BORDER_REPLICATE);


			//找边界  
			vector< vector<Point> > contours;
			vector<Vec4i> hierarchy;
			//复制segmentation  
			Mat tempSegmentation = segmentation.clone();
			findContours(segmentation, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//CV_RETR_TREE  
			vector< vector<Point> > contours_poly(contours.size());

			/*存储运动物体*/
			vector<Rect> boundRect;
			boundRect.clear();
			
			//画出运动物体  
			for (int index = 0; index < contours.size(); index++)
			{
				approxPolyDP(Mat(contours[index]), contours_poly[index], 3, true);
				Rect rect = boundingRect(Mat(contours_poly[index]));
				double  a, b,c;
				a = rect.width;
				b = rect.height;
				c = a / b;
				printf("宽=%.2f,高=%.2f,宽高比=%.2f \n", a, b, c);//高宽比
				//rectangle(currentBGRFrame, rect, Scalar(0, 255, 255), 2);
				if ((a * b > 500)&&a<b) // 面积小的方形抛弃掉
				{			
						rectangle(currentBGRFrame, rect, Scalar(0, 255, 0), 2);//过滤掉一些矩阵,显示绿色
				}
				else if ((a > 1.5*b)&&(a*b>200)){
					rectangle(currentBGRFrame, rect, Scalar(0, 0, 255), 2);//过滤掉一些矩阵，显示红色
					printf("摔倒\n");
					flag = 1;
					voice++;
					//while (1);
					//Sleep(0.2 * 1000);//延时5秒 
				}
			}
			if (voice == 1){
				SendOff();
				voice = 2;
			}


			//显示视频  
			imshow("video", currentBGRFrame);

			//前景检测  
			imshow("segmentation", segmentation);

			//保存当前帧的灰度图  
			previousFirstGrayFrame = currentGaryFrame.clone();

			//保存当前的前景检测  
			previousSegmentation = currentSegmentation.clone();
		}
		waitKey(50);
	}
	return 0;
}


void SendOff()
{
	//送别  
	const int ONE_BEEP = 600;
	const int HALF_BEEP = 300;

	const int NOTE_1 = 440;
	const int NOTE_2 = 495;
	const int NOTE_3 = 550;
	const int NOTE_4 = 587;
	const int NOTE_5 = 660;
	const int NOTE_6 = 733;
	const int NOTE_7 = 825;

	//长亭外  
	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP);

}
