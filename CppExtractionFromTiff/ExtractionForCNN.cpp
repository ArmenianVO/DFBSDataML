#include <iostream>
#include <DirManager.h>
#include <opencv2/opencv.hpp>

#define C "C"
#define cv_star "cv"
#define M "M"
#define Mrk "Mrk"
#define PN "PN"
#define sd "sd"
#define Sy1 "Sy1"
#define QSO "QSO"
#define WD "WD"

namespace
{
	//const std::string src_path = "E:\\Tif\\fbs1777_cor.tif";
	const std::string res_path = "C:\\Users\\Ghevond\\Desktop\\Specs5\\";
	const std::string fits_path = "E:\\10ppp\\";
}

cv::RNG rng(12345);

void Invert(cv::Mat& src) {
	int iWidht = src.cols;
	int iHeight = src.rows;

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidht; x++)
		{
			src.at<ushort>(y, x) = 0xffff - src.at<ushort>(y, x);
		}
	}

}

void Task(int begin, int end, const std::vector<std::string>& file_paths)
{
	int max_count = end - begin;
	int cnter = 0;
	for (int i = begin; i < end; ++i)
	{
		const std::string& file_path = file_paths[i];
		auto f = file_path.find("fbs");
		if (f == std::string::npos) continue;
		std::string plate_name = file_path.substr(f, 8);
		cv::Mat src_crop = cv::imread(file_path, cv::IMREAD_GRAYSCALE);
		cv::Mat src_tiff = cv::imread(file_path, -1);
		//cv::Mat src_crop = src.clone();//src(cv::Rect{3000, 3000, 1500, 1500});
		cv::Mat bf_crop;
		cv::Mat canny;

		cv::GaussianBlur(src_crop, src_crop, cv::Size(1, 11), 0.5, 8);
		//cv::bilateralFilter(src_crop, bf_crop, 111, 12, 4, cv::BORDER_DEFAULT);
		//cv::boxFilter(image, image, -1, cv::Size(11, 11));

		//cv::equalizeHist(src_crop, src_crop);

		Canny(src_crop, canny, 25, 80, 3);
		//adaptiveThreshold(src_crop, canny, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 99, 12);
		//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;

		findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);


		cv::Mat drawing = cv::Mat::zeros(canny.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		}

		int w_canny = canny.cols;
		int h_canny = canny.rows;
		std::vector<cv::Rect> rects;
		double avg_height = 0;
		int count = 0;
		const int padding_x = 5;
		const int padding_y_top = 8;
		const int padding_y_bottom = 18;
		for (int i = 0; i < contours.size(); i++)
		{
			auto& points = contours[i];
			//if (points.size() < 10) continue;
			auto point = points[0];
			int minx = point.x;
			int maxx = minx;
			int miny = point.y;
			int maxy = miny;
			for (int j = 1; j < points.size(); j++)
			{
				auto& p = points[j];
				int x = p.x;
				int y = p.y;
				if (x > maxx) maxx = x;
				if (x < minx) minx = x;
				if (y > maxy) maxy = y;
				if (y < miny) miny = y;
			}



			maxx = maxx + padding_x < w_canny ? maxx + padding_x : (w_canny - 1);
			minx = minx - padding_x >= 0 ? minx - padding_x : 0;
			maxy = maxy + padding_y_bottom < h_canny ? maxy + padding_y_bottom : (h_canny - 1);
			miny = miny - padding_y_top >= 0 ? miny - padding_y_top : 0;
			rects.push_back({ minx, miny, (maxx - minx), (maxy - miny) });
			avg_height += maxy - miny;
			++count;
		}

		Invert(src_tiff);

		for (int i = 1; i < rects.size(); i++)
		{
			if (rects[i].height <= 140 && rects[i].width <= 40) {
				cv::Mat res = src_tiff(rects[i]);
				//cv::Mat res_gray;
				//cv::cvtColor(res, res_gray, CV_8U);
				auto xstr = std::to_string(rects[i].x);
				auto ystr = std::to_string(rects[i].y);
				auto wstr = std::to_string(rects[i].width);
				auto hstr = std::to_string(rects[i].height);
				if (rects[i].height <= 5) continue;
				cv::imwrite(res_path + plate_name + std::to_string(i) + "_"
					+ xstr + "_"
					+ ystr + "_"
					+ wstr + "_"
					+ hstr + "_"
					+ ".tiff", res);
			}
		}
		std::cout<<"["<<std::this_thread::get_id()<<"]" << cnter++ << "/" << max_count << '\n';
	}
}

int main() {
	
	pen::DirManager dir(fits_path, "tif");

	const auto& files = dir.GetFilesWithExt();
	int size = files.size();
	int b1 = 0;
	int e1 = size/2;
	int b2 = e1;
	int e2 = size;
	//int b3 = e2;
	//int e3 = 100;
	//int b4 = e3;
	//int e4 = 150;

	std::thread t1{Task, b1, e1, std::ref(files)};
	std::thread t2{ Task, b2, e2, std::ref(files) };
	//std::thread t3{ Task, b3, e3, std::ref(files) };
	//std::thread t4{ Task, b4, e4, std::ref(files) };

	t1.join();
	t2.join();
	//t3.join();
	//t4.join();
#if 0
	cv::Mat src = cv::imread(src_path, cv::IMREAD_GRAYSCALE);
	cv::Mat src_tiff = cv::imread(src_path, -1);
	cv::Mat src_crop = src.clone();//src(cv::Rect{3000, 3000, 1500, 1500});
	cv::Mat bf_crop;
	cv::Mat canny;

	cv::GaussianBlur(src_crop, src_crop, cv::Size(1, 11), 0.5, 8);
	//cv::bilateralFilter(src_crop, bf_crop, 111, 12, 4, cv::BORDER_DEFAULT);
	//cv::boxFilter(image, image, -1, cv::Size(11, 11));

	//cv::equalizeHist(src_crop, src_crop);

	Canny(src_crop, canny, 25, 80, 3);
	//adaptiveThreshold(src_crop, canny, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 99, 12);
	//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	
	cv::Mat drawing = cv::Mat::zeros(canny.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
	}
	
	int w_canny = canny.cols;
	int h_canny = canny.rows;
	std::vector<cv::Rect> rects;
	double avg_height = 0;
	int count = 0;
	const int padding_x = 5;
	const int padding_y_top = 8;
	const int padding_y_bottom = 18;
	for (int i = 0; i < contours.size(); i++)
	{
		auto& points = contours[i];
		//if (points.size() < 10) continue;
		auto point = points[0];
		int minx = point.x;
		int maxx = minx;
		int miny = point.y;
		int maxy = miny;
		for (int j = 1; j < points.size(); j++)
		{
			auto& p = points[j];
			int x = p.x;
			int y = p.y;
			if (x > maxx) maxx = x;
			if (x < minx) minx = x;
			if (y > maxy) maxy = y;
			if (y < miny) miny = y;
		}



		maxx = maxx + padding_x < w_canny ? maxx + padding_x : (w_canny - 1);
		minx = minx - padding_x >= 0 ? minx - padding_x : 0;
		maxy = maxy + padding_y_bottom < h_canny ? maxy + padding_y_bottom : (h_canny - 1);
		miny = miny - padding_y_top >= 0 ? miny - padding_y_top : 0;
		rects.push_back({ minx, miny, (maxx - minx), (maxy - miny) });
		avg_height += maxy - miny;
		++count;
	}

	for (int i = 1; i < rects.size(); i++)
	{
		if (rects[i].height <= 140 && rects[i].width <= 40) {
			cv::Mat res = src_tiff(rects[i]);
			//cv::Mat res_gray;
			//cv::cvtColor(res, res_gray, CV_8U);
			auto xstr = std::to_string(rects[i].x);
			auto ystr = std::to_string(rects[i].y);
			auto wstr = std::to_string(rects[i].width);
			auto hstr = std::to_string(rects[i].height);
			cv::imwrite(res_path + "fbs1777_" + std::to_string(i) +"_"
				+ xstr + "_"
				+ ystr + "_"
				+ wstr + "_"
				+ hstr + "_"
				+".tiff", res);
		}
	}
#endif
	//cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	//cv::imshow("image_in", drawing);

	//cv::namedWindow("image_origin", cv::WINDOW_NORMAL);
	//cv::imshow("image_origin", src_crop);
	//cv::waitKey(0);
	return 0;
}
