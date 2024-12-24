#include "pch.h"
#include "NumberboardDlg.h"

using namespace cv::dnn;
using namespace std;
using namespace cv;



void CNumberboardDlg::YOLO()
{
	img_frame.copyTo(maskedImg, yolomask);
	
	Mat inputBlob = blobFromImage(maskedImg, 1 / 255.F, Size(416, 416), Scalar(), true, false); // blob으로 변환
	m_net.setInput(inputBlob); // YOLO 모델에 inputBlob을 입력으로 설정한다.

	vector<String> outNames = m_net.getUnconnectedOutLayersNames();

	m_net.forward(outs, outNames); 
		
}

void CNumberboardDlg::processDetections(const vector<Mat>& outs, const Mat& img, 
	const vector<string>& classes, float confThreshold = 0.5,
	const string& targetClass = "car")
{
	float focalLength = 800.0;  // 픽셀 단위로 초점 거리
	float realWidth = 0.3;      // 실제 객체 너비 
	string slow = "Slow";
	for (size_t i = 0; i < outs.size(); ++i)  
	{
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) //탐지된 객체에 대해 반복
		{
			Mat scores = outs[i].row(j).colRange(5, outs[i].cols); //클래스 확률을 저장한 부분
			Point classIdPoint;
			double confidence;

			//클래스 확률 중 가장 높은 값을 찾음
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

			if (confidence > confThreshold) 
			{
				//자동차만 필터링
				if (classes[classIdPoint.x] == targetClass)
				{
					//객체의 bounding box 좌표 계산
					int centerX = (int)(data[0] * img.cols);
					int centerY = (int)(data[1] * img.rows);
					int width = (int)(data[2] * img.cols);
					int height = (int)(data[3] * img.rows);
					
					//좌상단 좌표 계산
					int left = centerX - width / 2;
					int top = centerY - height / 2;

					//bounding box 그리기
					{
						rectangle(img, Point(left, top), Point(left + width, top + height),
							Scalar(0, 255, 0), 3);
						String label = format("%.2f", confidence);
						if (!classes.empty())
						{
							CV_Assert(classIdPoint.x < (int)classes.size());
							label = classes[classIdPoint.x] + ":" + label;
						}

						//카메라와 객체의 거리 추정
						float distance = calculateDistance(realWidth, focalLength, width);
						label += format(", %.2f m", distance);

						if (distance < 3.0)
						{

							putText(img, slow, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
						}

						int baseLine;
						Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
						top = max(top, labelSize.height);
						rectangle(img, Point(left, top - round(1.5 * labelSize.height)), Point(left + round(1.5 * labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
						putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
					}
				}
			}
		}
	}
}

float CNumberboardDlg::calculateDistance(float realWidth, float focalLength, float perceivedWidth)
{
	//거리 추정
	return (realWidth * focalLength) / perceivedWidth;
}