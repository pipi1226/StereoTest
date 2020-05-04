#ifndef COMMON_OPT_H
#define COMMON_OPT_H

#include "IncludeAll.h"
#include "capVideo.h"
#include "matchStereo.h"
#include "GlobalConfigParams.h"

#include <semaphore.h>
#include <math.h>

using namespace std;
using namespace cv;

#define MAX_THRESH 30000
#define MAX_9GRID 9

// add by yyq 2017-0110 [quick mode params]
#define QUICK_6GRID 6
#define EPSILON 1e-6

extern int g_iSignal;


// add by yyq [2017-0221]
extern sem_t sem_cap_id[2];
extern sem_t sem_cap_release_id[2];

extern pthread_mutex_t mutex[MAX_9GRID + 1];
extern pthread_mutex_t avg_mutex[MAX_9GRID];



/*
 * set 9grid boaders
 */
extern void setGrid_lb(int grid_lb[MAX_9GRID][4], int iHei, int iWid);
extern void generateClrDisp(Mat &src, Mat &ptArr, Mat &disp);

/*
 * 线程处理拼接函数
 */
extern void stitching();

/*
 * show choosed pictures
 */
extern void showChoosePicture(string strWndName, const Mat &matShow, int iInx = 0, bool bMutex = false);


/*
 * show whole pictures
 */
extern void showPictures(string strLeftWndname, const Mat &mleft,
                         string strRightWndname, const Mat &mright,
                         string strWndname, Mat &mthird);

/*
 * get pictures vector from files.xml
 */
extern bool readStringList( const string& filename, vector<string>& l );


// vfh method
/*
 * vfh method
 */
extern void getSafeRegion(int iHeight, int iWidth, Mat &ptArr, Mat &binArr);

// --------- thread process function ----------------
/*
 * capture video thread function
 */
extern void *getleftframe(void *arg);
extern void *getrightframe(void *arg);

/*
 * tk1 process 9 subpart pictures
 */
extern void *processSubpic(void *arg);

/*
 * modified by yyq 2017-0110
 * move mouse move handle from dp_match.cpp to this file
 */
// mouse handle
extern Point g_pt;

extern void on_MouseHandle(int event, int x, int y, int flags, void* param);

extern void print3DCordinate(Point pt);



/*
 * add by yyq 2017-0214 [compute core point of best region]
 * 计算合适区域的重心
 * */
extern int computeRegionCorePoint(Mat &mLeft, Mat &mPic, Mat &mGray, Mat &mClr, int &iFront, int &iTail);


#endif
