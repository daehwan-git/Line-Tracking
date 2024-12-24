#include "pch.h"
#include "NumberboardDlg.h"



using namespace std;
using namespace cv;

//�����νİ� ����� ������� 
UINT CNumberboardDlg::RunThread_TEXT(LPVOID pParam)
{
	CNumberboardDlg* g_pDlg = (CNumberboardDlg*)pParam;
	if (g_pDlg)
	{
		while (g_pDlg->m_bStop == false)
		{
			if (g_pDlg->m_bRun == true)
			{
				tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
				if (api->Init(NULL, "eng"))
				{
					api->End();
					exit(1);
				}
				else
				{
					Mat input_img = g_pDlg->img_frame;
					api->SetImage((uchar*)input_img.data,
						input_img.cols, input_img.rows,
						input_img.channels(), input_img.step1());
					char* outText = api->GetUTF8Text();
					putText(input_img, outText, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
				}
			}

			Sleep(1);
		}
	}

	return 0;
}


UINT CNumberboardDlg::RunThread_LEFT(LPVOID pParam)
{
	CNumberboardDlg* g_pDlg = (CNumberboardDlg*)pParam;
	if (g_pDlg)
	{
		while (g_pDlg->m_bStop == false)
		{
			if (g_pDlg->m_bRun == true)
			{			//������ �����Ѵ�.
				g_pDlg->RunThread_LEFT();
			}

			Sleep(1);
		}
	}

	return 0;
}

UINT CNumberboardDlg::RunThread_LEFT()
{
	vector<Vec4i> lines;
 	lines = houghLines_Left(m_stream_img_left);

	if (lines.size() > 0)
	{
		Leftlane = Leftregression(lines, m_stream_img_left);
	}

	return 0;
}

UINT CNumberboardDlg::RunThread_RIGHT(LPVOID pParam)
{
	CNumberboardDlg* g_pDlg = (CNumberboardDlg*)pParam;
	if (g_pDlg)
	{
		while (g_pDlg->m_bStop == false)
		{
			if (g_pDlg->m_bRun == true)
			{			//������ �����Ѵ�.
				g_pDlg->RunThread_RIGHT();
			}

			Sleep(1);
		}
	}

	return 0;
}

UINT CNumberboardDlg::RunThread_RIGHT()
{

	vector<Vec4i> lines;
	lines = houghLines_Right(m_stream_img_right);

	if (lines.size() > 0)
	{
		Rightlane = Rightregression(lines, m_stream_img_right);
	}

	return 0;
}

UINT CNumberboardDlg::RunThread_YOLO(LPVOID pParam)
{
	CNumberboardDlg* g_pDlg = (CNumberboardDlg*)pParam;
	if (g_pDlg)
	{
		//Ŭ���� �̸� �ε�
		ifstream ifs(g_pDlg->classesFile.c_str());
		string line;
		while (getline(ifs, line))
		{
			g_pDlg->classes.push_back(line);
		}

		//��Ʈ��ũ �ε�
		g_pDlg->m_net = readNetFromDarknet(g_pDlg->yolo_cfg, g_pDlg->yolo_weights);
		if (g_pDlg->m_net.empty())
		{
			cout << "YOLO�� �ε� ����" << endl;
		}
		else
		{
			cout << "YOLO�� �ε� ����" << endl;
		}
		g_pDlg->m_net.setPreferableBackend(DNN_BACKEND_OPENCV);
		g_pDlg->m_net.setPreferableTarget(DNN_TARGET_CPU);

		
		
		
		while (g_pDlg->m_bStop == false)
		{
			if (g_pDlg->m_bRun == true)
			{
				g_pDlg->YOLO();
			}

			Sleep(1);
		}
	}

	return 0;
}

void CNumberboardDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (m_pThread1 != NULL)
	{
		m_bStop = true;
		m_pThread1 = NULL;
	}

	if (m_pThread2 != NULL)
	{
		m_bStop = true;
		m_pThread2 = NULL;
	}

	if (m_pThread3 != NULL)
	{
		m_bStop = true;
		m_pThread3 = NULL;
	}

	if (m_pThread4 != NULL)
	{
		m_bStop = true;
		m_pThread4 = NULL;
	}
}

Mat CNumberboardDlg::filter_colors(Mat img_frame)
{
	Mat output;
	UMat img_hsv, img_combine;
	UMat white_mask, white_image;
	UMat yellow_mask, yellow_image;
	img_frame.copyTo(output);

	Scalar lower_white = Scalar(200, 200, 200);
	Scalar upper_white = Scalar(255, 255, 255);
	Scalar lower_yellow = Scalar(10, 100, 100);
	Scalar upper_yellow = Scalar(40, 255, 255);

	inRange(output, lower_white, upper_white, white_mask);
	bitwise_and(output, output, white_image, white_mask);

	cvtColor(output, img_hsv, COLOR_BGR2HSV);

	inRange(img_hsv, lower_yellow, upper_yellow, yellow_mask);
	bitwise_and(output, output, yellow_image, yellow_mask);

	addWeighted(white_image, 1.0, yellow_image, 1.0, 0.0, output);
	return output;
}

void CNumberboardDlg::NoiseRemove2_Prewitt()
{
	float maskx[] = { -1,0,1,-1,0,1,-1,0,1 };
	float masky[] = { -1,-1,-1,0,0,0,1,1,1 };
	Mat dst1(3, 3, CV_32FC1, maskx);
	Mat dst2(3, 3, CV_32FC1, masky);
	Mat prewitt1, prewitt2;
	filter2D(img_frame, prewitt1, -1, dst1);
	filter2D(img_frame, prewitt2, -1, dst2);
	convertScaleAbs(prewitt1, dst1);
	convertScaleAbs(prewitt2, dst2);
	addWeighted(dst1, 1.0, dst2, 0.5, 0, m_dstImage);
	m_dstImage.copyTo(dst2);
	Laplacian(m_dstImage, dst1, CV_16S, 3, 1, 0);
	convertScaleAbs(dst1, m_dstImage);
	addWeighted(dst2, 0.5, m_dstImage, 0.5, 0, m_dstImage);
}

Mat CNumberboardDlg::DetectMaskRegion(Mat img_edges)
{
	int width = img_edges.cols;
	int height = img_edges.rows;

	Mat output;
	Mat mask = Mat::zeros(height, width, CV_8UC1);

	cv::Point points[4]{
	cv::Point((width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20),   // ���� �� �𼭸�
	cv::Point(width - (width * (1 - trap_top_width)) / 2 + 30, height - height * trap_height - 20), // ������ �� �𼭸�
	cv::Point(width - (width * (1 - trap_bottom_width)) / 2 + maskval2, height - maskval1), // ������ �Ʒ� �𼭸�
	cv::Point((width * (1 - trap_bottom_width)) / 2 + 30, height - 100)  // ���� �Ʒ� �𼭸�
	};
	
	cv::fillConvexPoly(mask, points, 4, Scalar(255, 0, 0));

	bitwise_and(img_edges, mask, output);

	return output;
}

vector<Vec4i> CNumberboardDlg::houghLines(Mat img_mask)
{	
	
	vector<Vec4i>line;
	HoughLinesP(img_mask, line, 1, CV_PI / 180, 20, 40, 30);
	return line;
}

vector<vector<Vec4i>> CNumberboardDlg::separateLine(Mat img_mask, vector<Vec4i>lines)
{
	//����� ��� ������ȯ �������� ���� ���� �����Ѵ�.
	//���� ����� �뷫���� ��ġ�� ���� �¿�� �з��Ѵ�.
	
	Point ini, fini;
	vector<double> slopes;
	vector<Vec4i> selected_lines, left_lines, right_lines;
	double slope_thresh = 0.3;

	//����� �������� ���⸦ ���
	for (int i = 0; i < lines.size(); i++)
	{
		Vec4i line = lines[i];
		ini = Point(line[0], line[1]);
		fini = Point(line[2], line[3]);

		double slope = (static_cast<double>(fini.y) - static_cast<double>(ini.y))
			/ (static_cast<double>(fini.x) - static_cast<double>(ini.x));

		//���Ⱑ �ʹ� ������ ���� ����
		if (abs(slope) > slope_thresh)
		{
			slopes.push_back(slope);
			selected_lines.push_back(line);
		}
	}
	//������ �¿� ������ �з�
	img_center = static_cast<double>((img_mask.cols / 2));
	for (int i = 0; i < selected_lines.size(); i++)
	{
		ini = Point(selected_lines[i][0], selected_lines[i][1]);
		fini = Point(selected_lines[i][2], selected_lines[i][3]);

		if (slopes[i] > 0 && fini.x > img_center && ini.x > img_center)
		{
			right_lines.push_back(selected_lines[i]);
			right_detect = true;
		}
		else if (slopes[i] < 0 && fini.x < img_center && ini.x < img_center)
		{
			left_lines.push_back(selected_lines[i]);
			left_detect = true;
		}
	}
	
	vector<vector<Vec4i>> output(2);
	output[0] = right_lines;
	output[1] = left_lines;
	return output;
}

vector<Point> CNumberboardDlg::regression(vector<vector<Vec4i>> separatedLines, Mat lmg_input)
{
	//���� ȸ�͸� ���� �¿� ���� ������ ���� ������ ���� ã�´�.

	Point ini, fini;
	Point ini2, fini2;
	Vec4d left_line, right_line;
	vector<Point> left_pts, right_pts;

	if (right_detect)
	{
		for (auto i : separatedLines[0])
		{
			ini = Point(i[0], i[1]);
			fini = Point(i[2], i[3]);

			right_pts.push_back(ini);
			right_pts.push_back(fini);
		}
		
		if (right_pts.size() > 0)
		{
			fitLine(right_pts, right_line, DIST_L2, 0, 0.01, 0.01);

			right_m = right_line[1] / right_line[0]; //������ ������ ����
			right_b = Point(right_line[2], right_line[3]); //������ ������ y����
		}
			
	}

	if (left_detect)
	{
		for (auto j : separatedLines[1])
		{
			ini2 = Point(j[0], j[1]);
			fini2 = Point(j[2], j[3]);

			left_pts.push_back(ini2);
			left_pts.push_back(fini2);
		}
		

		if (left_pts.size() > 0)
		{
			fitLine(left_pts, left_line, DIST_L2, 0, 0.01, 0.01);

			left_m = left_line[1] / left_line[0]; //���� ������ ����
			left_b = Point(left_line[2], left_line[3]); //���� ������ y����
		}
			
	}


	//�¿� �� ������ �� ������ ����Ѵ�.
	//y = m*x + b --> x = (y-b) / m
	int ini_y = lmg_input.rows;
	int fin_y = 340;

	double right_ini_x = ((ini_y - (right_b).y) / right_m) + (right_b).x;
	double right_fin_x = ((fin_y - (right_b).y) / right_m) + (right_b).x;

	double left_ini_x = ((ini_y - (left_b).y) / left_m) + (left_b).x;
	double left_fin_x = ((fin_y - (left_b).y) / left_m) + (left_b).x;

	vector<Point> output(4);
	output[0] = Point(right_ini_x, ini_y);
	output[1] = Point(right_fin_x, fin_y);
	output[2] = Point(left_ini_x, ini_y);
	output[3] = Point(left_fin_x, fin_y);

	return output;
}

string CNumberboardDlg::predictDir()
{		
		//�� ������ �����ϴ� ����(������� ��)�� �߽����κ��� 
		//���ʿ� �ִ��� �����ʿ� �ִ����� ��������� �����Ѵ�. 
		
		string output = "";
		double thres_vp = 10;
		double center = static_cast<double>((img_frame.cols / 2));
		//�� ���� �����ϴ� ���� ���
		double vx = static_cast<double>(((right_m * (right_b).x) - (left_m * (left_b).x) - (right_b).y + (left_b).y) / (right_m - left_m));

		if (vx < center - thres_vp)
			output = "Left Turn";	
		else if (vx > center + thres_vp)
			output = "Right Turn";
		else if (vx >= (center - thres_vp) && vx <= (center + thres_vp))
			output = "Straight";

		output += ", Right Slope : " + to_string(right_m);
		output += ", Left Slope : " + to_string(left_m);

		return output;
}

Mat CNumberboardDlg::drawLine(Mat lmg_input, vector<Point> lane, string dir)
{
	vector<Point> poly_points;
	Mat output_img;

	lmg_input.copyTo(output_img);
	poly_points.push_back(lane[2]);
	poly_points.push_back(lane[0]);
	poly_points.push_back(lane[1]);
	poly_points.push_back(lane[3]);

	fillConvexPoly(output_img, poly_points, Scalar(0, 230, 0, 80), LINE_AA, 0);
	addWeighted(output_img, 0.3, lmg_input, 0.7, 0, lmg_input);

	line(lmg_input, lane[0], lane[1], Scalar(0, 255, 0, 80), 5, LINE_AA);
	line(lmg_input, lane[2], lane[3], Scalar(0, 255, 0, 80), 5, LINE_AA);
	putText(lmg_input, dir, Point(180, 100), FONT_HERSHEY_PLAIN, 1.4, Scalar(255, 255, 255), 3, LINE_AA);
	return lmg_input;
}

void CNumberboardDlg::recognizeText(Mat img, tesseract::TessBaseAPI& tess) {
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	tess.SetImage(gray.data, gray.cols, gray.rows, 1, gray.cols * gray.elemSize());
	string result = tess.GetUTF8Text();

	putText(img, result, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

	imshow("Text Recognition", img);
}

void CNumberboardDlg::_putText(Mat& img, String text, Point org, int ori, const char*fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor)
{
	int fontSize = (int)(10 * fontScale);
	int width = img.cols;
	int height = fontSize * 3 / 2;

	HDC hdc = CreateCompatibleDC(NULL); //�ؽ�Ʈ �̹����� ����� �δ� ��

	HBRUSH hBrush = CreateSolidBrush(bkColor.rgb);

	//�ؽ�Ʈ ũ�� ����
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	//��Ʈ���� ������ ����
	BITMAPINFOHEADER header;
	ZeroMemory(&header, sizeof(BITMAPINFOHEADER));
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader;
	HBITMAP hbmp = CreateDIBSection(NULL, (LPBITMAPINFO)&bitmapInfo, DIB_RGB_COLORS, NULL, NULL, 0);
	SelectObject(hdc, hbmp);

	FillRect(hdc, &rect, hBrush); //������ ũ�⸸ŭ �����ϰ� ä��

	BITMAP bitmap;
	GetObject(hbmp, sizeof(BITMAP), &bitmap);

	//�ؽ�Ʈ �̹����� ���� �� ����� �� �ִ� ��Ʈ�� �����Ѵ�.
	HFONT hFont = CreateFontA(
		fontSize,
		0,
		0,
		0,
		fontWeight,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		VARIABLE_PITCH | FF_ROMAN,
		fontName);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, textColor.rgb);
	SetBkColor(hdc, bkColor.rgb);

	//����� ���� �ؽ�Ʈ�� ������ ���ϱ�
	SIZE size;
	GetTextExtentPoint32A(hdc, text.c_str(), (int)text.length(), &size);

	TextOutA(hdc, 0, height / 3 * 1, text.c_str(), (int)text.length()); //�̹����� �ؽ�Ʈ�� �׸���.
	int posX = (ori == 0 ? org.x : (ori == 1 ? org.x - (size.cx / 2) : org.x - size.cx));
	int posY = org.y - (size.cy / 2 + 5);

	//��Ʈ�� ������ opencv�̹����� �����Ѵ�.
	unsigned char* _tmp;
	unsigned char* _img;
	for (int y = 0; y < bitmap.bmHeight; y++)
	{
		if (posY + y >= 0 && posY + y < img.rows)
		{
			_img = img.data + (int)(3 * posX + (posY + y) * (((bitmap.bmBitsPixel / 8) * img.cols) & ~3));
			_tmp = (unsigned char*)(bitmap.bmBits) + (int)((bitmap.bmHeight - y - 1) * (((bitmap.bmBitsPixel / 8) * bitmap.bmWidth) & ~3));
			for (int x = 0; x < bitmap.bmWidth; x++)
			{
				if (x + posX >= img.cols)
					break;

				if (_tmp[0] != bkColor.b || _tmp[0] != bkColor.g || _tmp[0] != bkColor.r)
				{
					_img[0] = (unsigned char)_tmp[0];
					_img[1] = (unsigned char)_tmp[1];
					_img[2] = (unsigned char)_tmp[2];
				}
				_img += 3;
				_tmp += 3;
			}
		}
	}
	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hbmp);
	DeleteObject(hdc);
}

void CNumberboardDlg::NoiseRemove()
{

}


void CNumberboardDlg::LineDetection()
{
	Mat  gray, img_mask, img_filter, Gaus, canny;
	vector<Vec4i> lines;
	vector<vector<Vec4i>>separated_lines;
	vector<Point> lane;
	string dir;
	string wor = "Worning!!";
	char* outText = nullptr;
	tesseract::TessBaseAPI* api;

	CString str = _T("ALL files(*.*)|*.*|");
	CFileDialog dlg(TRUE, _T("*.dat"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, this);
	if (dlg.DoModal() == IDOK)
	{

		// ���� ������ ������ ���� �̸� ��������
		CString strPathName = dlg.GetPathName();
		CT2CA pszString(strPathName);
		string strPath(pszString);

		// ������ ������ �о����
		VideoCapture video(strPath);
		if (!video.isOpened())
		{
			MessageBox(L"������ ������ �� �� �����ϴ�.");
			return;
		}

		// ������ ���Ͽ��� 1�徿 �̹��� ��������
		while (1)
		{
			video >> img_frame;
			if (img_frame.empty())
				continue;

			cvtColor(img_frame, gray, COLOR_BGR2GRAY);
			GaussianBlur(gray, Gaus, Size(3, 3), 0);
			Canny(Gaus, img_edges, 150, 250);


			//����� ������� ������ ���͸� �Ѵ�.
			img_filter = filter_colors(img_frame);
			//imshow("src", img_filter);

			//ȸ�� �̹����� ��ȯ ��Ų��.
			cvtColor(img_filter, gray, COLOR_BGR2GRAY);
			//imshow("gray", gray);

			// ������ ����
			
			GaussianBlur(gray, Gaus, Size(3, 3), 0);

			// ������ ���� 1
			Canny(Gaus, img_edges, 150, 250);
			//imshow("img_edges", img_edges);

			//���� ������ MASK ó�� �Ѵ�.
			img_mask = DetectMaskRegion(img_edges);
			//imshow("img_mask", img_mask);


			//������ �����Ѵ�.
			lines = houghLines(img_mask);

			if (lines.size() > 0)
			{
				//������ ���� ���� ������ �� �¿� ���� �з��Ѵ�.
				separated_lines = separateLine(img_mask, lines);

				//���� ������ ���� ã�´�.
				lane = regression(separated_lines, img_frame);

				//�� ���� �����ϴ� ������ ��ġ�� ���� ��������� �����Ѵ�.
				dir = predictDir();

				//���� ������ ����Ѵ�.
				img_frame = drawLine(img_frame, lane, dir);

				//���� �ν� �� ����� ����Ѵ�.
				//m_pThread = AfxBeginThread(RunThread, new STPARAM(this, outText));
					
			}
			else
			{
				//���� ������ ���� ���(������ �νĵ��� ���� ���) ��� �˸��� ����.
				putText(img_frame, wor, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
			}

			imshow("result", img_frame);
			if (waitKey(30) == 27)
				break;

		}
	}
	//api->End();
}




void CNumberboardDlg::Test()
{

	// ������ ���Ͽ��� �̹��� 1�徿 �����´�. ( ������� )
	// ������ �̹����� ��� filter ó���� �Ѵ�.
	// �� �̹����� LEFT / RIGHT / TEXT �����忡 �����ش�.
	// �� ������� �ڱ� ���� ������ �Ѵ�.
	// ���� ����� 1�� �̹����� ǥ�� �Ѵ�. ( 1���� )

	m_bRun = false;
	m_bStop = false;
	m_pThread1 = AfxBeginThread(RunThread_TEXT, this);
	m_pThread2 = AfxBeginThread(RunThread_LEFT, this);
	m_pThread3 = AfxBeginThread(RunThread_RIGHT, this);
	m_pThread4 = AfxBeginThread(RunThread_YOLO, this);
	Sleep(1000);

	CString str = _T("ALL files(*.*)|*.*|");
	CFileDialog dlg(TRUE, _T("*.dat"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, this);
	if (dlg.DoModal() == IDOK)
	{
		Mat  gray, img_mask, img_filter, Gaus, canny;
		vector<Vec4f> lines;
		vector<vector<Vec4i>>separated_lines;
		vector<Point> lane;
		string dir;
		string wor = "Worning!!";
		char* outText = nullptr;

		// ���� ������ ������ ���� �̸� ��������
		CString strPathName = dlg.GetPathName();
		CT2CA pszString(strPathName);
		string strPath(pszString);

		// ������ ������ �о����
		VideoCapture video(strPath);
		if (!video.isOpened())
		{
			MessageBox(L"������ ������ �� �� �����ϴ�.");
			return;
		}
		while (1)	
		{
			video >> img_frame;
			if (img_frame.empty())
				continue;
			
			//����� ������� �����Ѵ�.
			img_filter = filter_colors(img_frame);

			//ȸ�� �̹����� ��ȯ ��Ų��.
			cvtColor(img_filter, gray, COLOR_BGR2GRAY);
			
			// ������ ����
			GaussianBlur(gray, Gaus, Size(3, 3), 0);

			// ������ ���� 1
			// ��� : ������ ������ ��� �����
			Canny(Gaus, img_edges, 150, 250);

			//���� ������ MASK ó�� �Ѵ�.
			DetectMaskRegion1(img_edges);
			
			m_bRun = true;
			if (m_bRun)
			{
				// YOLO �׸� �׸���
				float confThreshold = 0.5;

				processDetections(outs, img_frame, classes, confThreshold, "car");
				
				// ���� �׸���
				if (Leftlane.size() != 0 && Rightlane.size() != 0)
				{
					dir = predictDir();
					
					TestdrawLine(img_frame, dir);
				}
			}
			
			imshow("result", img_frame);

			if (waitKey(30) == 27)
				break;  // ESC Ű�� ������ ����	
		}
	}
}

