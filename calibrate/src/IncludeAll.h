#pragma once
#ifndef _INCLUDEALL_H_
#define _INCLUDEALL_H_

// standard
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <unistd.h>
#include <string>
#include <string.h>


// OpenCV includes
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

// OpenCV includes
#include <opencv2/video/video.hpp>
#include <opencv2/videoio.hpp> // for camera


#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/utility.hpp"

using namespace std;

//enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3, STEREO_3WAY=4 };
typedef enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3, STEREO_3WAY=4} STEREO_METHOD;
typedef enum { CALIB_LOAD_CAMERA_PARAMS, CALIB_SINGLE_CAMERA_FIRST, CALIB_STEREO_CAMERAS_DIRECTLY } CALIB_ORDER;
typedef enum { SHOW_ORIGINAL_FRAME, SHOW_EDGE_IMAGE, SHOW_EQUAL_HISTOGRAM, SHOW_CONVERT_COLOR } FRAME_PROCESS_METHOD;

#endif

