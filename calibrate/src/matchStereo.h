#ifndef _MATCHSTEREO_H
#define _MATCHSTEREO_H

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

// common struct
#include "IncludeAll.h"

using namespace cv;

class matchStereos
{
public:
    matchStereos();
    matchStereos(int iAlg, String strIntrinsic, String strExtrinsic, int iWid, int iHei, int iChannel = 3);
    ~matchStereos();

    void init();
    void setChannel(int iChannel);
    void setExtrinsicYml(String strExtrinsic);
    void setIntrinsicYml(String strInstrinsic);
    void setNumberOfDisparities(int iNumberDisparity);
    void setSize(Size &img_size);
    void setSADWindowSize(int iSize);
    void setBlockSize(int iBlockSz);
    void setShowSub(int iShowSub=true);
    void setAlgorithm(int iAlg);

    void getYmlParameter();
    void reMap(Mat &img1, Mat &img2);
    void ReprojectImageTo3D(Mat &disp, Mat& norm_disp, Mat &points);

    void match_stereo_sgbm_init();
    void match_stereo_bm_init();
    void match_preProcess(Mat &mleft, Mat &mright);
    void match_processing(Mat &img1, Mat &img2, Mat &points, Mat &disp, Mat &norm_disp, bool bNorm = true);

private:

    int m_iAlg;
    int m_iSADWindowSize;
    int m_iNumberOfDisparities;
    float m_fScale;
    int m_iChannels;
    int m_iBlockSz;
    int m_iShowSub;

    string m_strInyml, m_strExyml;

    // intrinsic parameters
    Size m_szImg_size;
    Mat m_mM1, m_mD1, m_mM2, m_mD2;

    // extrinsic parameters
    Mat m_mR, m_mT, m_mR1, m_mP1, m_mR2, m_mP2;
    Mat m_mMap11, m_mMap12;
    Mat m_mMap21, m_mMap22;

    Rect m_recRoi1, m_recRoi2;
    Mat m_mQ;
    Mat m_mDisp;

    // algorithm type
    Ptr<StereoSGBM> m_sgbm;
    Ptr<StereoBM> m_bm;

};

#endif
