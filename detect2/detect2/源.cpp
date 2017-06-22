
/********************************************

2017.6.19����zzuyuting

ʹ����֡���㷨

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

//��������  
const int B1 = 262;
const int B2 = 296;
const int B3 = 330;
const int B4 = 349;
const int B5 = 392;
const int B6 = 440;
const int B7 = 494;

//�������  
const int C1 = 523;
const int C2 = 587;
const int C3 = 659;
const int C4 = 698;
const int C5 = 784;
const int C6 = 880;
const int C7 = 988;

//����߶���  
const int D1 = 1047;
const int D2 = 1175;
const int D3 = 1319;
const int D4 = 1397;
const int D5 = 1568;
const int D6 = 1760;
const int D7 = 1976;

//�������  
const int OneBeat = 600;
const int HalfBeat = 300;
void SendOff();

const unsigned char FORE_GROUD = 255;
int thresh = 8;

int main(int argc, char*argv[])
{

	VideoCapture video("D:/1fall.avi");

	//�ж����video�Ƿ���Դ�  
	if (!video.isOpened())
		return -1;

	//���ڱ��浱ǰ֡��ͼƬ  
	Mat currentBGRFrame;

	//����������һ֡�͵�ǰ֡�ĻҶ�ͼƬ  
	Mat previousSecondGrayFrame;
	Mat previousFirstGrayFrame;
	Mat currentGaryFrame;

	//�������ε�֡��  
	Mat previousFrameDifference;//previousFrameFirst - previousFrameSecond�Ĳ��  
	Mat currentFrameDifference;//currentFrame - previousFrameFirst;  

	//��������֡��ľ���ֵ  
	Mat absFrameDifferece;

	//������ʾǰ��  
	Mat previousSegmentation;
	Mat currentSegmentation;
	Mat segmentation;


	//��ʾǰ��  
	namedWindow("segmentation", 1);
	createTrackbar("��ֵ:", "segmentation", &thresh, FORE_GROUD, NULL);

	//֡��  
	int numberFrame = 0;
	//ˤ����־
	int flag = 0;
	int voice = 0;
	//��̬ѧ�����õ�������  
	Mat morphologyKernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

	for (;;)
	{
		//��ȡ��ǰ֡  
		video >> currentBGRFrame;

		//�жϵ�ǰ֡�Ƿ����  
		if (!currentBGRFrame.data)
			break;

		numberFrame++;
		//��ɫ�ռ��ת��  
		cvtColor(currentBGRFrame, currentGaryFrame, COLOR_BGR2GRAY);

		if (numberFrame == 1)
		{
			//���浱ǰ֡�ĻҶ�ͼ  
			previousSecondGrayFrame = currentGaryFrame.clone();

			//��ʾ��Ƶ  
			imshow("video", currentBGRFrame);
			continue;
		}
		else if (numberFrame == 2)
		{
			//���浱ǰ֡�ĻҶ�ͼ  
			previousFirstGrayFrame = currentGaryFrame.clone();

			//previousFirst - previousSecond  
			subtract(previousFirstGrayFrame, previousSecondGrayFrame, previousFrameDifference, Mat(), CV_16SC1);

			//ȡ����ֵ  
			absFrameDifferece = abs(previousFrameDifference);

			//λ��ĸı�  
			absFrameDifferece.convertTo(absFrameDifferece, CV_8UC1, 1, 0);

			//��ֵ����  
			threshold(absFrameDifferece, previousSegmentation, double(thresh), double(FORE_GROUD), THRESH_BINARY);

			//��ʾ��Ƶ  
			imshow("video", currentBGRFrame);
			continue;
		}

		else
		{
			//src1-src2  
			subtract(currentGaryFrame, previousFirstGrayFrame, currentFrameDifference, Mat(), CV_16SC1);

			//ȡ����ֵ  
			absFrameDifferece = abs(currentFrameDifference);

			//λ��ĸı�  
			absFrameDifferece.convertTo(absFrameDifferece, CV_8UC1, 1, 0);

			//��ֵ����  
			threshold(absFrameDifferece, currentSegmentation, double(thresh), double(FORE_GROUD), THRESH_BINARY);

			//������  
			bitwise_and(previousSegmentation, currentSegmentation, segmentation);

			//��ֵ�˲�  
			medianBlur(segmentation, segmentation, 3);

			//��̬ѧ����(��������)  
			//morphologyEx(segmentation,segmentation,MORPH_OPEN,morphologyKernel,Point(-1,-1),1,BORDER_REPLICATE);  
			morphologyEx(segmentation, segmentation, MORPH_CLOSE, morphologyKernel, Point(-1, -1), 2, BORDER_REPLICATE);


			//�ұ߽�  
			vector< vector<Point> > contours;
			vector<Vec4i> hierarchy;
			//����segmentation  
			Mat tempSegmentation = segmentation.clone();
			findContours(segmentation, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//CV_RETR_TREE  
			vector< vector<Point> > contours_poly(contours.size());

			/*�洢�˶�����*/
			vector<Rect> boundRect;
			boundRect.clear();
			
			//�����˶�����  
			for (int index = 0; index < contours.size(); index++)
			{
				approxPolyDP(Mat(contours[index]), contours_poly[index], 3, true);
				Rect rect = boundingRect(Mat(contours_poly[index]));
				double  a, b,c;
				a = rect.width;
				b = rect.height;
				c = a / b;
				//printf("��=%.2f,��=%.2f,��߱�=%.2f \n", a, b, c);//�߿��
				//rectangle(currentBGRFrame, rect, Scalar(0, 255, 255), 2);
				if ((a * b > 500)&&a<b) // ���С�ķ���������
				{			
						rectangle(currentBGRFrame, rect, Scalar(0, 255, 0), 2);//���˵�һЩ����,��ʾ��ɫ
				}
				else if ((a > 1.5*b)&&(a*b>200)){
					rectangle(currentBGRFrame, rect, Scalar(0, 0, 255), 2);//���˵�һЩ������ʾ��ɫ
					printf("ˤ��\n");
					flag = 1;
					voice++;
					//while (1);
					//Sleep(0.2 * 1000);//��ʱ5�� 
				}
			}
			if (voice == 1){
				SendOff();
				voice = 2;
			}


			//��ʾ��Ƶ  
			imshow("video", currentBGRFrame);

			//ǰ�����  
			imshow("segmentation", segmentation);

			//���浱ǰ֡�ĻҶ�ͼ  
			previousFirstGrayFrame = currentGaryFrame.clone();

			//���浱ǰ��ǰ�����  
			previousSegmentation = currentSegmentation.clone();
		}
		waitKey(50);
	}
	return 0;
}


void SendOff()
{
	//�ͱ�  
	const int ONE_BEEP = 600;
	const int HALF_BEEP = 300;

	const int NOTE_1 = 440;
	const int NOTE_2 = 495;
	const int NOTE_3 = 550;
	const int NOTE_4 = 587;
	const int NOTE_5 = 660;
	const int NOTE_6 = 733;
	const int NOTE_7 = 825;

	//��ͤ��  
	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP);

}