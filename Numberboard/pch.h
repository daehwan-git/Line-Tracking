// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <atlstr.h>
#include <vector>
#include <cmath> 
#include<algorithm>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <atlimage.h>
#include "afxdialogex.h"
#include <windows.h>
#include <fstream>
#include <algorithm>
#include <afxwin.h>
#include <sapi.h>	// TTS를 위한 헤더
#include <sphelper.h> // TTS 도우미 헤더

#endif //PCH_H

#pragma comment(linker,"/entry:wWinMainCRTStartup /subsystem:console")