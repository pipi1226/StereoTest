// This is a demo introduces you to reading a video and camera
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

// multi thread
#include<pthread.h>
#include <unistd.h>
#include <fstream>

#include "GlobalConfigParams.h"
#include "stereo_calib.h"
using namespace cv;

VideoWriter videoWriteLeft, videoWriteRight;
VideoCapture cap1, cap2;
Mat frameL, frameR;


bool bwrite = false;
bool bterminate = false;

int iLeft = 0, iRight = 0;

// left thread
void *getleftframe(void *arg)
{
    while (1)
    {
        cap1 >> frameL;
        iLeft = 1;
        usleep(100);
    }

    return ((void *)0);
}

// right thread
void *getrightframe(void *arg)
{

    while (1)
    {
        cap2 >> frameR; // get a new frame from camera
        iRight = 1;
        usleep(100);
    }

    return ((void *)0);
}

// right thread
void *getsavesignal(void *arg)
{
    int itest = 0;
    while (1)
    {
        cin >> itest; // get a new frame from camera
        if(itest == 1)
        {
            bwrite = true;
        }
        if(itest == 3)
            bterminate = true;
    }

    return ((void *)0);
}


int main(int argc, const char** argv)
{

    getParameters(g_config);

    if (g_config.iCapture == WRITEVIDEO ||
        g_config.iCapture == WRITEPIC)
    {

        pthread_t tid1,tid2, tid3;
        int err;
        void *tret;

        cap1.open(g_config.iportl);
        cap2.open(g_config.iportr);


        if(!cap1.isOpened())
        {
            cout << "cap1 open failed!" << endl;
            return 0;
        }

        if (!cap2.isOpened())// check if we succeeded
        {
            cout << "cap2 open failed!" << endl;
            return 0;
        }

        // frame setting

        cap1.set(CV_CAP_PROP_FRAME_WIDTH, g_config.iImgWid);
        cap1.set(CV_CAP_PROP_FRAME_HEIGHT, g_config.iImgHei);

        cap2.set(CV_CAP_PROP_FRAME_WIDTH, g_config.iImgWid);
        cap2.set(CV_CAP_PROP_FRAME_HEIGHT, g_config.iImgHei);

        // write video
        if(g_config.iCapture == WRITEVIDEO)
        {
            if (!videoWriteLeft.isOpened())
            {
                videoWriteLeft.open(g_config.strLeftVideo, VideoWriter::fourcc('x','v','i','d'), 24,
                                    Size(g_config.iImgWid, g_config.iImgHei), true);
                if (!videoWriteLeft.isOpened())
                {
                    printf("can't create video writer\n");
                    return 0;
                }
                // throw std::runtime_error("can't create video writer");
            }

            if (!videoWriteRight.isOpened())
            {
                videoWriteRight.open(g_config.strRightVideo, VideoWriter::fourcc('x','v','i','d'), 24,
                                     Size(g_config.iImgWid, g_config.iImgHei), true);
                if (!videoWriteRight.isOpened())
                {
                    printf("can't create video writer\n");
                    return 0;
                }
                // throw std::runtime_error("can't create video writer");
            }


        }

        namedWindow("VideoLeft", 1);
        namedWindow("VideoRight", 1);

        err=pthread_create(&tid1,NULL, getleftframe, NULL);//创建线程
        if(err!=0)
        {
            printf("left frame catch pthread_create error:%s\n",strerror(err));
            exit(-1);
        }


        err=pthread_create(&tid2,NULL, getrightframe, NULL);
        if(err!=0)
        {
            printf("right frame catch pthread_create error:%s\n",strerror(err));
            exit(-1);
        }

        err=pthread_create(&tid3,NULL, getsavesignal, NULL);//创建线程
        if(err!=0)
        {
            printf("save frame pthread_create error:%s\n",strerror(err));
            exit(-1);
        }


        int iInit = 0;

        int ipiccnt = 1;


        bool bOpen = false;
        ofstream examplefile(g_config.strOut_filename);
        if (examplefile.is_open())
        {
            bOpen = true;
        }

        int itest = 0;
        const char *ptPath = g_config.strFolderPath;

        // show image
        while (1)
        {
            //if(bterminate)
            //  break;

            if (iLeft == 1 && iRight == 1)
            {
                imshow("VideoLeft", frameL);
                imshow("VideoRight", frameR);

                //for(int i = 0; i < 1000; ++i) printf("%dth picture cnt = %d", ipiccnt, i);
                if (g_config.iCapture == WRITEVIDEO)
                {
                    videoWriteLeft << frameL;
                    videoWriteRight << frameR;

                    if (waitKey(10) == 27) break;
                }
                else
                {
                    if (bwrite)
                    {
                        printf("write left and right picture\n");
                        char strL[32], strR[32];
                        memset(strL, 0, 32);
                        memset(strR, 0, 32);


                        // sprintf(strL, "%s/left%02d.jpg", ptPath, ipiccnt);

                        // sprintf(strR, "%s/right%02d.jpg", ptPath, ipiccnt);

                        Mat mleft(frameL), mright(frameR);
                        sprintf(strL, "left%02d.jpg", ipiccnt);
                        imwrite(strL, mleft);

                        sprintf(strR, "right%02d.jpg", ipiccnt);
                        imwrite(strR, mright);

                        printf("leftpicname = %s\nssss\n", strL);
                        printf("rightpicname = %s\n", strR);
                        examplefile << "\"" << strL << "\"" << endl;
                        examplefile << "\"" << strR << "\"" << endl;
                        ipiccnt += 1;
                        bwrite = false;
                    }


                    iInit += 1;

                    usleep(100);
                    if (waitKey(30) == 27) break;
                }


            }
            else
            {
                for(int i = 0; i < 1000; i++);
            }
            if (bterminate == true)
                break;
        }

        if(bOpen)
        {
            examplefile.close();
        }

        err=pthread_join(tid1,&tret);//阻塞等待线程id为tid1的线程，直到该线程退出
        if(err!=0)
        {
            printf("can not join with thread1:%s\n",strerror(err));
            exit(-1);
        }
        printf("thread 1 exit code %d\n", 0);


        err=pthread_join(tid2,&tret);
        if(err!=0)
        {
            printf("can not join with thread2:%s\n",strerror(err));
            exit(-1);
        }
        printf("thread 2 exit code %d\n", 0);

        err=pthread_join(tid3,&tret);
        if(err!=0)
        {
            printf("can not join with thread3:%s\n",strerror(err));
            exit(-1);
        }
        printf("thread 3 exit code %d\n", 0);

        // Release the camera or video file
        cap1.release();
        cap2.release();

        if(g_config.iCapture == WRITEVIDEO)
        {
            videoWriteLeft.release();
            videoWriteRight.release();
        }
        destroyAllWindows();
    }
    else if (g_config.iCapture == CALIBRATE)
    {
        stereo_calibrate();
    }

    return 0;
}
