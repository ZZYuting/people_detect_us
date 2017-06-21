/********************************************

2017.6.19测试zzuyuting

使用基于统计学习的svm+hog算法

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

	//打开视频文件：其实就是建立一个VideoCapture结构
	VideoCapture capture("D:/测试.mp4");	//打开视频
	//VideoCapture capture(0);//打开摄像头
	double c;
	int flag,count=0,count2=0;
	double c1[1000];
	vector<Point> c2;
	//检测是否正常打开:成功打开时，isOpened返回ture

	if (!capture.isOpened())
		cout << "fail to open!" << endl;

	//滤波器的核

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
		//读取下一帧
		if (!capture.read(frame))

		{

			cout << "处理视频结束" << endl;
			break;

		}

	


		////这里加滤波程序

		imshow("待检测视频", frame);

		if (frame.empty())

		{

			cout << "read image failed" << endl;

		}



		// 1. 定义HOG对象  

		HOGDescriptor hog; // 采用默认参数  





		// 2. 设置SVM分类器  

		hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());   // 采用已经训练好的行人检测分类器  



		// 3. 在测试图像上检测行人区域  

		vector<Rect> found, found_filtered;//矩形框数组  

		CvSize size;

		size.height = 270;

		size.width = 480;

		resize(frame, frame, size);

		hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.05, 1);

		//找出所有没有嵌套的矩形框r,并放入found_filtered中,如果有嵌套的话,则取外面最大的那个矩形框放入found_filtered中  
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

		//画矩形框，因为hog检测出的矩形框比实际人体框要稍微大些,所以这里需要做一些调整  
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
					rectangle(frame, r.tl(), r.br(), Scalar(0, 0, 255), 3);//画红色矩形
					red_count++;
			}
			if(flag1==0){
				rectangle(frame, r.tl(), r.br(), Scalar(0, 255, 0), 3);//绿色矩形
			}
			a = r.width/10;
			b = r.height/10;
			c = a / b;
			Point cpt;
			cpt.x = r.x + cvRound(r.width / 2.0);							//质心
			cpt.y = r.y + cvRound(r.height / 2.0);
			c2.push_back(cpt);
			//printf("宽=%.2f,高=%.2f,宽高比=%.2f \n", a, b, c);//高宽比
			//printf("x=%d,y=%d \n", cpt.x, cpt.y);//质心
			flag = 1;
			count2++;
		}
		imshow("异常行为检测", frame);
		if (flag == 0){														//跌倒检测
			count2--;
			c1[count] = c;			//存入高宽比，count为当前frame的序号
			/*c2[count] = cpt;*/
			int count1 = count - 1;	//count1为前一帧的序号
			count++;
			if (count > 20)			//连续20帧没有检测到行人，因子u1
			{
					//printf("count=%d,c1[%d]=%.2f,count2=%d\n",count,count1,c1[count1],count2);

				if (count1 > 0 && (c1[count1] <=c) && count2>0)			//高宽增加超过阈值，因子u2
				{
					double a2 = c2[count2].x - c2[count2-1].x, b2 = c2[count2].y - c2[count2-1].y;			//质心的变化
					//printf(" count=%d,a=%.2f,b=%.2f\n", count, a2, b2);
					a2=fabs(a2), b2=fabs(b2);																//取绝对值
					//printf(" count=%d,a=%.2f,b=%.2f\n", count, a2, b2);
					if (a2 > 2 || b2 > 2)																	//质心变化超出阈值，因子u3
					{
						printf("跌倒 count=%d,a=%.2f,b=%.2f\n", count,a2,b2);		
						//voice=!voice;
						voice++;																			//保证摔倒时候才警报
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
	//关闭视频文件

	capture.release();

	waitKey(0);
	return 0;

}

void SendOff()
{
	//送别  
	const int ONE_BEEP = 600;
	const int HALF_BEEP = 300;

	const int NOTE_5 = 660;

	Beep(NOTE_5, ONE_BEEP);
	Beep(NOTE_5, ONE_BEEP);
}
