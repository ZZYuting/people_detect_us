/********************************************

2017.6.19����zzuyuting

ʹ�û���ͳ��ѧϰ��svm+hog�㷨

*******************************************/
#include <opencv2/core/core.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include <opencv2/opencv.hpp>

#include <windows.h>  

using namespace std;

using namespace cv;

void SendOff();


int main()

{

	//����Ƶ�ļ�����ʵ���ǽ���һ��VideoCapture�ṹ
	VideoCapture capture("D:/����.mp4");	//����Ƶ
	//VideoCapture capture(0);//������ͷ
	double c;
	int flag,count=0,count2=0;
	double c1[1000];
	vector<Point> c2;
	//����Ƿ�������:�ɹ���ʱ��isOpened����ture

	if (!capture.isOpened())
		cout << "fail to open!" << endl;

	//�˲����ĺ�

	int kernel_size = 3;
	Mat kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);

	int flag1 = 0, flag2 = 0;
	int voice=0;
	int red_count=0;
	//SendOff();
	while (1)

	{
		flag = 0;
		Mat frame;
		capture >> frame;
		//printf("count1=%d \n", count1);
		//��ȡ��һ֡
		if (!capture.read(frame))

		{

			cout << "������Ƶ����" << endl;
			break;

		}

	


		////������˲�����

		imshow("�������Ƶ", frame);

		if (frame.empty())

		{

			cout << "read image failed" << endl;

		}





		// 1. ����HOG����  

		HOGDescriptor hog; // ����Ĭ�ϲ���  





		// 2. ����SVM������  

		hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());   // �����Ѿ�ѵ���õ����˼�������  



		// 3. �ڲ���ͼ���ϼ����������  

		vector<Rect> found, found_filtered;//���ο�����  

		CvSize size;

		size.height = 270;

		size.width = 480;

		resize(frame, frame, size);

		hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.05, 1);

		//�ҳ�����û��Ƕ�׵ľ��ο�r,������found_filtered��,�����Ƕ�׵Ļ�,��ȡ���������Ǹ����ο����found_filtered��  
		for (int i = 0; i < found.size(); i++)
		{
			Rect r = found[i];
			int j = 0;
			for (; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
			if (j == found.size())
				found_filtered.push_back(r);
		}

		//�����ο���Ϊhog�����ľ��ο��ʵ�������Ҫ��΢��Щ,����������Ҫ��һЩ����  
		for (int i = 0; i<found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];
			double  a, b;
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			if (red_count>13)
			{
				flag1 = 0;
				red_count = 0;
			}
			//printf("red_count=%d\n", red_count);
			if (flag1 == 1&&flag==0){
					rectangle(frame, r.tl(), r.br(), Scalar(0, 0, 255), 3);//����ɫ����
					red_count++;
			}
			if(flag1==0){
				rectangle(frame, r.tl(), r.br(), Scalar(0, 255, 0), 3);//��ɫ����
			}
			a = r.width/10;
			b = r.height/10;
			c = a / b;
			Point cpt;
			cpt.x = r.x + cvRound(r.width / 2.0);							//����
			cpt.y = r.y + cvRound(r.height / 2.0);
			c2.push_back(cpt);
			//printf("��=%.2f,��=%.2f,��߱�=%.2f \n", a, b, c);//�߿��
			//printf("x=%d,y=%d \n", cpt.x, cpt.y);//����
			flag = 1;
			count2++;
		}
		imshow("�쳣��Ϊ���", frame);
		if (flag == 0){														//�������
			count2--;
			c1[count] = c;			//����߿�ȣ�countΪ��ǰframe�����
			/*c2[count] = cpt;*/
			int count1 = count - 1;	//count1Ϊǰһ֡�����
			count++;
			if (count > 20)			//����20֡û�м�⵽���ˣ�����u1
			{
					//printf("count=%d,c1[%d]=%.2f,count2=%d\n",count,count1,c1[count1],count2);

				if (count1 > 0 && (c1[count1] <=c) && count2>0)			//�߿����ӳ�����ֵ������u2
				{
					double a2 = c2[count2].x - c2[count2-1].x, b2 = c2[count2].y - c2[count2-1].y;			//���ĵı仯
					//printf(" count=%d,a=%.2f,b=%.2f\n", count, a2, b2);
					a2=fabs(a2), b2=fabs(b2);																//ȡ����ֵ
					//printf(" count=%d,a=%.2f,b=%.2f\n", count, a2, b2);
					if (a2 > 2 || b2 > 2)																	//���ı仯������ֵ������u3
					{
						printf("���� count=%d,a=%.2f,b=%.2f\n", count,a2,b2);		
						//voice=!voice;
						voice++;																			//��֤ˤ��ʱ��ž���
						printf("voice=%d\n", voice);
						flag1 = 1;
						if (red_count<=13)
						{
							flag2 = 1;
						}
						count = 0;
					}
				}
			}
		}
		if (voice == 1){
			SendOff();
			voice=2;
		}
		else{
			waitKey(30);
		}

	}
	//�ر���Ƶ�ļ�

	capture.release();

	waitKey(0);
	return 0;

}

void SendOff()
{
	//�ͱ�  
	const int ONE_BEEP = 600;
	const int HALF_BEEP = 300;

	const int NOTE_5 = 660;

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP);
}