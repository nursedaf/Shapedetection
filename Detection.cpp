#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;


int GetAngleABC(Point a, Point b, Point c)
{
	Point ab = { b.x - a.x, b.y - a.y };
	Point cb = { b.x - c.x, b.y - c.y };

	float dot = (ab.x * cb.x + ab.y * cb.y); 
	float cross = (ab.x * cb.y - ab.y * cb.x); 

	float alpha = atan2(cross, dot);

	return (int)floor(alpha * 180. / 3.14 + 0.5);
}

int main() {
	Mat x = imread("shapes.png");
	resize(x, x, Size(682, 550));

	Mat y, z, gradient;
	cvtColor(x, y, COLOR_BGR2GRAY);
	//threshold(y, y, 50, 255, THRESH_BINARY);	//binarye çevirme
	//min deðer 250 aþaðýsý siyah yukarýsý beyaz - max 255
	
	//Closing görüntü üzerindeki nesneyi kapatýp beyaz alaný arttýrýp görüntü üzerindeki gürültüleri kaldýrýr.
	morphologyEx(y, z, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(7, 7)), Point(-1, -1), 1); 

	Mat canny; //Canny kenar bulma
	Canny(z, canny, 100, 170);

	//Gradient: hatlarý ortaya çýkarma
	morphologyEx(canny, gradient, MORPH_GRADIENT, getStructuringElement(MORPH_RECT, Size(2, 2)), Point(-1, -1), 1);
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(gradient, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	vector<Point> approx;
	Mat draw = x.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		//double alpha = 0.3;
		//double beta = (1.0 - alpha);

		//addWeighted(x, alpha, draw, beta, 0.0, draw);
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true); //görüntünün köþe sayýsýný almak için
		drawContours(draw, contours, -1, Scalar(0, 0, 255), 2);


		Rect r = boundingRect(contours[i]);
		Point pt(r.x + ((r.width) / 2), r.y + ((r.height) / 2));
		Scalar color(255, 0, 0);
	
		if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx)) // Küçük veya dýþbükey olmayan nesneleri atla
			continue;
		
		if (approx.size() == 3) 
		{
			//double line1 = sqrt(pow(approx[0].x - approx[1].x, 2) + pow(approx[0].y - approx[1].y, 2));
			//double line2 = sqrt(pow(approx[1].x - approx[2].x, 2) + pow(approx[1].y - approx[2].y, 2));
			//double line3 = sqrt(pow(approx[0].x - approx[2].x, 2) + pow(approx[0].y - approx[2].y, 2));

			int angle = GetAngleABC(approx[2], approx[0], approx[1]);
			int angle2 = GetAngleABC(approx[0], approx[1], approx[2]);
			int angle3 = GetAngleABC(approx[1], approx[2], approx[0]);

			string angleDeg = to_string(angle);
			string angleDeg2 = to_string(angle2);
			string angleDeg3 = to_string(angle3);

			putText(draw, angleDeg, approx[0], FONT_ITALIC, 0.75, color, 1);
			putText(draw, angleDeg2, approx[1], FONT_ITALIC, 0.75, color, 1);
			putText(draw, angleDeg3, approx[2], FONT_ITALIC, 0.75, color, 1);

			if (angle > 92 || angle2 > 92 || angle3 > 92) {
				//putText(draw, "Genis ", pt, FONT_ITALIC, 0.75, color, 1);
			}
			else if (angle >= 89 && angle <= 92 || angle2 >= 89 && angle2 <= 92 || angle3 >= 89 && angle3 <= 92) {
				//putText(draw, "Dik ", pt, FONT_ITALIC, 0.75, color, 1);
			}
			else if (angle >= 55 && angle <= 65 && angle2 >= 55 && angle2 <= 65 && angle3 >= 55 && angle3 <= 65) {
				//putText(draw, "Eskenar", pt, FONT_ITALIC, 0.75, color, 1);
			}
			else {
				//putText(draw, "Triangle", pt, FONT_ITALIC, 0.75, color, 1);

			}
		}

		else if (approx.size() == 4) 
		{
			double line1 = sqrt(pow(approx[1].x - approx[0].x, 2) + pow(approx[1].y - approx[0].y, 2));
			double line2 = sqrt(pow(approx[2].x - approx[1].x, 2) + pow(approx[2].y - approx[1].y, 2));

			cout << "line1:" << line1 << endl;
			cout << "line2:" << line2 << endl;
			double ratio = line1 / line2;


			int angle = GetAngleABC(approx[3], approx[0], approx[1]);
			int angle2 = GetAngleABC(approx[0], approx[1], approx[2]);
			int angle3 = GetAngleABC(approx[1], approx[2], approx[3]);
			int angle4 = GetAngleABC(approx[2], approx[3], approx[0]);

			string angleDeg = to_string(angle);
			string angleDeg2 = to_string(angle2);
			string angleDeg3 = to_string(angle3);
			string angleDeg4 = to_string(angle4);

			double diff = angle - angle3;
			double diff2 = angle2 - angle4;
			cout << diff << endl;
			cout << diff2 << endl;

			putText(draw, angleDeg, approx[0], FONT_ITALIC, 0.75, color, 1);
			putText(draw, angleDeg2, approx[1], FONT_ITALIC, 0.75, color, 1);
			putText(draw, angleDeg3, approx[2], FONT_ITALIC, 0.75, color, 1);
			putText(draw, angleDeg4, approx[3], FONT_ITALIC, 0.75, color, 1);

			if (angle >= 89 && angle <= 92 || angle2 >= 89 && angle2 <= 92 || angle3 >= 89 && angle3 <= 92) {
				if (ratio >= 0.95 && ratio < 1.05) {
					//putText(draw, "Square", pt, FONT_ITALIC, 0.75, color, 1);
				}
				else {
					//putText(draw, "Rectangle", pt, FONT_ITALIC, 0.75, color, 1);
				}
			}
			else if(diff >= -5 && diff <= 5 && diff2 >= -5 && diff2 <= 5) {
				if (ratio >= 0.95 && ratio < 1.05) {
					//putText(draw, "Eskenar Dortgen", pt, FONT_ITALIC, 0.75, color, 1);
				}
				else {
					//putText(draw, "Paralel", pt, FONT_ITALIC, 0.75, color, 1);
				}
			}
			else {
				//putText(draw, "Dortgen", pt, FONT_ITALIC, 0.75, color, 1);
			}
		}
		else if (approx.size() == 5)
		{
			putText(draw, "Penta", pt, FONT_ITALIC, 0.75, color, 1);
		}
		else if (approx.size() == 6)
		{
			putText(draw, "Hexa", pt, FONT_ITALIC, 0.75, color, 1);
		}
		else if (approx.size() == 8)
		{
			putText(draw, "Octa", pt, FONT_ITALIC, 0.75, color, 1);
		}
		else
		{
			double area = contourArea(contours[i]);
			int radius = r.width / 2;

			if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2) {
				putText(draw, "Circle", pt, FONT_ITALIC, 0.75, color, 1);
			}
			else if (approx.size() >= 0 && approx.size() < 15)
			{
				putText(draw, "Ellipse", pt, FONT_ITALIC, 0.75, color, 1);
			}
		}
	}

	//imshow("Orijinal", x);
	//imshow("THRESHBINARY", z);
	//imshow("Canny", canny);
	//imshow("Gradient", gradient);
	imshow("Contours", draw);
	waitKey(0);
}
