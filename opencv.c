#include <opencv2/opencv.hpp>
#include <string.h>
using namespace cv;
using namespace std;

Point kpt[1024];
Point Answer[3];

void setLabel(Mat& image, string str, vector<Point> contour, int index)
{
	int fontface = FONT_HERSHEY_SIMPLEX;
	double scale = 0.5;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(str, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width) / 2), r.y + ((r.height) / 2));
	kpt[index] = pt;
}

#define RESOURSE_COUNT 7
#define GAMETIME 60 * 100
#define LIFE 3

#define TITLE 0
#define PLAY 1
#define LEVEL 2
#define OVER 3

RNG rng(getTickCount());
static int match[RESOURSE_COUNT] = { 0, };
static int matchCount = RESOURSE_COUNT;
static Point hidePoint[RESOURSE_COUNT];
static Point lifePoint[RESOURSE_COUNT];
static int life = LIFE;
static int timer = GAMETIME;
static Mat copyImage;
static int mode = TITLE;

Rect ground = Rect(609, 147, 500, 466);
int ansFlag[3] = { 0, 0, 0 };

void drawCircle(Point pt, void* param)
{
	Mat& img = *(Mat*)(param);
	circle(img, Point(pt.x, pt.y), 25, Scalar(0, 255, 0), 5);
}
void drawX(Point pt, void* param)
{
	Mat& img = *(Mat*)(param);

	line(img, Point(pt.x - 25, pt.y - 25), Point(pt.x + 25, pt.y + 25), Scalar(0, 0, 255), 5);
	line(img, Point(pt.x + 25, pt.y - 25), Point(pt.x - 25, pt.y + 25), Scalar(0, 0, 255), 5);
}

void onMouse(int event, int x, int y, int flags, void* param)
{
	// IMAGE RECT POINT init
	Mat temp2;
	Mat temp = imread("title.jpg", IMREAD_COLOR);
	Mat title;
	resize(temp, title, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
	temp = imread("play.jpg", IMREAD_COLOR);
	Mat play;
	resize(temp, play, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
	temp = imread("level.png", IMREAD_COLOR);
	Mat clear;
	resize(temp, clear, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
	temp = imread("clear.jpg", IMREAD_COLOR);
	Mat over;
	resize(temp, over, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
	temp = imread("level.png", IMREAD_COLOR);
	Mat level;
	resize(temp, level, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
	

	// clone Mat param
	copyImage = (*(Mat*)(param)).clone();

	// click point
	Point click_pt(x, y);
	// 누른 위치가 오답인지 확인하는 플래그
	int failFlag = 0;

	if (event == EVENT_LBUTTONUP) {

		if (mode == TITLE)
		{
			//============================
			// TITLE
			//============================
			mode = LEVEL;
			Mat& img = *(Mat*)(param);
			img = level.clone();
			imshow("baseImage", img);

		}
		else if (mode == LEVEL)
		{
			if (Rect(228, 293, 748, 47).contains(Point(x, y)))
			{
				timer = 10 * 100;
			}
			else if (Rect(228, 363, 748, 47).contains(Point(x, y)))
			{
				timer = 30 * 100;
			}
			else if (Rect(228, 433, 748, 47).contains(Point(x, y)))
			{
				timer = 60 * 100;
			}
			else
			{
				return;
			}

			printf("pos : %d %d\n", x, y);
			mode = PLAY;
			Mat& img = *(Mat*)(param);
			img = play.clone();
			int random = rng.uniform(0, RESOURSE_COUNT);
			printf("%d\n", random);
			switch (random)
			{
			case 0:
				temp = imread("resource0.jpg", IMREAD_COLOR);
				temp2 = imread("copy0.jpg", IMREAD_COLOR);
				break;
			case 1:
				temp = imread("resource1.jpg", IMREAD_COLOR);
				temp2 = imread("copy1.jpg", IMREAD_COLOR);
				break;
			case 2:
				temp = imread("resource2.jpg", IMREAD_COLOR);
				temp2 = imread("copy2.jpg", IMREAD_COLOR);
				break;
			case 3:
				temp = imread("resource3.jpg", IMREAD_COLOR);
				temp2 = imread("copy3.jpg", IMREAD_COLOR);
				break;
			case 4:
				temp = imread("resource4.jpg", IMREAD_COLOR);
				temp2 = imread("copy4.jpg", IMREAD_COLOR);
				break;
			case 5:
				temp = imread("resource5.jpg", IMREAD_COLOR);
				temp2 = imread("copy5.jpg", IMREAD_COLOR);
				break;
			case 6:
				temp = imread("resource6.jpg", IMREAD_COLOR);
				temp2 = imread("copy6.jpg", IMREAD_COLOR);
				break;
			default:
				temp = imread("resource0.jpg", IMREAD_COLOR);
				temp2 = imread("copy0.jpg", IMREAD_COLOR);
				break;
			};
			Mat baseImage;
			resize(temp, baseImage, Size(500, 446), 0, 0, CV_INTER_LINEAR);
			Mat copyImage;
			resize(temp2, copyImage, Size(500, 446), 0, 0, CV_INTER_LINEAR);

			Mat a;
			Mat gray_a;
			Mat binary_a;
			absdiff(baseImage, copyImage, a); // 다른곳 찾기
			cvtColor(a, gray_a, COLOR_BGR2GRAY); // 그레이 스케일
			threshold(gray_a, binary_a, 16, 255, THRESH_BINARY); // 이진화

			vector<vector<Point> > contours;
			findContours(binary_a, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			//contour를 근사화한다.
			Mat img_result;
			vector<Point2f> approx;
			img_result = binary_a.clone();

			printf("%d\n", contours.size());
			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

				if (fabs(contourArea(Mat(approx))) > 35)  //면적이 일정크기 이상이어야 한다. 
				{

					int size = approx.size();

					//Contour를 근사화한 직선을 그린다.
					if (size % 2 == 0) {
						line(img_result, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 3);

						for (int k = 0; k < size - 1; k++)
							line(img_result, approx[k], approx[k + 1], Scalar(0, 255, 0), 3);

						for (int k = 0; k < size; k++)
							circle(img_result, approx[k], 3, Scalar(0, 0, 255));
					}
					else {
						line(img_result, approx[0], approx[approx.size() - 1], Scalar(0, 255, 0), 3);

						for (int k = 0; k < size - 1; k++)
							line(img_result, approx[k], approx[k + 1], Scalar(0, 255, 0), 3);

						for (int k = 0; k < size; k++)
							circle(img_result, approx[k], 3, Scalar(0, 0, 255));
					}

					setLabel(img_result, to_string(approx.size()), contours[i], i);
				}
			}
			Mat copy;
			copy = img_result.clone();
			int index = 0;
			for (int i = 0; i < contours.size(); i++)
			{
				if (kpt[i] == Point(0, 0)) continue;
				Answer[index++] = kpt[i];
				drawCircle(Answer[index - 1], &copy);
				printf("%d %d\n", Answer[index - 1].x, Answer[index - 1].y);
			}


			//imshow("Canny Edge", copy);
			//imshow("A", binary_a);
			//=========================================================================
			//=========================================================================
			//=========================================================================
			//=========================================================================

			Rect r;
			r = Rect(90, 147, 500, 446);
			Mat roi(img, r);
			baseImage.copyTo(roi);
			Rect r2;
			r2 = Rect(609, 147, 500, 446);
			Mat roi2(img, r2);
			copyImage.copyTo(roi2);

			imshow("baseImage", img);
		}
		else if (mode == PLAY)
		{
			//============================
			// PLAY
			//============================
			Mat& img = *(Mat*)(param);	

			if (ground.contains(Point(x, y)))
			{
				
				if (Rect(Answer[0].x + 609 - 25, Answer[0].y + 147 - 25, 50, 50).contains(Point(x, y)))
				{
					ansFlag[0] = 1;
					drawCircle(Point(Answer[0].x + 609, Answer[0].y + 147), param);
				}
				else if (Rect(Answer[1].x + 609 - 25, Answer[1].y + 147 - 25, 50, 50).contains(Point(x, y)))
				{
					ansFlag[1] = 1;
					drawCircle(Point(Answer[1].x + 609, Answer[1].y + 147), param);
				}
				else if (Rect(Answer[2].x + 609 - 25, Answer[2].y + 147 - 25, 50, 50).contains(Point(x, y)))
				{
					ansFlag[2] = 1;
					drawCircle(Point(Answer[2].x + 609, Answer[2].y + 147), param);
				}
				else
				{
					life--;
					drawX(Point(x, y), param);
				}
			}
			imshow("baseImage", img);

		}
		else if (mode == OVER)
		{
		//============================
		// GAMEOVER
		//============================
		mode = TITLE;
		Mat& img = *(Mat*)(param);
		img = title.clone();
		imshow("baseImage", img);
		}




	}

	else if (event == EVENT_RBUTTONDOWN) {}
	else if (event == EVENT_MBUTTONDOWN) {}
	else if (event == EVENT_MOUSEMOVE) {
	}
}


void decToStr(int dec, char* str);
// OnTimer Function
void onTimer(void* param) {
	// TIME OVER일때
	if((ansFlag[0] == 1)&& (ansFlag[1] == 1)&& (ansFlag[2] == 1))
	{
		for (int i = 0; i < 3; i++)
		{
			ansFlag[i] = 0;
		}
		Mat clear;
		Mat temp = imread("clear.jpg", IMREAD_COLOR);
		resize(temp, clear, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
		mode = OVER;

		imshow("baseImage", clear);
	}
	else if (timer == GAMETIME)
	{

	}
	else if (timer < 0) {
		Mat over;
		Mat temp = imread("gameover.jpg", IMREAD_COLOR);
		resize(temp, over, Size(1200, 645), 0, 0, CV_INTER_LINEAR);
		printf("[Test]Time Over\n");
		Mat& img = *(Mat*)(param);
		mode = OVER;

		imshow("baseImage", over);
	}
	// Play 중일때
	else {
		


		char timeStr[20] = "";
		decToStr(timer/100, timeStr);
		//printf("[Test]onTimer : %s\n", timeStr);
		// clone Mat param
		copyImage = (*(Mat*)(param)).clone();
		Mat& img = *(Mat*)(param);
		putText(copyImage, timeStr, Point(img.cols / 2 - 40, 100), FONT_HERSHEY_PLAIN, 4.0, Scalar(255, 255, 255), 5);


		Mat lifeImage[RESOURSE_COUNT];
		Rect r[RESOURSE_COUNT];
		for (int i = 0; i < life; i++) {
			lifePoint[i].x = 75 + (90 * i);
			lifePoint[i].y = 30;

			Mat temp;
			temp = imread("life.jpg", IMREAD_COLOR);
			resize(temp, lifeImage[i], Size(75, 75), 0, 0, CV_INTER_LINEAR);

			r[i] = Rect(lifePoint[i].x, lifePoint[i].y, lifeImage[i].cols, lifeImage[i].rows);
			Mat roi(copyImage, r[i]);
			lifeImage[i].copyTo(roi);
		}

		if (life == 0)
		{
			timer = 0;
		}
		imshow("baseImage", copyImage);
	}
}

void my_callback_function(void (*ptr)(void*), void* userdata = 0) {
	(*ptr)(userdata);   //calling the callback function
}
int main()
{
	// rand number init
	RNG rng(getTickCount());
	// based Image

	Mat temp = imread("title.jpg", IMREAD_COLOR);
	Mat title;
	resize(temp, title, Size(1200, 645), 0, 0, CV_INTER_LINEAR);




	imshow("baseImage", title);


	// mouse callback
	setMouseCallback("baseImage", onMouse, &title);

	// loop
	while (1) {
		// TIMER Callback Func
		my_callback_function(onTimer, &title);
		if (mode == TITLE)
		{
			timer = GAMETIME;
			life = LIFE;
			for (int i = 0; i < 1024; i++)
			{
				kpt[i] = Point(0, 0);
			}
			for (int i = 0; i < 3; i++)
			{
				Answer[i] = Point(0, 0);
			}
		}
		else if (mode == PLAY)
		{
			timer -= 2;
		}
		else if (mode == LEVEL)
		{
		}
		else if (mode == OVER)
		{
			timer = GAMETIME;
		}
		waitKey(10);
	}
	waitKey(0);
	return 0;
}


void decToStr(int dec, char* str) {
	if (0 == dec) {
		str[0] = '0';
		str[1] = '0';

		str[2] = NULL;
	}
	else if ((0 < dec) && (dec < 10)) {
		str[0] = '0';
		str[1] = '0' + (dec % 10);

		str[2] = NULL;
	}
	else if ((9 < dec) && (dec < 100)) {
		str[1] = '0' + (dec % 10);
		dec /= 10;
		str[0] = '0' + (dec % 10);

		str[2] = NULL;
	}
	else if ((99 < dec) && (dec < 1000)) {
		str[2] = '0' + (dec % 10);
		dec /= 10;
		str[1] = '0' + (dec % 10);
		dec /= 10;
		str[0] = '0' + (dec % 10);

		str[3] = NULL;
	}
}