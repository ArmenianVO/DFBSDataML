#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#define __validate255(x) x = x > 255 ? 255 : x;
#define __validate0(x) x = x < 0 ? 0 : x;

namespace app {



	struct Point
	{
		int x, y;
	};

	struct Rect
	{
		Point stLeftTop;
		Point stRightBottom;
	};

	struct Rect4
	{
		int top{};
		int bottom{};
		int left{};
		int right{};

		bool PtOnRect(int x, int y) {
			return x >= left && x <= right && y >= top && y <= bottom;
		}

		bool PtInRect(int x, int y) {
			return false;
		}

	};

	struct Elipse
	{
		int cx{};
		int cy{};
		int rx{};
		int ry{};

		Elipse(const Rect4& rect) {
			rx = (rect.right - rect.left) / 2;
			ry = (rect.bottom - rect.top) / 2;
			cx = rect.left + rx;
			cy = rect.top + ry;
		}

		bool PtInElipse(int x, int y) {
			return (((x - cx) * (x - cx)) / (rx * rx)) + (((y - cy) * (y - cy)) / (ry * ry)) <= 1;
		}

	};

	namespace {
		const std::string SrcPath1 = "fbs0066-DFBSJ082211.84+390149.4.jpg";
		const std::string SrcPath2 = "fbs0036-DFBSJ232743.00+271924.4.jpg";
		const std::string SrcPath3 = "fbs0059-DFBSJ080211.41+385840.2.jpg";
		const std::string SrcPath4 = "fbs0066-DFBSJ082210.13+390104.0.jpg";
		const std::string SrcPath5 = "fbs0123-DFBSJ005214.97+270936.2.jpg";
		const std::string SrcPath6 = "fbs0131-DFBSJ021729.69+271304.2.jpg";
		const std::string SrcPath7 = "fbs1309-DFBSJ041619.14-005522.1.jpg";
		const std::string SrcPath8 = "fbs0785M-DFBSJ071746.00+470631.8.jpg";
		const std::string SrcPathOut1 = "fbs0066-DFBSJ082211.84+390149.4_out.jpg";
		const std::string SrcPathOut2 = "fbs0036-DFBSJ232743.00+271924.4_out.jpg";
		const std::string SrcPathOut3 = "fbs0059-DFBSJ080211.41+385840.2_out.jpg";
		const std::string SrcPathOut4 = "fbs0066-DFBSJ082210.13+390104.0_out.jpg";
		const std::string SrcPathOut5 = "fbs0123-DFBSJ005214.97+270936.2_out.jpg";
		const std::string SrcPathOut6 = "fbs0131-DFBSJ021729.69+271304.2_out.jpg";
		const std::string SrcPathOut7 = "fbs1309-DFBSJ041619.14-005522.1_out.jpg";
		const std::string SrcPathOut8 = "fbs0785M-DFBSJ071746.00+470631.8_out.jpg";
	}

	void Segmentation(cv::Mat& src, const int t, const int iRowCount) {

		std::vector<int> hist(256, 0);

		int iWidth = src.cols;
		int iHeight = src.rows;

		//int max = 0;
		//int min = std::numeric_limits<int>::max();

		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				auto pixel = src.at<cv::Vec3b>(y, x);
				int result = (pixel[0] + pixel[1] + pixel[2]) / 3;
				src.at<cv::Vec3b>(y, x)[0] = result;
				src.at<cv::Vec3b>(y, x)[1] = result;
				src.at<cv::Vec3b>(y, x)[2] = result;
				hist[result] += 1;
				//max = std::max(max, result);
				//min = std::min(min, result);
			}
		}

		/*std::cout << max << std::endl;
		std::cout << min << std::endl;

		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				auto pixelVal = src.at<cv::Vec3b>(y, x)[0];
				if (pixelVal > t) {
					src.at<cv::Vec3b>(y, x)[0] = 255;
					src.at<cv::Vec3b>(y, x)[1] = 255;
					src.at<cv::Vec3b>(y, x)[2] = 255;
				}
				else {
					src.at<cv::Vec3b>(y, x)[0] = 0;
					src.at<cv::Vec3b>(y, x)[1] = 0;
					src.at<cv::Vec3b>(y, x)[2] = 0;
				}
			}
		}*/


		int avg = 0;
		int count = 0;
		for (int i = 1; i < hist.size(); i++)
		{
			avg += i * hist[i];
			count += hist[i];
		}

		int th = avg / count;
		th += th << 1;
		__validate255(th);


		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				auto pixelVal = src.at<cv::Vec3b>(y, x)[0];
				if (pixelVal > th) {
					src.at<cv::Vec3b>(y, x)[0] = 255;
					src.at<cv::Vec3b>(y, x)[1] = 255;
					src.at<cv::Vec3b>(y, x)[2] = 255;
				}
				else {
					src.at<cv::Vec3b>(y, x)[0] = 0;
					src.at<cv::Vec3b>(y, x)[1] = 0;
					src.at<cv::Vec3b>(y, x)[2] = 0;
				}
			}
		}

	}

	void Erosion(cv::Mat& src, int iSize) {
		int iWidth = src.cols;
		int iHeight = src.rows;

		for (int x = 0; x < iWidth; x++)
		{
			bool segment = false;
			int iBegin = 0;
			int iEnd = 0;
			for (int y = 0; y < iHeight; y++)
			{
				auto pixel = src.at<uchar>(y, x);
				if (pixel == 255) {
					if (segment) {
						int d = iEnd - iBegin;
						if (d <= iSize) {
							for (int dy = iBegin; dy <= iEnd; dy++)
							{
								src.at<uchar>(dy, x) = 255;
							}
						}
						segment = false;
					}
				}
				else {
					if (!segment) {
						iBegin = y;
						segment = true;
					}
					iEnd = y;
				}
			}
		}

	}

	Rect4 RectSegmentation(cv::Mat& src) {

		Rect4 result;
		int iWidth = src.cols;
		int iHeight = src.rows;

		int cX = (iWidth + 1) >> 1;
		int cY = (iHeight + 1) >> 1;

		std::vector<bool> lines;

		while (true)
		{
			auto pixel = src.at<uchar>(cY, cX);

			if (pixel == 0) {
				bool iterTop = true;
				bool iterBottom = true;
				int yTop = cY;
				int yBottom = cY;
				while (iterTop || iterBottom)
				{
					auto topPixel = src.at<uchar>(yTop, cX);
					auto bottomPixel = src.at<uchar>(yBottom, cX);

					if (topPixel == 0 && iterTop) {
						--yTop;
					}
					else {
						iterTop = false;
					}

					if (bottomPixel == 0 && iterBottom) {
						++yBottom;
					}
					else {
						iterBottom = false;
					}

				}

				result.top = yTop - 5 >= 0 ? yTop - 5 : yTop;
				result.bottom = yBottom + 10 < iHeight ? yBottom + 10 : yBottom;

				bool iterLeft = true;
				bool iterRight = true;
				int xLeft = cX;
				int xRight = cX;

				while (iterLeft || iterRight)
				{
					int countXLeft = 0;
					int countXRight = 0;
					for (int y = result.top; y <= result.bottom; y++)
					{

						auto pixelLeft = src.at<uchar>(y, xLeft);
						auto pixelRight = src.at<uchar>(y, xRight);

						if (pixelLeft == 0) {
							++countXLeft;
						}
						if (pixelRight == 0) {
							++countXRight;
						}
					}

					if (countXLeft > 0 && iterLeft) {
						--xLeft;
					}
					else {
						iterLeft = false;
					}

					if (countXRight > 0 && iterRight) {
						++xRight;
					}
					else {
						iterRight = false;
					}

				}

				result.left = xLeft - 2 >= 0 ? xLeft - 2 : xLeft;
				result.right = xRight + 2 < iWidth ? xRight + 2 : xRight;
				break;
			}
			else {
				cY--;
			}
		}

		return result;

	}

	void DrawRectContour(cv::Mat& src, Rect4 rect)
	{
		int iWidth = src.cols;
		int iHeight = src.rows;

		for (int y = 0; y < iHeight; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				if (!rect.PtOnRect(x, y)) {
					src.at<uchar>(y, x) = 0;
				}
			}
		}

	}

	void EdgeErosion(cv::Mat& src) {

		int iWidth = src.cols;
		int iHeight = src.rows;

		for (int y = 1; y < iHeight - 1; y++)
		{
			for (int x = 1; x < iWidth - 1; x++)
			{
				auto current = src.at<uchar>(y, x);
				auto left = src.at<uchar>(y, x - 1);
				auto right = src.at<uchar>(y, x + 1);
				auto top = src.at<uchar>(y - 1, x);
				auto bottom = src.at<uchar>(y + 1, x);

				if (current == 0) {
					if (left == 255 || right == 255 || top == 255 || bottom == 255) {
						src.at<uchar>(y, x) = 125;
					}
				}

			}
		}

	}

	void GradImage(cv::Mat& src) {

		int iWidth = src.cols;
		int iHeight = src.rows;

		for (int y = 1; y < iHeight - 1; y++)
		{
			for (int x = 1; x < iWidth - 1; x++)
			{
				auto current = src.at<uchar>(y, x);
				auto left = src.at<uchar>(y, x - 1);
				auto right = src.at<uchar>(y, x + 1);
				auto top = src.at<uchar>(y - 1, x);
				auto bottom = src.at<uchar>(y + 1, x);
				auto left_top = src.at<uchar>(y - 1, x - 1);
				auto right_top = src.at<uchar>(y + 1, x - 1);
				auto left_bottom = src.at<uchar>(y - 1, x + 1);
				auto right_bottom = src.at<uchar>(y + 1, x + 1);

				int gx = (left_top + 2 * left + left_bottom +
					(-1) * right_top + (-2) * right + (-1) * right_bottom);

				int gy = (left_top + (2) * top + right_top +
					(-1) * left_bottom + (-2) * bottom + (-1) * right_bottom);


				int g = std::sqrt(gx * gx + gy * gy);

				src.at<uchar>(y, x) = g;

			}
		}

	}

	void RectSegmentationV2(cv::Mat& src) {

		Rect4 result;
		int iWidth = src.cols;
		int iHeight = src.rows;

		int cX = (iWidth + 1) >> 1;
		int cY = (iHeight + 1) >> 1;

		while (true)
		{
			auto current = src.at<uchar>(cY, cX);

			if (current == 0) {

			}
			else if (current == 125) {

			}
			else {
				cY--;
			}

		}

	}
}
//V1
//const int Color1 = 50;
//const int Color2 = 150;
//int main() {
//
//
//	cv::Mat img = cv::imread(app::SrcPath8);
//	cv::Mat gray = img.clone();
//	cv::Mat grayResult = img.clone();
//	std::cout << img.cols << std::endl;
//	std::cout << img.rows << std::endl;
//
//	cv::Mat blured = img.clone();
//	cv::Mat resultImg = img.clone();
//	cv::GaussianBlur(img, blured, cv::Size(3, 3), 0, 0);
//
//	cv::cvtColor(blured, gray, cv::COLOR_RGBA2GRAY, 0);
//	cv::cvtColor(img, grayResult, cv::COLOR_RGBA2GRAY, 0);
//	cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
//
//	//Segmentation(blured, 128, 10);
//
//	//DrawContours(bluredCopy, blured);
//
//	/*Erosion(gray, 5);
//	Rect4 result = RectSegmentation(gray);
//	DrawRectContour(grayResult, result);*/
//
//	app::EdgeErosion(gray);
//
//
//	//imwrite(SrcPathOut7, grayResult);
//	cv::namedWindow("image", cv::WINDOW_NORMAL);
//	cv::imshow("image", gray);
//	cv::waitKey(0);
//
//	return 0;
//}

//V2
using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

std::vector<std::vector<Point>> thresh_callback(int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(src_gray, canny_output, 80,255, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	/*for (int i = 0; i < contours.size(); i++)
	{*/
		/*Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, 0, color, 2, 8, hierarchy, 0, Point());*/
	/*}*/

	/// Show in a window
	namedWindow("Contours", WINDOW_NORMAL);
	imshow("Contours", drawing);

	return contours;
}

app::Rect4 GetCentralRect(std::vector<std::vector<Point>>& contours, int iWidth, int iHeight) {
	app::Rect4 result;

	int cx = (iWidth + 1) >> 1;

	std::vector<std::vector<Point>> result_c;

	/*std::for_each(contours.begin(), contours.end(), [&](std::vector<Point>& cont) {
		std::for_each(cont.begin(), cont.end(), [&](Point& point) {
			
		});
	});*/

	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			Point& point = contours[i][j];
			if (point.x == cx) {
				result_c.push_back(contours[i]);
				break;
			}
		}
	}

	int max_x{};
	int min_x{};
	int max_y{};
	int min_y{};

	if (result_c.size() > 0) {
		std::vector<Point>& points = result_c[0];
		max_x = points[0].x;
		min_x = points[0].x;
		max_y = points[0].y;
		min_y = points[0].y;

		std::for_each(points.begin() + 1, points.end(), [&](Point& p) {
			int x = p.x;
			int y = p.y;

			if (x > max_x) {
				max_x = x;
			}
			else if(x < min_x) {
				min_x = x;
			}

			if (y > max_y) {
				max_y = y;
			}
			else if (y < min_y) {
				min_y = y;
			}

		});

	}
	else {
		//TODO
	}

	result.top = (min_y - 6) >= 0 ? min_y - 6 : min_y;
	result.bottom = (max_y + 6) <= iHeight ? max_y + 6 : max_y;
	result.left = min_x - 2 >= 0 ? min_x - 2 : min_x;
	result.right = max_x + 2 <= iWidth ? max_x + 2 : max_x;

	return result;
}

int main() {


	src = cv::imread(app::SrcPath8);
	Mat result_image = src.clone();
	int iWidth = src.cols;
	int iHeight = src.rows;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	cvtColor(result_image, result_image, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
	

	//createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
	std::vector<std::vector<Point>> contours =  thresh_callback(0, 0);
	app::Rect4 result = GetCentralRect(contours, iWidth, iHeight);

	app::DrawRectContour(result_image, result);

	imwrite(app::SrcPathOut8, result_image);
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", result_image);
	cv::waitKey(0);

	return 0;
}