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
 #include <stdint.h>
using namespace std;

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/utility.hpp"

//enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3, STEREO_3WAY=4 };
typedef enum {
    STEREO_BM=0,
    STEREO_SGBM=1,
    STEREO_HH=2,
    STEREO_VAR=3,
    STEREO_3WAY=4
} STEREO_METHOD;


typedef enum {
    CALIB_LOAD_CAMERA_PARAMS,
    CALIB_SINGLE_CAMERA_FIRST,
    CALIB_STEREO_CAMERAS_DIRECTLY
} CALIB_ORDER;

typedef enum {
    SHOW_ORIGINAL_FRAME,
    SHOW_EDGE_IMAGE,
    SHOW_EQUAL_HISTOGRAM,
    SHOW_CONVERT_COLOR
} FRAME_PROCESS_METHOD;


// video capture method
typedef enum {
    CAPTURE_VIDEO = 0,
    READ_VIDEO = 1,
    READ_PIC = 2,
    WRITE_VIDEO = 3,
    WRITE_PIC = 4,
    CALIBRATE = 5,
    READ_VIDEO_INTO_PIC = 6
} VIDEO_METHOD;

/*
 * 2017-0214 add enum com type
 * */

typedef enum {
    WHOLE_GRID=0,
    SUB_GRID = 1,
    THREE_MID_GRID,
    CHOOSE_GRID
}COMTYPE;

/*
 * 2017-0313 add enum video type
 * */
typedef enum {
    NORMAL_CAMERA=0,
    ZED_CAMERA = 1,
    GABOO_CAMERA
}VIDEOTYPE;

#endif

