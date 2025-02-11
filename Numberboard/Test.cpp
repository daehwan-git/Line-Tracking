#include "pch.h"
#include "NumberboardDlg.h"

using namespace std;
using namespace cv;

Mat CNumberboardDlg::DetectMaskRegion1(Mat img_edges)
{
	// 앞유리창에 보이는 사다리꼴 마스크 영역
	int width = img_edges.cols;
	int height = img_edges.rows;

	Mat crop_mask_image_result;
	Point points[4]{
	Point((width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20),   // 왼쪽 위 모서리
	Point(width - (width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20), // 오른쪽 위 모서리
	Point(width - (width * (1 - trap_bottom_width)) / 2 + maskval2, height - maskval1), // 오른쪽 아래 모서리
	Point((width * (1 - trap_bottom_width)) / 2 + 30, height - 100)  // 왼쪽 아래 모서리
	};
	Mat mask = Mat::zeros(height, width, CV_8UC1);
	fillConvexPoly(mask, points, 4, Scalar(255, 0, 0));
	bitwise_and(img_edges, mask, crop_mask_image_result);
	
	//YOLO 관심영역 지정
	roiPoints.push_back(Point((width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20));
	roiPoints.push_back(Point(width - (width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20));
	roiPoints.push_back(Point(width - (width * (1 - trap_bottom_width)) / 2 + maskval2, height - maskval1));
	roiPoints.push_back(Point((width * (1 - trap_bottom_width)) / 2 + 30, height - 100));
	yolomask = Mat::zeros(height, width, CV_8UC1);
	vector<vector<Point>> pts = { roiPoints };
	fillPoly(yolomask, pts, Scalar(255));
	
	
	
	// 왼쪽, 오른쪽 이미지를 나눠서 만들어준다.
	Rect left_roi(0, height / 2, width / 2, height / 2);

	m_stream_img_left = crop_mask_image_result(left_roi).clone();

	Rect right_roi(width / 2, height / 2, width / 2, height / 2);
	m_stream_img_right = crop_mask_image_result(right_roi).clone();
	
	return crop_mask_image_result;
}

vector<Vec4i> CNumberboardDlg::houghLines_Left(Mat img_left)
{
	Vec4i LbestLine;
	vector<Vec4i>left_line;
	HoughLinesP(img_left, left_line, 1, CV_PI / 180, 20, 40, 30);
	left_detect = true;
	return left_line;

	vector<Point> Lpoints;
	for (const auto& line : left_line) {
		Lpoints.push_back(Point(line[0], line[1]));
		Lpoints.push_back(Point(line[2], line[3]));
	}

	if (!Lpoints.empty())
	{
		LbestLine = fitLineRANSAC(Lpoints, 1000, 10);

	}
	else
	{
		// Lpoints가 비어 있을 경우, LbestLine을 초기화
		LbestLine = Vec4i(0, 0, 0, 0);
	}
	

	vector<Vec4i> output(1);
	output[0] = LbestLine;
	return output;
}

vector<Vec4i> CNumberboardDlg::houghLines_Right(Mat img_right)
{
	Vec4i RbestLine;
	vector<Vec4i>right_line;
	HoughLinesP(img_right, right_line, 1, CV_PI / 180, 20, 40, 30);
	right_detect = true;
	return right_line;

	vector<Point> Rpoints;
	for (const auto& line : right_line) {
		Rpoints.push_back(Point(line[0], line[1]));
		Rpoints.push_back(Point(line[2], line[3]));
		
	}

	if (!Rpoints.empty())
	{
		RbestLine = fitLineRANSAC(Rpoints, 1000, 10);
	}
	else
	{
		// Rpoints가 비어 있을 경우, RbestLine을 초기화
		RbestLine = Vec4i(0, 0, 0, 0);
	}
	

	vector<Vec4i> output(1);
	output[0] = RbestLine;
	return output;
}

Vec4i CNumberboardDlg::fitLineRANSAC(const vector<Point>& points, int maxIter, double threshold)
{
	int bestInlierCount = 0;
	Vec4i bestLine;

	if (points.size() < 2)
	{
		return bestLine;
	}


	for (int iter = 0; iter < maxIter; ++iter) {
		// 랜덤으로 두 개의 포인트 선택
		Point p1 = points[rand() % points.size()];
		Point p2 = points[rand() % points.size()];

		// 두 점이 동일하면 건너뛰기
		while (p1 == p2)
		{
			p2 = points[rand() % points.size()];
		}

		// 직선 방정식 ax + by + c = 0을 위한 계수 계산
		float a = p2.y - p1.y;
		float b = p1.x - p2.x;
		float c = p2.x * p1.y - p1.x * p2.y;

		// 직선과의 거리 계산
		int inlierCount = 0;
		for (const auto& pt : points) {
			float distance = fabs(a * pt.x + b * pt.y + c) / sqrt(a * a + b * b);
			if (distance < threshold)
			{
				++inlierCount;
			}
		}

		// 현재 직선이 더 많은 인라이어를 포함하면 갱신
		if (inlierCount > bestInlierCount) {
			bestInlierCount = inlierCount;
			bestLine = Vec4i(p1.x, p1.y, p2.x, p2.y);
		}
	}

	return bestLine;
}

vector<Point> CNumberboardDlg::Leftregression(vector<Vec4i> lines, Mat lmg_input)
{
	//선형 회귀를 통해 좌우 차선 각각의 가장 적합한 선을 찾는다.

	Point ini, fini;
	Vec4d left_line;
	vector<Point> left_pts;

	if (left_detect)
	{
		for (auto i : lines)
		{
			float OffsetX = 0;// abs(lmg_input.cols - img_frame.cols);
			float OffsetY = abs(lmg_input.rows - img_frame.rows);

			ini = Point(i[0] + OffsetX, i[1] + OffsetY);
			fini = Point(i[2] + OffsetX, i[3] + OffsetY);

			left_pts.push_back(ini);
			left_pts.push_back(fini);
		}
		if (left_pts.size() > 0)
		{
			fitLine(left_pts, left_line, DIST_L2, 0, 0.1, 0.1);

			left_m = left_line[1] / left_line[0]; //왼쪽 차선의 기울기
			left_b = Point(left_line[2], left_line[3]); //왼쪽 차선의 y절편
		}
	}

	//좌우 선 각각의 두 끝점을 계산한다.
	//y = m*x + b --> x = (y-b) / m
	int ini_y = img_frame.rows;
	int fin_y = lmg_input.rows;

	double left_ini_x = ((ini_y - left_b.y) / left_m) + left_b.x;
	double left_fin_x = ((fin_y - left_b.y) / left_m) + left_b.x;

	vector<Point>output(2);
	output[0] = Point(left_ini_x, ini_y);
	output[1] = Point(left_fin_x, fin_y);

	return left_pts;
}

vector<Point> CNumberboardDlg::Rightregression(vector<Vec4i> lines, Mat lmg_input)
{
	Point ini, fini;
	Vec4d right_line;
	vector<Point> right_pts;

	if (right_detect)
	{
		for (auto i : lines)
		{
			float OffsetX = abs(lmg_input.cols - img_frame.cols);
			float OffsetY = abs(lmg_input.rows - img_frame.rows);

			ini = Point(i[0] + OffsetX, i[1] + OffsetY);
			fini = Point(i[2] + OffsetX, i[3] + OffsetY);

			right_pts.push_back(ini);
			right_pts.push_back(fini);
		}

		if (right_pts.size() > 0)
		{
			fitLine(right_pts, right_line, DIST_L2, 0, 0.1, 0.1);

			right_m = right_line[1] / right_line[0]; //오른쪽 차선의 기울기
			right_b = Point(right_line[2], right_line[3]); //오른쪽 차선의 y절편
		}
	}

	//좌우 선 각각의 두 끝점을 계산한다.
	//y = m*x + b --> x = (y-b) / m
	int ini_y = img_frame.rows;
	int fin_y = lmg_input.rows;

	double right_ini_x = ((ini_y - right_b.y) / right_m) + (right_b).x;
	double right_fin_x = ((fin_y - right_b.y) / right_m) + (right_b).x;

	vector<Point>output(2);
	output[0] = Point(right_ini_x, ini_y);
	output[1] = Point(right_fin_x, fin_y);

	return right_pts;
}

Mat CNumberboardDlg::TestdrawLine(Mat img_input, string dir)
{
	vector<Point> poly_points;
	Mat output_img;
	
	//Mat left_res = LeftdrawLine();

	//Mat right_res = RightdrawLine();

	//hconcat(left_res, right_res, img_input);

	img_input.copyTo(output_img);
	
	poly_points.push_back(Rightlane[0]);
	poly_points.push_back(Leftlane[0]);
	poly_points.push_back(Leftlane[1]);
	poly_points.push_back(Rightlane[1]);

	fillConvexPoly(output_img, poly_points, Scalar(0, 230, 0, 80), LINE_AA, 0);
	addWeighted(output_img, 0.3, img_input, 0.7, 0, img_input);
	

	line(img_input, Leftlane[0], Leftlane[1], Scalar(0, 255, 0, 80), 5, LINE_AA);
	line(img_input, Rightlane[0], Rightlane[1], Scalar(0, 255, 0, 80), 5, LINE_AA);

	putText(img_input, dir, Point(180, 100), FONT_HERSHEY_PLAIN, 1.4, Scalar(255, 255, 255), 3, LINE_AA);
	return img_input;
}

Mat CNumberboardDlg::LeftdrawLine()
{
	Mat output_img;
	left.copyTo(output_img);
	
	line(output_img, Leftlane[0], Leftlane[1], Scalar(0, 255, 0, 80), 5, LINE_AA);

	return output_img;
}

Mat CNumberboardDlg::RightdrawLine()
{
	Mat output_img;

	right.copyTo(output_img);
	line(output_img, Rightlane[0], Rightlane[1], Scalar(0, 255, 0, 80), 5, LINE_AA);
	
	return output_img;

}