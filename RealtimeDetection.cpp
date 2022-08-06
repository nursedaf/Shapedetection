#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace cv;
using namespace std;

int GetAngle(Point a, Point b, Point c)
{
	Point ab = { b.x - a.x, b.y - a.y };
	Point cb = { b.x - c.x, b.y - c.y };

	float dot = (ab.x * cb.x + ab.y * cb.y);
	float cross = (ab.x * cb.y - ab.y * cb.x);

	float alpha = atan2(cross, dot);

	return (int)floor(alpha * 180. / 3.14 + 0.5);
}

int main()
{
    Mat image;
    VideoCapture cam(0);

    if (!cam.isOpened()) {
        cout << "cannot open camera";
    }
	
    while (true) {
        cam >> image;
        Mat frame;
        bool bSuccess = cam.read(frame);
        char k = waitKey(30);

		Mat gray, img_blur, edges;

		cvtColor(frame, gray, COLOR_BGR2GRAY);

		GaussianBlur(gray, img_blur, Size(5,5), 1.8);
	
		Canny(img_blur, edges, 50, 100, 3, false);
		//imshow("canny", edges);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

		vector<Point> approx;
		Mat draw = frame.clone();
		
		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true); //görüntünün köþe sayýsýný almak için
			drawContours(draw, contours, -1, Scalar(0, 0, 255), 2);


			Rect r = boundingRect(contours[i]);
			Point pt(r.x + ((r.width) / 2), r.y + ((r.height) / 2));
			Point pt2(r.x + ((r.width) / 2), r.y + ((r.height) / 2)+15);
			Point pt3(r.x + ((r.width) / 2), r.y + ((r.height) / 2) +20);
			Scalar color(255, 0, 0);

			int area = contourArea(contours[i]);
			if (area < 1000 || area > 200000 || !isContourConvex(approx)) // Küçük veya dýþbükey olmayan nesneleri atla
				continue;

			//putText(draw, to_string(area), pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);

			if (approx.size() == 3) 
			{ 
				int angle = GetAngle(approx[2], approx[0], approx[1]);
				int angle2 = GetAngle(approx[0], approx[1], approx[2]);
				int angle3 = GetAngle(approx[1], approx[2], approx[0]);

				double diff = angle - angle2;
				double diff2 = angle2 - angle3;
				double diff3 = angle - angle3;
				
				if (angle > 92 || angle2 > 92 || angle3 > 92) {
					putText(draw, "Genis", pt, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1);
				}
				else if (angle >= 89 && angle <= 92 || angle2 >= 89 && angle2 <= 92 || angle3 >= 89 && angle3 <= 92) {
					putText(draw, "Dik", pt, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1);
				}
				else if (angle >= 55 && angle <= 65 && angle2 >= 55 && angle2 <= 65 && angle3 >= 55 && angle3 <= 65) {
					putText(draw, "Eskenar", pt, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1);
				}
				else if ((diff >= -5 && diff <= 5 && !(diff2 >= -5 && diff2 <= 5)) || (diff2 >= -5 && diff2 <= 5 && !(diff3 >= -5 && diff3 <= 5)) || (diff3 >= -5 && diff3 <= 5 && !(diff >= -5 && diff <= 5))) {
					putText(draw, "Ikizkenar", pt2, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 0.75);
				}
				else {
					putText(draw, "Ucgen", pt3, FONT_HERSHEY_COMPLEX_SMALL, 1, color, 1);
				}
			}
			else if (approx.size() == 4)
			{

				double line1 = sqrt(pow(approx[1].x - approx[0].x, 2) + pow(approx[1].y - approx[0].y, 2));
				double line2 = sqrt(pow(approx[2].x - approx[1].x, 2) + pow(approx[2].y - approx[1].y, 2));

				double ratio = line1 / line2;

				//açýlar
				int angle = GetAngle(approx[3], approx[0], approx[1]);
				int angle2 = GetAngle(approx[0], approx[1], approx[2]);
				int angle3 = GetAngle(approx[1], approx[2], approx[3]);
				int angle4 = GetAngle(approx[2], approx[3], approx[0]);

				double diff = angle - angle3; 
				double diff2 = angle2 - angle4;
				double diff3 = angle - angle2;
				double diff4 = angle4 - angle3;
				double diff5 = angle4 - angle;

				if (diff3 >= -5 && diff3 <= 5 && diff4 >= -5 && diff4 <= 5 && diff5 >= -5 && diff5 <= 5) { //tüm açýlar eþitse
					if (angle >= 89 && angle <= 92 || angle2 >= 89 && angle2 <= 92 || angle3 >= 89 && angle3 <= 92) { // dik açý kontrol
						if (ratio >= 0.95 && ratio < 1.05) { //kenarla eþitse
							putText(draw, "Kare", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
						}
						else {
							putText(draw, "Dikdortgen", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
						}
					}
				}//sadece karþýlýklý açýlar eþitse
				else if((diff >= -5 && diff <= 5 && diff2 >= -5 && diff2 <= 5) && !(diff3 >= -5 && diff3 <= 5 && diff4 >= -5 && diff4 <= 5 && diff5 >= -5 && diff5 <= 5)) { 
					if (ratio >= 0.95 && ratio < 1.05) { //kenarlar eþitse
						putText(draw, "Eskenar Dortgen", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
					}
					else {
						putText(draw, "Paralelkenar", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
					}
				}
				else {
					putText(draw, "Dortgen", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
				}

			}
			else if (approx.size() == 5)
			{
				putText(draw, "Besgen", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
			}
			else if (approx.size() == 6)
			{
				putText(draw, "Altýgen", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
			}
			
			else
			{
				double area = contourArea(contours[i]);
				int radius = r.width / 2;

				if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2) {
					putText(draw, "Daire", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
				}
				else if (approx.size() >= 0 && approx.size() < 15)
				{
					putText(draw, "Elips", pt, FONT_HERSHEY_COMPLEX_SMALL, 0.75, color, 1);
				}
			}
		}
		
	//imshow("Orijinal", frame);
	imshow("Draw", draw);
        if (k == 27) {
            break;
        }

    }
  
}

