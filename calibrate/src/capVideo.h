#ifndef _CAPVIDEOS_H
#define _CAPVIDEOS_H

#include "IncludeAll.h"

using namespace cv;


class capVideos
{
public:
    capVideos();
    capVideos(int iCameraPort1, int iCameraPort2, int iWid, int iHei, int iFPS=30, int iCameraCnt = 2);
    ~capVideos();

    void setFPS(int iFPS = 30);
    void setCameraPort(int iCameraPort1, int iCameraPort2);
    void setVideoFile(string strFile);
    void setShowSub(int iShowSub=1);
    void setCameraCnt(int iCnt = 2);
    void setChannel(int iChannel = 3);
    void setCaptureMethod(int iMethod = CAPTURE_VIDEO);
    void setWriteVideo(bool bWriteVideo = false);
    void insertMat(Mat &matInsert);
    void writeLeftMat(Mat &tmp);
    void writeRightMat(Mat &tmp);
    void releaseVideo();
    void setCapture(bool bCap);
    void setPicXMLPath(string strPath);
    void setWidAndHei(int iWid, int iHei);

    int init();
    int initCapture();
    int initReadVideo();
    int initWriteVideo();
    int initReadPic();
    int initWritePic();
    int initReadVideoIntoPic();


    int initCapStereoCamera();
    int initWriteCamera();
    int initWriteStitchCamera();

    int catchLeftFrame(Mat &tmp);
    int catchRightFrame(Mat &tmp);

    // get stereo picture while in stitching mode
    void getStereoPic(string strLeft, string strRight);

    VideoCapture cap1, cap2;
    VideoWriter m_videoWrite;
    VideoWriter m_videoLeftW;
    VideoWriter m_videoRightW;
    Mat frameL, frameR;

    vector<string> m_vecList;

    bool m_bInit;

private:
    int m_iStereo1;
    int m_iStereo2;
    string m_strVideoFile;
    string m_strWriteVideo;
    string m_strPicXML;

    int m_iWid, m_iHei;
    int m_iFPS;
    int m_iShowSub;

    int m_iCameraCnt;
    int m_iChannel;
    int m_iCapture;
    bool m_bWriteVideo;

};

#endif
