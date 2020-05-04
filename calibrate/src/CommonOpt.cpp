#include "CommonOpt.h"


int g_iSignal;

// thread processing...

_st_config_param g_config;

int g_iIndex[MAX_9GRID] = {4, 3, 5, 1, 0, 2, 7, 6, 8};
// int g_iQuickIndex[QUICK_6GRID] = {4, 3, 5, 1, 0, 2};

float g_avg[MAX_9GRID] = {0};
float g_avgdot[MAX_9GRID] = {0};
float g_score[MAX_9GRID] = {0};
float g_sumdist[MAX_9GRID] = {0};
int g_iSumDot[MAX_9GRID] = {0};
int g_iDots[MAX_9GRID] = {0};

int g_iBestRegNo = 4;
float g_fMaxDist = MAX_THRESH / 1000;

Mat g_ll[MAX_9GRID], g_rr[MAX_9GRID];
Mat g_disp[MAX_9GRID+1], g_norm_disp[MAX_9GRID+1];
Mat g_clr_disp[MAX_9GRID];
Mat g_ptArr[MAX_9GRID+1];

matchStereos g_matches[MAX_9GRID+1];

sem_t sem_id[MAX_9GRID];
sem_t sem_main_ids[MAX_9GRID];

// add by yyq [2017-0221]
sem_t sem_cap_id[2];
sem_t sem_cap_release_id[2];

pthread_mutex_t mutex[MAX_9GRID + 1], avg_mutex[MAX_9GRID];

int g_iLeft;
int g_iRight;

int grid_lb[MAX_9GRID][4] = {{0, 0, 105, 79}, {106, 0, 211, 79}, {212, 0, 319, 79},   // 0, 1, 2
                     {0, 80, 105, 159}, {106, 80, 211, 159}, {212, 80, 319, 159},    // 3, 4, 5
                     {0, 160, 105, 239}, {106, 160, 211, 239}, {212, 160, 319, 239}};   // 6, 7, 8


int grid_orig_lb[MAX_9GRID][4] = {{0, 0, 105, 79}, {106, 0, 211, 79}, {212, 0, 319, 79},   // 0, 1, 2
                             {0, 80, 105, 159}, {106, 80, 211, 159}, {212, 80, 319, 159},    // 3, 4, 5
                             {0, 160, 105, 239}, {106, 160, 211, 239}, {212, 160, 319, 239}};   // 6, 7, 8

/*
 * add by yyq [2017-0317]
 * */
// window name
String strWndname = "disparity";
String strLeftWndname = "VideoLeft";
String strRightWndname = "VideoRight";


/*
 * 获取9宫格坐标
 * */
void setGrid_lb(int grid_lb[MAX_9GRID][4], int iHei, int iWid)
{
    int iSubH, iSubW;

    if(g_config.iComType == 1)
    {
        int iW0, iW1, iW2, iH0, iH1, iH2;

        iSubH = (iHei) / 3;
        iSubW = (iWid - g_config.iDTSize*16) / 3;

        iH0 = 0, iW0 = 0;
        iH1 = iSubH, iW1 = iSubW + g_config.iDTSize*16;
        iH2 = iSubH * 2, iW2 = iSubW*2 + g_config.iDTSize*16;

        // 0
        grid_lb[0][0] = iW0;
        grid_lb[0][1] = iH0;
        grid_lb[0][2] = iW1 + g_config.iEnlargeX;
        grid_lb[0][3] = iH1 + g_config.iEnlargeY;

        // 1
        grid_lb[1][0] = iW1 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[1][1] = iH0;
        grid_lb[1][2] = iW2 + g_config.iEnlargeX;
        grid_lb[1][3] = iH1 + g_config.iEnlargeY;

        // 2
        grid_lb[2][0] = iW2 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[2][1] = iH0;
        grid_lb[2][2] = iWid - 1;
        grid_lb[2][3] = iH1 + g_config.iEnlargeY;

        // 3
        grid_lb[3][0] = iW0;
        grid_lb[3][1] = iH1 - g_config.iEnlargeY;
        grid_lb[3][2] = iW1 + g_config.iEnlargeX;
        grid_lb[3][3] = iH2 + g_config.iEnlargeY;

        // 4
        grid_lb[4][0] = iW1 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[4][1] = iH1 - g_config.iEnlargeY;
        grid_lb[4][2] = iW2 + g_config.iEnlargeX;
        grid_lb[4][3] = iH2 + g_config.iEnlargeY;

        // 5
        grid_lb[5][0] = iW2 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[5][1] = iH1 - g_config.iEnlargeY;
        grid_lb[5][2] = iWid - 1;
        grid_lb[5][3] = iH2 + g_config.iEnlargeY;

        // 6
        grid_lb[6][0] = iW0;
        grid_lb[6][1] = iH2 - g_config.iEnlargeY;
        grid_lb[6][2] = iW1 + g_config.iEnlargeX;
        grid_lb[6][3] = iHei - 1;

        // 7
        grid_lb[7][0] = iW1 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[7][1] = iH2 - g_config.iEnlargeY;
        grid_lb[7][2] = iW2 + g_config.iEnlargeX;
        grid_lb[7][3] = iHei - 1;

        // 8
        grid_lb[8][0] = iW2 - g_config.iEnlargeX - g_config.iDTSize * 16;
        grid_lb[8][1] = iH2 - g_config.iEnlargeY;
        grid_lb[8][2] = iWid - 1;
        grid_lb[8][3] = iHei - 1;

        for(int i = 0; i < 9; ++i)
        {
            cout << "rect " << i << endl;
            cout << "lu.x = " << grid_lb[i][0] << ", lu.y = " << grid_lb[i][1] << endl;
            cout << "rd.x = " << grid_lb[i][2] << ", rd.y = " << grid_lb[i][3] << endl;
        }

    }
    else
    {
        iSubH = (iHei) / 3;
        iSubW = (iWid - g_config.iDTSize * 16) / 3;

        // 0
        grid_lb[0][0] = g_config.iDTSize * 16;
        grid_lb[0][1] = 0;
        grid_lb[0][2] = grid_lb[0][0] + iSubW;
        grid_lb[0][3] = grid_lb[0][1] + iSubH;

        // 1
        grid_lb[1][0] = grid_lb[0][2] + 1;
        grid_lb[1][1] = grid_lb[0][1];
        grid_lb[1][2] = grid_lb[1][0] + iSubW;
        grid_lb[1][3] = grid_lb[1][1] + iSubH;

        // 2
        grid_lb[2][0] = grid_lb[1][2] + 1;
        grid_lb[2][1] = grid_lb[0][1];
        grid_lb[2][2] = iWid - 1;
        grid_lb[2][3] = grid_lb[2][1] + iSubH;

        // 3
        grid_lb[3][0] = g_config.iDTSize * 16;
        grid_lb[3][1] = grid_lb[2][3] + 1;
        grid_lb[3][2] = grid_lb[3][0] + iSubW;
        grid_lb[3][3] = grid_lb[3][1] + iSubH;

        // 4
        grid_lb[4][0] = grid_lb[3][2] + 1;
        grid_lb[4][1] = grid_lb[3][1];
        grid_lb[4][2] = grid_lb[4][0] + iSubW;
        grid_lb[4][3] = grid_lb[4][1] + iSubH;

        // 5
        grid_lb[5][0] = grid_lb[4][2] + 1;
        grid_lb[5][1] = grid_lb[3][1];
        grid_lb[5][2] = iWid - 1;
        grid_lb[5][3] = grid_lb[5][1] + iSubH;

        // 6
        grid_lb[6][0] = g_config.iDTSize * 16;
        grid_lb[6][1] = grid_lb[5][3] + 1;
        grid_lb[6][2] = grid_lb[6][0] + iSubW;
        grid_lb[6][3] = iHei - 1;

        // 7
        grid_lb[7][0] = grid_lb[6][2] + 1;
        grid_lb[7][1] = grid_lb[6][1];
        grid_lb[7][2] = grid_lb[7][0] + iSubW;
        grid_lb[7][3] = iHei - 1;

        // 8
        grid_lb[8][0] = grid_lb[7][2] + 1;
        grid_lb[8][1] = grid_lb[6][1];
        grid_lb[8][2] = iWid - 1;
        grid_lb[8][3] = iHei - 1;
    }

}


/*
 * 采集左摄像头图像线程
 * left thread
 * */
void *getleftframe(void *arg)
{
    capVideos* caps = (capVideos*)arg;
    Mat tmp;
    while (1)
    {

        printf("getleftframe() before catch ... \n");
        caps->catchLeftFrame(tmp);
        g_iLeft = 1;

        if(g_config.bWriteVideo)
        {
            caps->writeLeftMat(tmp);
        }
        printf("getleftframe() after catch ... \n");
        printf("sem_cap_id[0] post ... \n");
        sem_post(&sem_cap_id[0]);

        printf("sem_cap_release_id[0] wait ... \n");
        sem_wait(&sem_cap_release_id[0]);
        if(g_iSignal == 1)break;
    }

    sem_post(&sem_cap_id[0]);
    printf("getleftframe() return ... \n");
    return ((void *)0);
}


/*
 * 采集右摄像头图像线程
 * right thread
 * */
void *getrightframe(void *arg)
{
    capVideos* caps = (capVideos*)arg;
    Mat tmp;
    while (1)
    {

        printf("getrightframe() before catch ... \n");
        caps->catchRightFrame(tmp); // get a new frame from camera
        g_iRight = 1;


        if(g_config.bWriteVideo)
        {
            caps->writeRightMat(tmp);
        }

        printf("sem_cap_id[1] post ... \n");
        sem_post(&sem_cap_id[1]);

        printf("sem_cap_release_id[1] wait ... \n");
        sem_wait(&sem_cap_release_id[1]);

        if(g_iSignal == 1)break;

    }

    sem_post(&sem_cap_id[1]);
    printf("getrightframe() return ... \n");
    return ((void *)0);
}


/*
 * 计算边缘
 * */
void computeSubMargin(int i, Point &ptlu, Point &ptrd, int iHei, int iWid)
{
    switch(i)
    {
        case 0:
        {
            ptlu.x = g_config.iDTSize*16;
            ptlu.y = 0;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 1:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = 0;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 2:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = 0;
            ptrd.x = iWid;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 3:
        {
            ptlu.x = g_config.iDTSize*16;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 4:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 5:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;
        }
        case 6:
        {
            ptlu.x = g_config.iDTSize*16;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei;
            break;
        }
        case 7:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei;
            break;
        }
        case 8:
        {
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid;
            ptrd.y = iHei;
            break;
        }
        default:
            ptlu.x = g_config.iDTSize*16 + g_config.iEnlargeX;
            ptlu.y = g_config.iEnlargeY;
            ptrd.x = iWid - g_config.iEnlargeX;
            ptrd.y = iHei - g_config.iEnlargeY;
            break;

    }
}


/*
 * processing subpicture
 * 线程处理9宫格的各个小格
 * */
void *processSubpic(void *arg)
{
    int i = *(int*)arg;

    int subwid = grid_lb[i][2] - grid_lb[i][0];
    int subhei = grid_lb[i][3] - grid_lb[i][1];

    Point ptlu, ptrd;
    computeSubMargin(i, ptlu, ptrd, subhei, subwid);
    cout << "sub rect "<< i << " == (" << ptlu.x << ", " << ptlu.y << "), ("<< ptrd.x << ", "<< ptrd.y  << ")" << endl;

    Point ptLU, ptRD;
    ptLU.x = 32;
    ptLU.y = 0;
    ptRD.x = subwid;
    ptRD.y = subhei;
    Rect recSub(ptLU, ptRD);

    sem_wait(&sem_id[i]);

    while(1)
    {

        if(g_iSignal == 1)
            break;
        // usleep(100);
        Mat ptArr, disps, normdisps;
        Vec3f point;
        //cout << i << "th, thread..." << endl;
        pthread_mutex_lock(&mutex[i]);
        g_matches[i].match_processing(g_ll[i], g_rr[i], g_ptArr[i], g_disp[i], g_norm_disp[i]);
        // cout << g_ptArr[i].at<Vec3f>(0, 50) << endl;

        if(g_config.iAvoidMethod == 0)
        {
            g_avg[i] = 0.0;
            g_avgdot[i] = 0.0;
            g_iSumDot[i] = 0;
            g_sumdist[i] = 0;
            int cntDot = 0;
            for(int y = ptlu.y; y < ptrd.y; ++y)
            // for(int y = ptlu.y + g_config.iEnlargeY; y < ptrd.y - g_config.iEnlargeY; ++y)
            {
                for(int x = ptlu.x; x < ptrd.x; ++x)
                // for(int x = ptlu.x + g_config.iDTSize * 16 + g_config.iEnlargeX; x < ptrd.x - g_config.iEnlargeX; ++x)
                {
                    // cout << "hei = " << subhei << "wid = " << subwid  << "i = " << i << ", y = " << y << ", x = " << x << endl;
                    // cout << "hei = " << subhei << "wid = " << subwid << "rows = " << norm_disp[i].rows << "cols = " << norm_disp[i].cols << endl;
                    Vec3f point = g_ptArr[i].at<Vec3f>(y, x);

                    float fTmp = point[2] * 16.0;

                    float sstmp = 0;

                    if(fTmp - g_fMaxDist < 1e-5 )
                    {
                        if(fTmp - 0.0 < 1.0e-5)
                            sstmp = 0.0;
                        else
                        {
                            cntDot += 1;
                            // sstmp = (fTmp * 1.0) / g_config.iDistThresh;
                            sstmp = (fTmp * 1.0) / 1000.0;
                            if(fTmp - g_config.iDistThresh < 1.0e-5)
                                g_avgdot[i] += 1;
                        }
                    }
                    else
                    {
                        sstmp = 0.0;
                        // avgdot[i] += 1;
                    }
                    g_sumdist[i] += sstmp;

                }

            }

            // sumdist[i] = avg[i];
            g_iSumDot[i] = cntDot; // 有效点个数
            g_iDots[i] = g_avgdot[i]; // 当前块，障碍物点数
            g_avg[i] = g_sumdist[i] / (cntDot); // 平均距离
            g_avgdot[i] = g_avgdot[i] * 1.0 / g_iSumDot[i];

            // cout << "sub function" << i << ", distance =" << avg[i] << \
            // ", dot ratio = " << avgdot[i] << endl;
        }

        pthread_mutex_unlock(&mutex[i]);
        sem_post(&sem_main_ids[i]);
        sem_wait(&sem_id[i]);
    }
    sem_post(&sem_main_ids[i]);
    pthread_exit(NULL);

}

/*
 * 线程处理拼接函数
 */
void stitching()
{
    Mat matSt[MAX_9GRID], matEnd[MAX_9GRID];
    Rect rec;
    Point ptlu, ptrd;

    for(int i = 0; i < MAX_9GRID; ++i)
    {
        pthread_mutex_lock(&mutex[i]);
        computeSubMargin(i, ptlu, ptrd, g_disp[i].rows, g_disp[i].cols);
        rec = Rect(ptlu, ptrd);
        matSt[i] = g_disp[i](rec);
        pthread_mutex_unlock(&mutex[i]);
    }

    hconcat(matSt[0], matSt[1], matEnd[0]);
    hconcat(matEnd[0], matSt[2], matEnd[0]);

    hconcat(matSt[3], matSt[4], matEnd[1]);
    hconcat(matEnd[1], matSt[5], matEnd[1]);

    hconcat(matSt[6], matSt[7], matEnd[2]);
    hconcat(matEnd[2], matSt[8], matEnd[2]);

    vconcat(matEnd[0], matEnd[1], matEnd[3]);
    vconcat(matEnd[3], matEnd[2], matEnd[3]);

    pthread_mutex_lock(&mutex[9]);
    matEnd[3].copyTo(g_disp[9]);
    pthread_mutex_unlock(&mutex[9]);

}


// vfh get binary matrix
void getSafeRegion(int iHeight, int iWidth, Mat &ptArr, Mat &binArr)
{

    for(int i = 0; i < iHeight; ++i)
    {
        for(int j = 0; j < iWidth; ++j)
        {
            binArr.at<char>(i, j) = 0;
            if(j >= g_config.iDTSize * 16)
            {
                Vec3f pt = ptArr.at<Vec3f>(i, j);
                float fdepth = pt[2];
                if (fdepth - g_config.iDistThresh < 1.0e-5)
                {
                    binArr.at<char>(i, j) = 0;
                }
                else
                {
                    binArr.at<char>(i, j) = 254;
                }
            }



        }
    }

    Mat matRe;

    // erode
    int i = 3;
    Mat element = getStructuringElement( 0,Size( 3, 3 ), Point(0, 0 ));
    erode(binArr, matRe, element);

    imwrite("out.jpg", matRe);

}

bool judgeRectBound(int x, int y, int dwWid, int dwHei, int upWid, int upHei)
{
    if(x <= dwWid || x > upWid || y <= dwHei || y > upHei)
        return 0;
    else
        return 1;

}


/*
 * 2017-0111
 * 产生彩色的色差图
 * */
void generateClrDisp(Mat &src, Mat &ptArr, Mat &disp)
{
    // color map
    float max_val = 255.0f;
    float map[8][4] = {{0,0,0,114},{0,0,1,185},{1,0,0,114},{1,0,1,174},
                       {0,1,0,114},{0,1,1,185},{1,1,0,114},{1,1,1,0}};
    float sum = 0;
    for (int i=0; i<8; i++)
        sum += map[i][3];

    float weights[8]; // relative   weights
    float cumsum[8];  // cumulative weights
    cumsum[0] = 0;
    for (int i=0; i<7; i++) {
        weights[i]  = sum/map[i][3];
        cumsum[i+1] = cumsum[i] + map[i][3]/sum;
    }

    int height_ = src.rows;
    int width_ = src.cols;

    int iPiece = 10;
    int dwWid = g_config.iDTSize * 16 + (width_ - g_config.iDTSize * 16) / (iPiece - 4);
    int dwHei = height_ / (iPiece - 4);
    int upWid = g_config.iDTSize * 16 + (iPiece - 1) * (width_ - g_config.iDTSize * 16) / iPiece;
    int upHei = (iPiece - 1) * height_ / iPiece;

    // for all pixels do
    for (int v=0; v<height_; v++) {
        for (int u=0; u<width_; u++) {

            Vec3f pt = ptArr.at<Vec3f>(v, u);
            // get normalized value
            float val = std::min(std::max(src.data[v*width_ + u]/max_val,0.0f),1.0f);

            // find bin
            int i;
            for (i=0; i<7; i++)
                if (val<cumsum[i+1])
                    break;

            // compute red/green/blue values
            float   w = 1.0-(val-cumsum[i])*weights[i];

            uchar r = (uchar)(255.0);
            uchar g = (uchar)(0);
            uchar b = (uchar)(0);
            //rgb内存连续存放
            if(((pt[2] >= g_config.iDistThresh) &&
                    (pt[2] <= (2000 + 1000)/16)) &&
                    (1e-5 - pt[2]) < 1e-5 )
            {
                disp.data[v*width_*3 + 3*u + 0] = b;
                disp.data[v*width_*3 + 3*u + 1] = g;
                disp.data[v*width_*3 + 3*u + 2] = r;
            }
            else
            {
                disp.data[v*width_*3 + 3*u + 0] = src.data[v*width_ + u];
                disp.data[v*width_*3 + 3*u + 1] = src.data[v*width_ + u];
                disp.data[v*width_*3 + 3*u + 2] = src.data[v*width_ + u];
            }

        }
    }
}


/*
 * 2017-0111
 * show pictures
 */
void showChoosePicture(string strWndName, const Mat &matShow, int iInx, bool bMutex)
{
    if(bMutex == false)
    {
        imshow(strWndName, matShow);
    }
    else
    {
        pthread_mutex_lock(&mutex[iInx]);
        imshow(strWndName, matShow);
        pthread_mutex_unlock(&mutex[iInx]);
    }

}


/*
 * 2017-0116
 * 显示图片接口，专门用于拼接后彩色视差图的显示
 * */
void showPictures(string strLeftWndname, const Mat &mleft,
                  string strRightWndname, const Mat &mright,
                  string strWndname, Mat &mthird)
{
    imshow(strLeftWndname, mleft);
    imshow(strRightWndname, mright);
    if(g_config.iComType == 0)
    {
        if(g_config.bShowClrDisp)
        {
            g_clr_disp[0] = Mat(g_norm_disp[0].rows, g_norm_disp[0].cols, CV_8UC3);
            generateClrDisp(g_norm_disp[0], g_ptArr[0], g_clr_disp[0]);
            if(g_config.iChannel == 3)
            {
                hconcat(mthird, g_clr_disp[0], mthird);
                imshow(strWndname, mthird);

            }
            else
            {
                imshow(strWndname, g_clr_disp[0]);
            }
        }
        else
        {
            showChoosePicture(strWndname, g_norm_disp[0], 0, true);
        }

    }
    else
    {
        if(g_config.bShowClrDisp)
        {
            showChoosePicture(strWndname, g_norm_disp[9], 9, true);
        }
        else
        {
            showChoosePicture(strWndname, g_norm_disp[4], 4, true);
        }

    }
}


/*
 * 2017-0111
 * get pictures vector from files.xml
 */
bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);

    if( !fs.isOpened() )
        return false;

    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}

/*
 * modified by yyq 2017-0110
 * move mouse move handle from dp_match.cpp to this file
 */
// mouse handle
Point g_pt;

void on_MouseHandle(int event, int x, int y, int flags, void* param)
{

    // Mat& image = *(cv::Mat*) param;
    switch(event)
    {
        //左键按下消息
        case EVENT_LBUTTONDOWN:
        {
            g_pt = Point(x, y);
            print3DCordinate(g_pt);
        }
            break;
    }

}


void print3DCordinate(Point pt)
{
    if(g_config.iComType == 1)
    {
        Vec3f point;
        if(g_config.bShowClrDisp)
        {
            pthread_mutex_lock(&mutex[9]);
            point = g_ptArr[9].at<Vec3f>(pt.y, pt.x);
            pthread_mutex_unlock(&mutex[9]);

        }
        else
        {
            pthread_mutex_lock(&mutex[g_iIndex[0]]);
            point = g_ptArr[g_iIndex[0]].at<Vec3f>(pt.y, pt.x);
            pthread_mutex_unlock(&mutex[g_iIndex[0]]);

        }
        cout << "-------------------" << endl;
        cout << "[" << point[0]*16 << ", " << point[1]*16 << ", " << point[2]*16 << "]" << endl;
        cout << "z = [" << point[2] << "]" << endl;


    }
    else
    {
        Vec3f point = g_ptArr[0].at<Vec3f>(pt.y, pt.x);
        cout << "x = " << pt.x << ", y = " << pt.y << endl;
        cout << "[" << point[0]*16 << ", " << point[1]*16 << ", " << point[2]*16 << "]" << endl;
        cout << "z = [" << point[2] << "]" << endl;
        cout << "-------------------" << endl;
    }

    cout << "best region no.=" << g_iBestRegNo << endl;
    cout << "-------------------" << endl;

    if(g_config.iAvoidMethod == 0)
    {
        cout << "best region no. = " << g_iBestRegNo << endl;

        if(g_iBestRegNo < 9)
        {
            int iX, iY;
            Vec3f point;
            if (g_config.iComType == 0)
            {
                iX = (grid_lb[g_iBestRegNo][0] + grid_lb[g_iBestRegNo][2]) / 2;
                iY = (grid_lb[g_iBestRegNo][1] + grid_lb[g_iBestRegNo][3]) / 2;
                point = g_ptArr[0].at<Vec3f>(iY, iX);

            }
            else
            {
                iX = (abs(grid_lb[g_iBestRegNo][0] - grid_lb[g_iBestRegNo][2])+1) / 2;
                iY = (abs(grid_lb[g_iBestRegNo][1] - grid_lb[g_iBestRegNo][3])+1) / 2;
                point = g_ptArr[g_iBestRegNo].at<Vec3f>(iY, iX);

            }

            cout << "-------------------" << endl;
            cout << "Best region coordinate..." << endl;
            cout << "x = " << iX << ", y = " << iY << endl;

            cout << "[" << point[0]*16 << ", " << point[1]*16 << ", " << point[2]*16 << "]" << endl;
            cout << "-------------------" << endl;

        }

        /*
        for(int i = 0; i < 9; ++i)
        {
            //pthread_mutex_lock(&mutex[i]);
            cout << "grid[" << i << "] score = " << g_score[i] << endl;
            cout << "grid[" << i << "] avg = " << g_avg[i]<< endl;
            cout << "grid[" << i << "] iSumDot = " << g_iSumDot[i] << endl;
            cout << "grid[" << i << "] avgdot = " << g_avgdot[i] << endl;
            cout << "grid[" << i << "] iDot = " << g_iDots[i] << endl;
            cout << "grid[" << i << "] sumdist = " << g_sumdist[i] << endl;
            cout << "-------------------" << endl;
            //pthread_mutex_unlock(&mutex[i]);
        }
         */

    }

}





/*
 * add by yyq 2017-0214 [compute histogram]
 * 计算直方图
 * */
int computeHistogram(Mat &mSrc, Mat &mDisp, int &iFront, int &iTail)
{
    int iRe = 1;
    int iHist[640];
    memset(iHist, 0, 640*sizeof(int));

    for(int j = g_config.iDTSize*16; j < mSrc.cols; ++j)
    {
        for(int i = 0; i < mSrc.rows; ++i)
        {
            unsigned char byGray = mSrc.at<unsigned char>(i, j);
            if(byGray < 255)
            {
                iHist[j] += 1;
            }
        }
    }

    int iCur_front = g_config.iDTSize*16+1, iCur_tail = g_config.iDTSize*16+1;
    int iGlb_front = g_config.iDTSize*16+1, iGlb_tail = g_config.iDTSize*16+1;
    int iFlag = 0;

    for(int j = g_config.iDTSize*16; j < mSrc.cols; ++j)
    {
        if(iFlag == 0)
        {
            if(iHist[j] < g_config.iPointNum)
            {
                iCur_front = j;
                iFlag = 1;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if(iHist[j] < g_config.iPointNum)
            {
                iCur_tail = j;
            }
            else
            {
                iFlag = 0;
                if((iCur_tail - iCur_front) > (iGlb_tail - iGlb_front))
                {
                    iGlb_front = iCur_front;
                    iGlb_tail = iCur_tail;
                }
            }
        }

    }

    if((iCur_tail - iCur_front) > (iGlb_tail - iGlb_front))
    {
        iGlb_front = iCur_front;
        iGlb_tail = iCur_tail;
    }

    iFront = iGlb_front;
    iTail = iGlb_tail;

    // 计算该区域的平均深度
    float fDepthSum = 0.0;
    int iDotCnt = 0;
    for(int irows = 0; irows < mSrc.rows; ++irows)
    {
        for(int icols = iFront; icols <=iTail; ++icols)
        {
            Vec3f ptTemp;
            ptTemp = mDisp.at<Vec3f>(irows, icols);
            float fTemp = ptTemp[2] * 16.0;
            if(fTemp - MAX_THRESH < EPSILON && 0-fTemp < EPSILON)
            {
                fDepthSum += abs(fTemp);
                iDotCnt += 1;
            }

        }
    }

    float fAvgDist = fDepthSum *1.0 / iDotCnt;
    if(fAvgDist - g_config.iDistThresh < EPSILON)
    {
        iRe = 0;
    }
    //cout << "length = " << iGlb_tail - iGlb_front << " [" << iGlb_front << ", " << iGlb_tail << "]" << endl;
    return iRe;
}


/*
 * add by yyq 2017-0215 [compute core point of best region test]
 * 计算合适区域的重心
 * */
int computeTestRegionCorePoint(Mat &mPic, Mat &mGray, Mat &mClr, int &iFront, int &iTail)
{
    Vec3f ptTmp;
    Mat mBinary;
    mBinary = Mat(mPic.rows, mPic.cols, CV_8UC1, 1);
    // Mat mGray(mPic.rows, mPic.cols, CV_8UC1, 255);

    int iDistThresh = g_config.iMaxDistThresh / 100;
    int iDistSafeThresh = g_config.iDistThresh / 100;
    int iRe = 1;

    Vec3f ptLeft, ptRight;

    for(int i = 0; i < mPic.rows; ++i)
    {
        for(int j = 0; j < mPic.cols; ++j)
        {
            ptTmp = mPic.at<Vec3f>(i, j);
            int iDepth = (ptTmp[2] * 16) / 100;

            if(iDepth > 255 && iDepth < iDistThresh)
                iDepth = 255;

            if(iDepth >= iDistThresh)
                iDepth = 0;
            mGray.at<unsigned char>(i, j) = (unsigned char)(iDepth);
            mBinary.at<unsigned char>(i, j) = (unsigned char)(iDepth) > 0 ? 255 : 0;

            mClr.at<Vec3b>(i, j)[0] = (unsigned char)(iDepth);
            mClr.at<Vec3b>(i, j)[1] = (unsigned char)(iDepth);
            mClr.at<Vec3b>(i, j)[2] = (unsigned char)(iDepth);

        }
    }

    /*
    int iGrayThresh = (int)(g_config.iDistThresh / 100);
    Mat mSrcContour, mContourArr;
    threshold(mGray, mSrcContour, iGrayThresh, 255, CV_THRESH_BINARY_INV);

    // compute region core point
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(mSrcContour, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    cout << "contour size = " << contours.size() << endl;
    for(int i=0;i<contours.size();i++){
        if(hierarchy[i][3]!=-1)
            drawContours(mClr, contours, i, Scalar(255, 0, 0));
    }

    drawContours(mClr, contours, -1, Scalar(255, 0, 0));
    */

    // compute histogram

    iRe = computeHistogram(mBinary, mPic, iFront, iTail);

    if(iRe == 0)
    {
        iFront = -1;
        iTail = -1;
        cout << "computeHistogram cannot go through" << endl;
        return iRe;
    }


    ptLeft = mPic.at<Vec3f>((int)(mPic.rows / 2), iFront);
    ptRight = mPic.at<Vec3f>((int)(mPic.rows / 2), iTail);

    cout << "length = " << iTail - iFront << " [" << iFront << ", " << iTail << "]" << endl;

    float fLength = abs(ptLeft[0]*16 - ptRight[0]*16);
    if(fLength - g_config.iPlaneWid < EPSILON)
    {
        iRe = 0;
        iFront = -1;
        iTail = -1;
        cout << "cannot go through" << endl;
    }
    else
    {
        Point ptCenter;
        ptCenter.x = (iFront + iTail) / 2;
        ptCenter.y = mPic.rows / 2;
        circle(mClr, ptCenter, 25, Scalar(0, 0, 255), 15);
    }

    return iRe;

}


/*
 * add by yyq 2017-0214 [compute core point of best region]
 * 计算合适区域的重心
 * */
int computeRegionCorePoint(Mat &mLeft, Mat &mPic, Mat &mGray, Mat &mClr, int &iFront, int &iTail)
{
    Vec3f ptTmp;
    Mat mBinary;
    mBinary = Mat(mPic.rows, mPic.cols, CV_8UC1, 1);
    // Mat mGray(mPic.rows, mPic.cols, CV_8UC1, 255);

    int iDistThresh = MAX_THRESH / 100;
    int iDistSafeThresh = g_config.iDistThresh / 100;

    for(int i = 0; i < mPic.rows; ++i)
    {
        for(int j = 0; j < mPic.cols; ++j)
        {
            ptTmp = mPic.at<Vec3f>(i, j);
            int iDepth = (ptTmp[2] * 16) / 100;

            if(iDepth > 255 && iDepth < iDistThresh)
                iDepth = 255;

            if(iDepth >= iDistThresh)
                iDepth = 0;
            mGray.at<unsigned char>(i, j) = (unsigned char)(iDepth);
            mBinary.at<unsigned char>(i, j) = (unsigned char)(iDepth) > 0 ? 255 : 0;

            mClr.at<Vec3b>(i, j)[0] = (unsigned char)(iDepth);
            mClr.at<Vec3b>(i, j)[1] = (unsigned char)(iDepth);
            mClr.at<Vec3b>(i, j)[2] = (unsigned char)(iDepth);

        }
    }

    int iRe = 1;

    int iCur_front = g_config.iDTSize*16+1, iCur_tail = g_config.iDTSize*16+1;
    int iGlb_front = g_config.iDTSize*16+1, iGlb_tail = g_config.iDTSize*16+1;
    int iFlag = 0;

    for(int j = g_config.iDTSize*16; j < mPic.cols; ++j)
    {
        ptTmp = mPic.at<Vec3f>((int)(mPic.rows / 2), j);
        float fTemp = abs(ptTmp[2] * 16.0);
        if(iFlag == 0)
        {
            if(fTemp - g_fMaxDist < EPSILON && fTemp - g_config.iDistThresh > EPSILON)
            {
                iCur_front = j;
                iFlag = 1;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if(fTemp - g_fMaxDist < EPSILON && fTemp - g_config.iDistThresh > EPSILON)
            {
                iCur_tail = j;
            }
            else
            {
                iFlag = 0;
                if((iCur_tail - iCur_front) > (iGlb_tail - iGlb_front))
                {
                    iGlb_front = iCur_front;
                    iGlb_tail = iCur_tail;
                }
            }
        }

    }

    if((iCur_tail - iCur_front) > (iGlb_tail - iGlb_front))
    {
        iGlb_front = iCur_front;
        iGlb_tail = iCur_tail;
    }

    iFront = iGlb_front;
    iTail = iGlb_tail;

    cout << "length = " << iTail - iFront << " [" << iFront << ", " << iTail << "]" << endl;

    // 没找到合适的线段长度
    if(iFront >= iTail)
    {
        iRe = 0;
        cout << "front is bigger than tail, cannot go through" << endl;
        return iRe;
    }

    // 线段长度不够
    if(iTail - iFront < mPic.cols / 3)
    {
        iRe = 0;
        cout << "width is too small, cannot go through" << endl;
        return iRe;
    }

    Vec3f ptLeft, ptRight, ptMid;
    ptLeft = mPic.at<Vec3f>((int)(mPic.rows / 2), iFront);
    ptRight = mPic.at<Vec3f>((int)(mPic.rows / 2), iTail);

    // merge
    /*
    vector<Mat> channels;
    channels.push_back(mPic);
    channels.push_back(mPic);
    channels.push_back(mPic);

    merge(channels, mClr);
    */

    float fLength = abs(ptLeft[0]*16 - ptRight[0]*16);
    if(fLength - g_config.iPlaneWid < EPSILON)
    {
        iRe = 0;
        iFront = -1;
        iTail = -1;
        //cout << "cannot go through" << endl;
    }
    else
    {
        Point ptCenter;
        ptCenter.x = (iFront + iTail) / 2;
        ptCenter.y = mPic.rows / 2;
        circle(mClr, ptCenter, 5, Scalar(255, 0, 255));
        ptMid = mPic.at<Vec3f>(ptCenter.y, ptCenter.x);
        cout << "[" << ptMid[0] * 16 << ", " << ptMid[1] * 16 << ", " << ptMid[2] * 16 << "]" << endl;

        // draw left picture
        ptCenter.x = (iFront + iTail) / 2;
        ptCenter.y = mLeft.rows / 2;
        circle(mLeft, ptCenter, 5, Scalar(255, 0, 255));
    }

    return iRe;

}