
// NumberboardDlg.h: 헤더 파일
//

#pragma once





using namespace cv;
using namespace std;
using namespace cv::dnn;


// CNumberboardDlg 대화 상자
class CNumberboardDlg : public CDialogEx
{
// 생성입니다.
public:
	CNumberboardDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NUMBERBOARD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	double img_size, img_center;
	double left_m, right_m;
	Point left_b;
	Point right_b;
	bool left_detect = false, right_detect = false;

	double trap_bottom_width = 0.35;
	double trap_top_width = 0.15;
	double trap_height = 0.3;
	double maskval1 = 100;
	double maskval2 = 30;

public:
	cv::Mat m_srcImage; // 입력 : 파일에서 읽어 들인 버퍼 변수
	cv::Mat m_dstImage; // 출력 : 필터 처리후 최종 결과 버퍼 변수
	cv::Mat m_dstImage2;
	Mat img_frame;
	Mat img_edges;
	CImage m_bmpBitmap;
	CStatic m_picture;
	BITMAPINFO* m_pBitmapInfo_src; // Bitmap 정보를 담고 있는 구조체
	BITMAPINFO* m_pBitmapInfo_dst; // Bitmap 정보를 담고 있는 구조체
	void DrawImage1(cv::Mat Image);
	void DrawImage2(cv::Mat Image);
	void Blob(Mat srcImage, cv::Mat dstImage);
	Mat filter_colors(Mat img_frame);
	void NoiseRemove2_Prewitt();
	Mat DetectMaskRegion(Mat res);
	vector<Vec4i> houghLines(Mat img_mask);
	vector<vector<Vec4i>>separateLine(Mat img_mask, vector<Vec4i>lines);
	vector<Point> regression(vector<vector<Vec4i>> separated_lines, Mat ing_input);
	string predictDir();
	Mat drawLine(Mat lmg_input, vector<Point> lane, string dir);


	void recognizeText(Mat img, tesseract::TessBaseAPI& tess);

	void LineDetection();
	void NoiseRemove();
	
	void Test();
	vector<Vec4i> CNumberboardDlg::houghLines_Right(Mat img_right);
	vector<Vec4i> CNumberboardDlg::houghLines_Left(Mat img_left);
	Mat DetectMaskRegion1(Mat res);
	Vec4i fitLineRANSAC(const vector<Point>& points, int maxIter, double threshold);
	vector<Point> Leftregression(vector<Vec4i> lines, Mat ing_input);
	vector<Point> Rightregression(vector<Vec4i> lines, Mat ing_input);



	
	struct RGBScale
	{
		int r = 0;
		int g = 0;
		int b = 0;
		int rgb = 0;

		RGBScale(int r, int g, int b) {
			this->r = r;
			this->g = g;
			this->b = b;

			this->rgb = RGB(r, g, b);
		}
	};
	static void _putText(Mat& img_input, String text, Point org, int ori, const char* fontName, int fontWeight, double fontScale, RGBScale textColor, RGBScale bkColor);

	
	CString strPathName;
	VideoCapture video;
	Mat m_stream_img_left;
	Mat m_stream_img_right;
	Mat m_stream_img_YOLO;
	vector<Point> Leftlane;
	vector<Point> Rightlane;
	
	//스레드 변수
	bool m_bThreadStart;
	CWinThread* m_pThread1;
	CWinThread* m_pThread2;
	CWinThread* m_pThread3;
	CWinThread* m_pThread4;
	HANDLE hThread;
	bool m_bRun, m_bStop;
	static UINT RunThread_TEXT(LPVOID pParam);
	static UINT RunThread_LEFT(LPVOID pParam);
	UINT RunThread_LEFT();
	static UINT RunThread_RIGHT(LPVOID pParam);
	UINT RunThread_RIGHT();
	static UINT RunThread_YOLO(LPVOID pParam);

	//구조체
	struct STPARAM
	{
		CNumberboardDlg* m_pDlg = nullptr;
		char* outText;

		STPARAM(CNumberboardDlg* pDlg, char* text)
			:m_pDlg(pDlg), outText(text) {}
	};
	Mat TestdrawLine(Mat lmg_input, string dir);
	Mat LeftdrawLine();
	Mat RightdrawLine();
	Mat left;
	Mat right;
	//void ThreadStart();
	Mat maskedImg;
	Mat yolomask;
	vector<Point> roiPoints;
	
	const float confidenceThreshold = 0.24f;
	string yolo_cfg = "C:/Users/82108/source/repos/Numberboard/yolov3.cfg";
	string yolo_weights = "C:/Users/82108/source/repos/Numberboard/yolov3.weights";
	String classesFile = "C:/Users/82108/source/repos/Numberboard/coco.names";
	void YOLO();
	void processDetections(const vector<Mat>& outs, const Mat& img, const vector<string>& classes, float confThreshold, const string& targetClass);
	float calculateDistance(float realWidth, float focalLength, float perceivedWidth);
	//void Speak(const std::string& text);
	vector<string> classes;
	vector<Mat> outs;
	Net m_net;

	struct detectionResult
	{
		Rect plateRect;
		double confidence;
		int type;
	};
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonLoadImage();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();



	

	afx_msg void OnBnClickedButton3();
};

