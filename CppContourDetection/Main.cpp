#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#define __validate255(x) x = x > 255 ? 255 : x;
#define __validate0(x) x = x < 0 ? 0 : x;
#define __validate_size(x, y, x1, y1) x >= 0 && x < y && x1 >= 0 && x1 < y1;
#define PI 3.14

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
		const std::string SrcPath7 = "img.jpg";
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

		std::vector<std::pair<int, int>> result;

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
					if (left == 0 && right == 0 && top == 0 && bottom == 0) {
						result.push_back(std::make_pair(y,x));
					}
				}

			}
		}

		std::for_each(result.begin(), result.end(), [&](const std::pair<int, int>& p) {
			src.at<uchar>(p.first, p.second) = 255;
		});

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
Mat canny_output;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

std::vector<std::vector<Point>> thresh_callback(int, void*)
{
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	//Canny(src_gray, canny_output, 20, 60, 3, false);
	//cv::threshold(src_gray, canny_output, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
	adaptiveThreshold(src_gray, canny_output, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 99, 12);
	/// Find contours
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	/// Draw contours

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
#if 0
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}
#endif
	/// Show in a window
	namedWindow("Contours", WINDOW_NORMAL);
	imshow("Contours", canny_output);

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

namespace canny {

	void GaussianBlur(cv::Mat& src) {
		int iWidth = src.cols;
		int iHeight = src.rows;

		std::vector<std::vector<uchar>> result;
		result.resize(iHeight);
		std::for_each(result.begin(), result.end(), [&](std::vector<uchar>& width) {
			width.resize(iWidth);
		});

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				auto current = src.at<uchar>(i, j);

				auto left_top = src.at<uchar>(i - 1, j - 1);
				auto left = src.at<uchar>(i, j - 1);
				auto left_bottom = src.at<uchar>(i + 1, j - 1);
				auto top = src.at<uchar>(i - 1, j);

				auto right_top = src.at<uchar>(i - 1, j + 1);
				auto right = src.at<uchar>(i, j + 1);
				auto right_bottom = src.at<uchar>(i + 1, j + 1);
				auto bottom = src.at<uchar>(i + 1, j);

				int avg = (left_top + 2 * left + left_bottom + 2 * top + right_top + 4 * current
					+ 2 * right + right_bottom + 2 * bottom) / 16.0;

				result[i][j] = avg;
			}
		}

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{

				src.at<uchar>(i, j) = result[i][j];

			}
		}
	}

	void GetGradImage(cv::Mat& src) {
		std::cout << sizeof(4.5) << std::endl;
		auto __validate = [](double maxG, double g) -> double {
			return g * 255 / maxG;
		};

		int iWidth = src.cols;
		int iHeight = src.rows;

		std::vector<std::vector<uchar>> result;
		result.resize(iHeight);
		std::for_each(result.begin(), result.end(), [&](std::vector<uchar>& width) {
			width.resize(iWidth);
		});

		std::vector<std::vector<double>> angles(iHeight);
		for (size_t i = 0; i < angles.size(); i++)
		{
			angles[i].resize(iWidth);
		}

		int minG = 0;
		int maxG = 0;

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				uchar current = src.at<uchar>(i, j);

				double left = src.at<uchar>(i, j - 1);
				double top = src.at<uchar>(i - 1, j);
				double right = src.at<uchar>(i, j + 1);
				double bottom = src.at<uchar>(i + 1, j);

				double left_top = src.at<uchar>(i - 1, j - 1);
				double right_top = src.at<uchar>(i - 1, j + 1);
				double right_bottom = src.at<uchar>(i + 1, j + 1);
				double left_bottom = src.at<uchar>(i + 1, j - 1);

				double gx = (-1.) * left_top + (-2.) * left + (-1.) * left_bottom
					+ right_top + 2. * right + right_bottom;
				double gy = (-1.) * left_top + (-2.) * top + (-1.) * right_top
					+ left_bottom + 2. * bottom + right_bottom;

				double g = std::sqrt(gx*gx + gy*gy) / 2;

				if (maxG < g) {
					maxG = g;
				}

				double angle{};
				if ((gx != 0.0) || (gy != 0.0)) {
					angle = atan2(gy, gx) * 180.0 / PI;
				}
				else {
					angle = 0.0;
				}

				angles[i][j] = angle;
				result[i][j] = g;
			}
		}

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				src.at<uchar>(i, j) = __validate(maxG, result[i][j]);
			}
		}

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				auto pixel_value = src.at<uchar>(i, j);

				auto check = [&](int k, int l, int m, int n) {
					auto p1_v = src.at<uchar>(k, l);
					auto p2_v = src.at<uchar>(m, n);
					if (!(pixel_value >= p1_v && pixel_value >= p2_v)) {
						result[i][j] = 0;
					}
					else {
						result[i][j] = pixel_value;
					}
				};

				double angle = angles[i][j];



				if (0 <= angles[i][j] < 22.5 || 180 >= angles[i][j] >= 157.5) { // 0 degree
					check(i, j + 1, i, j - 1);
				}
				else if (22.5 <= angles[i][j] < 67.5) { // 45 degree
					check(i - 1, j + 1, i + 1, j - 1);
				}
				else if (67.5 <= angles[i][j] < 112.5) { // 90 degree
					check(i - 1, j, i + 1, j);
				}
				else if (112.5 <= angles[i][j] < 157.5) { // 130 degree
					check(i - 1, j - 1, i + 1, j + 1);
				}
				else {
					bool err = false;
				}
			}
		}

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{

				src.at<uchar>(i, j) = result[i][j];

			}
		}

	}

	void double_threshold(cv::Mat& img, int top, int bottom) {

		std::vector<std::vector<int>> result_mat(img.rows);

		for (size_t i = 0; i < result_mat.size(); i++)
		{
			result_mat[i].resize(img.cols);
		}

		for (size_t i = 1; i < img.rows - 1; i++)
		{
			for (size_t j = 1; j < img.cols - 1; j++)
			{
				if (img.at<uchar>(i, j) >= top) {
					img.at<uchar>(i, j) = 255;
				}
				else if (img.at<uchar>(i, j) <= bottom) {
					img.at<uchar>(i, j) = 0;
				}
				else {
					img.at<uchar>(i, j) = 20;
				}
			}
		}


	}

	void hysteresis(cv::Mat& src, int weak, int strong = 255) {
		int iWidth = src.cols;
		int iHeight = src.rows;

		std::vector<std::vector<uchar>> result;
		result.resize(iHeight);
		std::for_each(result.begin(), result.end(), [&](std::vector<uchar>& width) {
			width.resize(iWidth);
		});

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				auto current = src.at<uchar>(i, j);
				auto left_top = src.at<uchar>(i - 1, j - 1);
				auto left = src.at<uchar>(i, j - 1);
				auto left_bottom = src.at<uchar>(i + 1, j - 1);
				auto top = src.at<uchar>(i - 1, j);

				auto right_top = src.at<uchar>(i - 1, j + 1);
				auto right = src.at<uchar>(i, j + 1);
				auto right_bottom = src.at<uchar>(i + 1, j + 1);
				auto bottom = src.at<uchar>(i + 1, j);

				if (current == weak) {
					if (left >= strong || left_top >= strong || left_bottom >= strong || top >= strong
						|| right >= strong || right_bottom >= strong || right_top >= strong || bottom >= strong) {
						result[i][j] = strong;
					}
					else {
						result[i][j] = 0;
					}
				}
				else {
					result[i][j] = current;
				}
				
			}
		}

		for (size_t i = 1; i < iHeight - 1; i++)
		{
			for (size_t j = 1; j < iWidth - 1; j++)
			{
				src.at<uchar>(i, j) = result[i][j];
			}
		}
	}

};

//int main() {
//
//
//	src = cv::imread(app::SrcPath8);
//	Mat result_image = src.clone();
//	int iWidth = src.cols;
//	int iHeight = src.rows;
//	cvtColor(src, src_gray, COLOR_BGR2GRAY);
//	cvtColor(result_image, result_image, COLOR_BGR2GRAY);
//	//blur(src_gray, src_gray, Size(3, 3));
//	
//
//	//createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
//	std::vector<std::vector<Point>> contours =  thresh_callback(0, 0);
//	app::Rect4 result = GetCentralRect(contours, iWidth, iHeight);
//
//	app::DrawRectContour(result_image, result);
//
//	imwrite(app::SrcPathOut8, result_image);
//	cv::namedWindow("image", cv::WINDOW_NORMAL);
//	cv::imshow("image", result_image);
//	cv::waitKey(0);
//
//	return 0;
//}

////V3
#if 0
int main() {


	src = cv::imread(app::SrcPath8);
	Mat result_image = src.clone();
	int iWidth = src.cols;
	int iHeight = src.rows;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	
	cv::GaussianBlur(src_gray, src_gray, cv::Size(5, 5), 0, 0);
	canny::GetGradImage(src_gray);
	//canny::double_threshold(src_gray, 150, 60);
	

	//imwrite(app::SrcPathOut7, src_gray);
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", src_gray);
	cv::waitKey(0);

	return 0;
}
#endif

std::vector<Rect> GetRects(const std::vector<std::vector<Point>>& contours) 
{
	std::vector<Rect> result;
	std::for_each(contours.begin(), contours.end(), [&](const std::vector<Point>& points) {
		int maxX = points.begin()->x;
		int minX = points.begin()->x;
		int maxY = points.begin()->y;
		int minY = points.begin()->y;
		std::for_each(points.begin() + 1, points.end(), [&](const Point& point) {
			int x = point.x;
			int y = point.y;
			if (x > maxX) {
				maxX = x;
			}
			if (x < minX) {
				minX = x;
			}

			if (y > maxY) {
				maxY = y;
			}
			if (y < minY) {
				minY = x;
			}
		});
		Point p1{minX, minY};
		Point p2{ maxX, maxY };
		result.push_back(Rect(p2, p1));
	});
	return result;
}

enum class TDir
{
	RIGHT,
	RIGHT_REAR,
	FRONT_RIGHT,
	FRONT,
	FRONT_LEFT,
	LEFT,
	LEFT_REAR,
	REAR
};

struct Pix
{
	int x;
	int y;
	int value;

	bool operator==(const Pix& other) const {
		return other.x == this->x && other.y == this->y;
	}

};

struct Pt
{
	int x;
	int y;
};

struct Tracer
{
	Pix pix;
	TDir _enDirection;

	bool operator==(const Tracer& other) const {
		return pix == other.pix && _enDirection == other._enDirection;
	}

	void Move(bool left = true) {
		switch (_enDirection)
		{
		case TDir::RIGHT:
			if (left) {
				--pix.y;
				_enDirection = TDir::FRONT;
			}
			else {
				++pix.y;
				_enDirection = TDir::REAR;
			}
			break;
		case TDir::LEFT:
			if (left) {
				++pix.y;
				_enDirection = TDir::REAR;
			}
			else {
				--pix.y;
				_enDirection = TDir::FRONT;
			}
			break;
		case TDir::FRONT:
			if (left) {
				--pix.x;
				_enDirection = TDir::LEFT;
			}
			else {
				++pix.x;
				_enDirection = TDir::RIGHT;
			}
			break;
		case TDir::REAR:
			if (left) {
				++pix.x;
				_enDirection = TDir::RIGHT;
			}
			else {
				--pix.x;
				_enDirection = TDir::LEFT;
			}
			break;
		default:
			break;
		}
	}

	int GetX() const { return pix.x; }
	int GetY() const { return pix.y; }
	void SetValue(int value) { pix.value = value; }

	void ChnageDirectionRight() {
		switch (_enDirection)
		{
		case TDir::RIGHT:
			_enDirection = TDir::REAR;
			break;
		case TDir::FRONT:
			_enDirection = TDir::RIGHT;
			break;
		case TDir::REAR:
			_enDirection = TDir::LEFT;
			break;
		case TDir::LEFT:
			_enDirection = TDir::FRONT;
			break;
		default:
			break;
		}
	}

	void ChnageDirectionLeft() {
		switch (_enDirection)
		{
		case TDir::RIGHT:
			_enDirection = TDir::FRONT;
			break;
		case TDir::FRONT:
			_enDirection = TDir::LEFT;
			break;
		case TDir::REAR:
			_enDirection = TDir::RIGHT;
			break;
		case TDir::LEFT:
			_enDirection = TDir::REAR;
			break;
		default:
			break;
		}
	}

	std::tuple<Pt, Pt, Pt> GetCoords() {

		Pt p_left, p_front, p_right;
		switch (_enDirection)
		{
		case TDir::FRONT:
			p_left.x = GetX() - 1;
			p_left.y = GetY() - 1;
			p_front.x = GetX();
			p_front.y = GetY() - 1;
			p_right.x = GetX() + 1;
			p_right.y = GetY() - 1;
			break;
		case TDir::LEFT:
			p_left.x = GetX() - 1;
			p_left.y = GetY() + 1;
			p_front.x = GetX() - 1;
			p_front.y = GetY();
			p_right.x = GetX() - 1;
			p_right.y = GetY() - 1;
			break;
		case TDir::REAR:
			p_left.x = GetX() + 1;
			p_left.y = GetY() + 1;
			p_front.x = GetX();
			p_front.y = GetY() + 1;
			p_right.x = GetX() - 1;
			p_right.y = GetY() + 1;
			break;
		case TDir::RIGHT:
			p_left.x = GetX() + 1;
			p_left.y = GetY() - 1;
			p_front.x = GetX() + 1;
			p_front.y = GetY();
			p_right.x = GetX() + 1;
			p_right.y = GetY() + 1;
			break;
		default:
			break;
		}
		return std::make_tuple(p_left, p_front, p_right);
	}

	void SetPosition(int x, int y, TDir dir) {
		_enDirection = dir;
		pix.x = x;
		pix.y = y;
	}

};


void SBF(cv::Mat src) {

	int iWidth = src.cols;
	int iHeight = src.rows;

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidth; x++)
		{
			auto pixel = src.at<uchar>(y, x);
			if (pixel == 0) {
				Tracer tracer{ {x, y, pixel}, TDir::RIGHT };
				Tracer start_dir = tracer;
				std::vector<Pix> pixels;
				do
				{
					pixels.push_back({tracer.GetX(), tracer.GetY(), 0});
					if (pixel == 0) {
						tracer.Move();
						pixel = src.at<uchar>(tracer.GetY(), tracer.GetX());
						tracer.SetValue(pixel);
					}
					else {
						tracer.Move(false);
						pixel = src.at<uchar>(tracer.GetY(), tracer.GetX());
						tracer.SetValue(pixel);
					}
				} while (!(tracer == start_dir));

				std::for_each(pixels.begin(), pixels.end(), [&](const Pix& p) {
					src.at<uchar>(p.y, p.x) = 80;
				});

				return;
			}
		}
	}

}

void TBA(cv::Mat src) {

	int iWidth = src.cols;
	int iHeight = src.rows;


	const int iSize = 6;
	int colors[iSize] = {50,80,110,140,170, 200};
	int counter = 0;


	for (int y = 1; y < iHeight-1; y++)
	{
		for (int x = 1; x < iWidth-1; x++)
		{
			auto pixel = src.at<uchar>(y, x);
			if (pixel == 0) {
				Tracer tracer{ {x, y, pixel}, TDir::RIGHT };
				Tracer start_dir = tracer;
				std::vector<Pix> pixels;
				pixels.push_back({x, y, 0});
				do
				{
					auto next = tracer.GetCoords();
					auto left = std::get<0>(next);
					auto front = std::get<1>(next);
					auto right = std::get<2>(next);

					bool valid_Left = left.y >= 0 && left.y < iHeight && left.x > 0 && left.x < iWidth;
					bool valid_front = front.y >= 0 && front.y < iHeight && front.x > 0 && front.x < iWidth;
					bool valid_right = right.y >= 0 && right.y < iHeight && right.x > 0 && right.x < iWidth;

					auto pix_left = valid_Left ? src.at<uchar>(left.y, left.x) : 255;
					auto pix_front = valid_front ? src.at<uchar>(front.y, front.x) : 255;
					auto pix_right = valid_right ? src.at<uchar>(right.y, right.x) : 255;

					auto current = src.at<uchar>(tracer.GetY(), tracer.GetX());
					
					if (/*current == 0*/true) {
						auto last = pixels.rbegin();
						Pix tmp{tracer.GetX(), tracer.GetY(), 0};
						if (/*!(tmp == *last)*/true) {
							pixels.push_back(tmp);
						}
					}
					
					

					if (pix_left == 0) {
						tracer.SetPosition(left.x, left.y, tracer._enDirection);
						tracer.ChnageDirectionLeft();
					}
					else if (pix_front == 0) {
						tracer.SetPosition(front.x, front.y, tracer._enDirection);
					}
					else if (pix_right == 0) {
						tracer.SetPosition(right.x, right.y, tracer._enDirection);
					}
					else {
						tracer.ChnageDirectionRight();
					}

				} while (!(tracer == start_dir));

				std::for_each(pixels.begin(), pixels.end(), [&](const Pix& p) {
					bool valida = __validate_size(p.y, iHeight, p.x, iWidth);
					if (valida) {
						src.at<uchar>(p.y, p.x) = colors[counter];
					}
				});
				++counter;
				if (counter == iSize) {
					return;
				}
			}
		}
	}

}


int main() {


	src = cv::imread("fbs2021_cor.tiff");
	Rect crop(2600,2600,180,200);
	Mat src_crop = src(crop);
	Mat result_image = src_crop.clone();

	//std::cout << src.at<ushort>(100,100) << std::endl;

	cvtColor(src_crop, src_gray, COLOR_BGR2GRAY);
	//equalizeHist(src_gray, src_gray);
	cv::GaussianBlur(src_gray, src_gray, cv::Size(5, 5), 2, 2);
	cvtColor(result_image, result_image, COLOR_BGR2GRAY);

	std::vector<std::vector<Point>> contours =  thresh_callback(0, 0);
#if 0
	auto result = GetRects(contours);

	std::for_each(result.begin(), result.end(), [&](const Rect& rect) {
		rectangle(src_gray, rect, Scalar(255, 0, 0), 1, 8, 0);
	});
#endif

	app::EdgeErosion(canny_output);

	//SBF(canny_output);

	TBA(canny_output);

	imwrite(app::SrcPathOut7, src_gray);
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", canny_output);
	cv::waitKey(0);

	return 0;
}