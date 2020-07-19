#if 0
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>
#include <DirManager.h>
#include <Table.h>
#include <CSVReader.h>

const std::string CSV_PATH = "C:\\Users\\Ghevond\\Desktop\\Master\\DFBS_EXTRACTED.csv";
const std::string FITS_PATH = "E:\\Tif\\";
const std::string OUT_PATH_C = "E:\\AstroDataset\\C\\";
const std::string OUT_PATH_cv = "E:\\AstroDataset\\cv\\";
const std::string OUT_PATH_M = "E:\\AstroDataset\\M\\";
const std::string OUT_PATH_Mrk = "E:\\AstroDataset\\Mrk\\";
const std::string OUT_PATH_PN = "E:\\AstroDataset\\PN\\";
const std::string OUT_PATH_QSO = "E:\\AstroDataset\\QSO\\";
const std::string OUT_PATH_sd = "E:\\AstroDataset\\sd\\";
const std::string OUT_PATH_Sy1 = "E:\\AstroDataset\\Sy1\\";
const std::string OUT_PATH_WD = "E:\\AstroDataset\\WD\\";

bool White(cv::Mat& src) 
{
	int w = 0;
	int b = 0;
	int iWidht = src.cols;
	int iHeight = src.rows;

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidht; x++)
		{
			int val;
			try
			{
				val = src.at<ushort>(y, x);
			}
			catch (const std::exception&)
			{
				return true;
			}
			
			if (val > (0xffff / 2)) {
				++w;
			}
			else {
				++b;
			}
		}
	}
	return  w > b;
}

void Invert(cv::Mat& src) {
	int iWidht = src.cols;
	int iHeight = src.rows;

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidht; x++)
		{
			src.at<uchar>(y, x) = 0xff - src.at<uchar>(y, x);
		}
	}

}

void CreateCropImage(cv::Mat& src, cv::Mat& dst, const std::pair<int, int>& coord, bool pn = false)
{
	int iHeight = src.rows;
	int iWidth = src.cols;
	int x = coord.first;
	int y = iHeight - coord.second;
	const int size_x = pn ? 20 : 10;
	const int size_y = 70;
	int dx = size_x << 1;
	int dy = size_y << 1;
	int coordX = x - size_x;
	int coordY = y - 22;

	if (x - size_x < 0) {
		coordX = 0;
		dx = x << 1;
	}
	else if(x + size_x >= iWidth){
		int del = (iWidth - x) - 1;
		coordX = x - del;
		dx = del << 1;
	}

	if (y - size_x < 0) {
		coordY = 0;
	}
	
	if (y + dy >= iHeight) {
		dy = (iHeight - y) - 1;
	}

	//int val = src.at<ushort>(y, x);

	cv::Rect rect{ coordX, coordY, dx, dy };

	cv::Mat croppedRef(src, rect);
	//src.at<ushort>(y, x) = 0;
	croppedRef.copyTo(dst);
	//cv::normalize(dst, dst, 0, 0xffff);
	//cv::equalizeHist(dst, dst);
}

int main() {

	
	std::cout << "Hello World" << std::endl;
	pen::DirManager dir{FITS_PATH, "tif"};
	//fbs0005_cor
	pen::Table* table = pen::CSVReader::ReadCSV(CSV_PATH);
#if 1

	//for (int i = 1; i < table->GetDataCount(); i++)
	//{
	//	int dx = table->GetValue<int>(i, "dx");
	//	int dy = table->GetValue<int>(i, "dy");
	//	std::string cl = table->GetValue<std::string>(i, "Cl");
	//	if (dx == 0 && dy == 0) continue;
	//	std::string name = table->GetValue<std::string>(i, "Name");
	//	std::string plate = table->GetValue<std::string>(i, "plate");
	//	plate = plate.substr(0, plate.length() - 9) + ".tif";
	//	//if (!(plate == "fbs0335_cor.tif" || plate == "fbs1405_cor.tif" || plate == "fbs1069_cor.tif")) continue;
	//	
	//	if (dir.CheckFilePathInDir(plate)) {
	//		plate = FITS_PATH + plate;
	//		cv::Mat src = cv::imread(plate, -1);
	//		
	//		cv::Mat src_crop;
	//		CreateCropImage(src, src_crop, { dx, dy }, cl == "PN");
	//		cv::imwrite(OUT_PATH + name +".tiff", src_crop);
	//		/*cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	//		cv::imshow("image_in", src_crop);
	//		cv::waitKey(0);*/
	//	}
	//}

	auto kernel = [&](const int begin, const int end) {
		//static double count = 0;
		try
		{
			for (int i = end - 1; i >= begin; --i)
			{
				int dx = table->GetValue<int>(i, "dx");
				int dy = table->GetValue<int>(i, "dy");
				std::string cl = table->GetValue<std::string>(i, "Cl");
				if (dx == 0 && dy == 0) continue;
				std::string name = table->GetValue<std::string>(i, "Name");
				std::string plate = table->GetValue<std::string>(i, "plate");
				plate = plate.substr(0, plate.length() - 9) + ".tif";
				//if (!(plate == "fbs0335_cor.tif" || plate == "fbs1405_cor.tif" || plate == "fbs1069_cor.tif")) continue;
				if (dir.CheckFilePathInDir(plate)) {
					plate = FITS_PATH + plate;
					cv::Mat src = cv::imread(plate, -1);

					cv::Mat src_crop;
					CreateCropImage(src, src_crop, { dx, dy }, cl == "PN");
					if (cl == "C") {
						cv::imwrite(OUT_PATH_C + name + ".tiff", src_crop);
					}
					else if (cl == "cv") {
						cv::imwrite(OUT_PATH_cv + name + ".tiff", src_crop);
					}
					else if (cl == "M") {
						cv::imwrite(OUT_PATH_M + name + ".tiff", src_crop);
					}
					else if (cl == "Mrk") {
						cv::imwrite(OUT_PATH_Mrk + name + ".tiff", src_crop);
					}
					else if (cl == "PN") {
						cv::imwrite(OUT_PATH_PN + name + ".tiff", src_crop);
					}
					else if (cl == "QSO") {
						cv::imwrite(OUT_PATH_QSO + name + ".tiff", src_crop);
					}
					else if (cl == "sd") {
						cv::imwrite(OUT_PATH_sd + name + ".tiff", src_crop);
					}
					else if (cl == "Sy1") {
						cv::imwrite(OUT_PATH_Sy1 + name + ".tiff", src_crop);
					}
					else if (cl == "WD") {
						cv::imwrite(OUT_PATH_WD + name + ".tiff", src_crop);
					}
					else {
						std::cout << "---Wrong---:" << cl << std::endl;
					}
				}
			}
		}
		catch (const std::exception&)
		{
			std::cout << "catch exception" << std::endl;
		}
	};

	kernel(0, table->GetDataCount());
	
#else
	
	cv::Mat src = cv::imread(TEST_PATH, cv::IMREAD_GRAYSCALE);

	//cv::GaussianBlur(src, src, cv::Size{ 19,19 }, 0.2, 12);
	cv::GaussianBlur(src, src, cv::Size{ 5,21 }, 0.1, 21);

	cv::equalizeHist(src, src);
	

	cv::threshold(src, src, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	cv::imshow("image_in", src);
	cv::waitKey(0);
#endif
	delete table;
	return 0;
}
#endif