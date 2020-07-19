#if 0
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
	const std::string src_path = "C:\\Users\\Ghevond\\Desktop\\Astro\\AstroDataset\\";
	const std::string res_path = "C:\\Users\\Ghevond\\Desktop\\Astro\\AstroDataset\\Result\\";
}

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

bool IsWhite(cv::Mat& src)
{
	int w = 0;
	int b = 0;
	int iWidht = src.cols;
	int iHeight = src.rows;

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidht; x++)
		{
			int val = src.at<ushort>(y, x);
			if (val > (0xffff * 0.5)) {
				++w;
			}
			else {
				++b;
			}
		}
	}
	return  w > b;
}

int main() {
	std::vector<std::string> folders{
		src_path + C + "\\",
		src_path + cv_star + "\\",
		src_path + M + "\\",
		src_path + Mrk + "\\",
		src_path + PN + "\\",
		src_path + sd + "\\",
		src_path + Sy1 + "\\",
		src_path + QSO + "\\",
		src_path + WD + "\\"
	};

	std::vector<std::string> obj_class{
		C,
		cv_star,
		M,
		Mrk,
		PN,
		sd,
		Sy1,
		QSO,
		WD
	};

	int class_index = 0;
	for (auto folder : folders)
	{
		pen::DirManager dir{ folder, "tiff" };
		auto& images = dir.GetFilesWithExt();

		//int counter = 0;
		for (auto image : images)
		{
			std::string path = image;
			cv::Mat src = cv::imread(path, -1);
			std::string ob_name;
			auto iter_slash = path.rbegin();
			while (*iter_slash != '\\')
			{
				++iter_slash;
			}

			ob_name = std::string(path.rbegin(), iter_slash);
			std::reverse(ob_name.begin(), ob_name.end());

			if (IsWhite(src))
			{
				Invert(src);
			}

			cv::imwrite(res_path + obj_class[class_index] + "~~~" + ob_name, src);
			//if (counter == 50) return 0;
			//++counter;
		}
		++class_index;
	}
#if 0
	pen::DirManager dir{folders[0], "tiff"};
	auto& images = dir.GetFilesWithExt();

	std::string path = images[0];
	cv::Mat src = cv::imread(path, -1);
	std::string ob_name;
	auto iter_slash = path.rbegin();
	while (*iter_slash != '\\')
	{
		++iter_slash;
	}

	ob_name = std::string(path.rbegin(), iter_slash);
	std::reverse(ob_name.begin(), ob_name.end());
	std::cout << ob_name << std::endl;

	if (IsWhite(src))
	{
		Invert(src);
	}

	cv::imwrite(res_path + C + "~~~" + ob_name, src);

	cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	cv::imshow("image_in", src);
	cv::waitKey(0);
#endif
	return 0;
}
#endif