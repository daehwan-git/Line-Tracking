
// NumberboardDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Numberboard.h"
#include "NumberboardDlg.h"
#include "RoadLaneDetection.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNumberboardDlg 대화 상자



CNumberboardDlg::CNumberboardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NUMBERBOARD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNumberboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Picture, m_picture);
}

BEGIN_MESSAGE_MAP(CNumberboardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CNumberboardDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CNumberboardDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CNumberboardDlg::OnBnClickedButtonLoadImage)
	ON_BN_CLICKED(IDC_BUTTON16, &CNumberboardDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CNumberboardDlg::OnBnClickedButton17)
END_MESSAGE_MAP()


// CNumberboardDlg 메시지 처리기

BOOL CNumberboardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}



void CNumberboardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNumberboardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_bmpBitmap.IsNull() == false)
		{
			// display image area
			CRect rtROI;
			GetDlgItem(IDC_Picture)->GetWindowRect(&rtROI);
			int picture_sizeX = rtROI.Width();
			int picture_sizeY = rtROI.Height();

			// get picture-control dc
			CDC* pDc = m_picture.GetWindowDC();
			CDC memdc;
			memdc.CreateCompatibleDC(pDc);
			memdc.SelectObject(m_bmpBitmap);

			int width = m_bmpBitmap.GetWidth();
			int height = m_bmpBitmap.GetHeight();
			pDc->StretchBlt(0, 0, picture_sizeX, picture_sizeY, &memdc, 0, 0, width, height, SRCCOPY);

			// release dc to picture-control
			m_picture.ReleaseDC(pDc);
		}

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNumberboardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CNumberboardDlg::DrawImage1(cv::Mat Image)
{
	if (m_pBitmapInfo_src != NULL)
	{
		delete m_pBitmapInfo_src;
		m_pBitmapInfo_src = NULL;
	}

	if (Image.channels() == 1)
		m_pBitmapInfo_src = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else
		m_pBitmapInfo_src = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo_src->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo_src->bmiHeader.biPlanes = 1;
	m_pBitmapInfo_src->bmiHeader.biBitCount = Image.channels() * 8;
	m_pBitmapInfo_src->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo_src->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo_src->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo_src->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo_src->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo_src->bmiHeader.biClrImportant = 0;

	if (Image.channels() == 1)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo_src->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo_src->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo_src->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo_src->bmiColors[i].rgbReserved = 0;
		}
	}
	m_pBitmapInfo_src->bmiHeader.biWidth = Image.cols;
	m_pBitmapInfo_src->bmiHeader.biHeight = -Image.rows;



	CClientDC dc(GetDlgItem(IDC_Picture));
	CRect rect;
	GetDlgItem(IDC_Picture)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_srcImage.cols, m_srcImage.rows, m_srcImage.data, m_pBitmapInfo_src, DIB_RGB_COLORS, SRCCOPY);
}

void CNumberboardDlg::DrawImage2(cv::Mat Image)
{
	if (m_pBitmapInfo_dst != NULL)
	{
		delete m_pBitmapInfo_dst;
		m_pBitmapInfo_dst = NULL;
	}

	if (Image.channels() == 1)
		m_pBitmapInfo_dst = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else
		m_pBitmapInfo_dst = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo_dst->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo_dst->bmiHeader.biPlanes = 1;
	m_pBitmapInfo_dst->bmiHeader.biBitCount = Image.channels() * 8;
	m_pBitmapInfo_dst->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo_dst->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo_dst->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo_dst->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo_dst->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo_dst->bmiHeader.biClrImportant = 0;

	if (Image.channels() == 1)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo_dst->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo_dst->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo_dst->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo_dst->bmiColors[i].rgbReserved = 0;
		}
	}
	m_pBitmapInfo_dst->bmiHeader.biWidth = Image.cols;
	m_pBitmapInfo_dst->bmiHeader.biHeight = -Image.rows;

	CClientDC dc(GetDlgItem(IDC_Picture2));
	CRect rect;
	GetDlgItem(IDC_Picture2)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, Image.cols, Image.rows, Image.data, m_pBitmapInfo_dst, DIB_RGB_COLORS, SRCCOPY);
}

void CNumberboardDlg::Blob(cv::Mat srcImage, cv::Mat dstImage)
{	
	// 빨간색 점들을 찾아서 그려라.
	Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create();
	vector<cv::KeyPoint> keypoints;
	detector->detect(srcImage, keypoints);
	cv::drawKeypoints(srcImage, keypoints, dstImage, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


	// 초록색 BOX 를 찾아서 그려라.

	// binary image
	cv::Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(srcImage, img_labels, stats, centroids, 8, CV_32S);
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		int left = stats.at<int>(j, CC_STAT_LEFT);
		int top = stats.at<int>(j, CC_STAT_TOP);
		int width = stats.at<int>(j, CC_STAT_WIDTH);
		int height = stats.at<int>(j, CC_STAT_HEIGHT);

		rectangle(dstImage, Point(left, top), Point(left + width, top + height),
			Scalar(0, 255, 0), 1);
		int baseline;
		cv::Size label_size = getTextSize(to_string(j), FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, 1, &baseline);
		rectangle(dstImage, Point(left, top - label_size.height), Point(left + label_size.width, top), Scalar(0, 255, 0), 1);
		putText(dstImage, to_string(j), Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
	}

	DrawImage2(dstImage);
}


void CNumberboardDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Image (*.BMP, *.PNG, *.JPG) | *.BMP;*.PNG;*.JPG | All Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		CString strPathName = dlg.GetPathName();

		// load image jpg,bmp,png
		m_bmpBitmap.Destroy();
		m_bmpBitmap.Load(strPathName);

		// display image area
		CRect rtROI;
		GetDlgItem(IDC_Picture)->GetWindowRect(&rtROI);
		int picture_sizeX = rtROI.Width();
		int picture_sizeY = rtROI.Height();

		// get picture-control dc
		CDC* pDcPicture = m_picture.GetWindowDC();
		
		// new buffer
		CDC memdc;
		memdc.CreateCompatibleDC(pDcPicture);
		memdc.SelectObject(m_bmpBitmap);

		// set display with new buffer to Picture control
		int width = m_bmpBitmap.GetWidth();
		int height = m_bmpBitmap.GetHeight();
		pDcPicture->StretchBlt(0, 0, picture_sizeX, picture_sizeY, &memdc, 0, 0, width, height, SRCCOPY);

		// release dc to picture-control
		m_picture.ReleaseDC(pDcPicture);
	}

}


void CNumberboardDlg::OnBnClickedButton4()
{
	// 현재 열려 있는 이미지의 edge 를 찾아서 다시 화면에 그린다.

	CString szFilter = _T("Image (*.BMP, *.PNG, *.JPG) | *.BMP;*.PNG;*.JPG | All Files(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		CString strPathName = dlg.GetPathName();
		
		USES_CONVERSION;
		cv::Mat src = cv::imread(T2A(strPathName), 0);

		int thresh = 50;
		// 알고리즘 - rule based
		resize(src, src, Size(500, 500), 0, 0, INTER_LANCZOS4);

		Mat img_copy = src.clone(); // 원본 이미지 복사
		Mat show_img = src.clone(); // 원본 이미지 복사


		Mat harris_c;
		Mat harris_norm;
		Mat cs_abs;

		harris_norm = Mat::zeros(src.size(), CV_32FC1);

		// Harris Corner
		// cornerHarris(입력 이미지(Grayscale), 출력 이미지, 인접 픽셀 크기(Blocksize), Sobel ksize, Harris parameter, 픽셀 보간법)
		cornerHarris(src, harris_c, 2, 3, 0.05);

		// 정규화(Normalizing)
		// normalize(입력 이미지, 출력 이미지, normalize range(low), normalize range(high), 픽셀 보간법) 
		normalize(harris_c, harris_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

		// 미분한 결과에 절대값을 적용하여 8bit의 unsigned int형으로 바꾸어 표현
		convertScaleAbs(harris_norm, cs_abs);

		// Drawing a circle around corners
		for (int j = 0; j < harris_norm.rows; j += 1)
			for (int i = 0; i < harris_norm.cols; i += 1)
				if ((int)harris_norm.at<float>(j, i) >= 200)
					circle(img_copy, Point(i, j), 7, Scalar(255, 0, 255), 1);

		cv::imshow("Original Image", show_img);
		cv::imshow("convertScaleAbs", cs_abs);
		cv::imshow("Harris Corner Detection", img_copy);



		waitKey(0);
	}


}

void CNumberboardDlg::OnBnClickedButtonLoadImage()
{
	// lena.bmp 파일을 1번만 선택
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog fileDlg (TRUE, NULL, NULL, OFN_READONLY, _T("Image file(*.BMP, *.PNG, *.JPG, *.jpeg) | *.BMP;*.PNG;*.JPG;*.JPEG; | All Files(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();

		CT2CA pszString(path);
		std::string strPath(pszString);

		m_srcImage = imread(strPath, IMREAD_GRAYSCALE);
		m_srcImage.copyTo(m_dstImage);
		m_srcImage.copyTo(m_dstImage2);

		DrawImage1(m_srcImage);
	}
	
}



void CNumberboardDlg::OnBnClickedButton16()
{
	cv::bitwise_not(m_srcImage, m_dstImage);
	Blob(m_dstImage, m_dstImage2);
}


void CNumberboardDlg::OnBnClickedButton17()
{
	Test();
}


