//
// Created by sky on 17-3-17.
//

#include "writeVideo.h"


/*
global variables
*/
// capture videos
capVideos caps;

// ----------------------
// init capvideos class
// ----------------------
int init_caps()
{
    int iRe = 1;
    caps.setCameraPort(g_config.iportl, g_config.iportr);
    caps.setCameraCnt(g_config.iCameraCnt);
    caps.setWidAndHei(g_config.iImgWid, g_config.iImgHei);
    caps.setChannel(g_config.iChannel);
    caps.setWriteVideo(g_config.bWriteVideo);
    caps.setCaptureMethod(g_config.iCapture);
    iRe = iRe & caps.init();
    g_iLeft = 0;
    g_iRight = 0;
    return iRe;
}



/*
 * add by yyq [2017-0317]
 * write picture function
 * */
void writeVideo()
{
    pthread_t tid1,tid2, tid3;
    int err;
    void *tret;

    int iRe = init_caps();

    if(iRe == 0)
    {
        cout << "open camera failed" << endl;
        return;
    }
    // create capture thread
    err=pthread_create(&tid1,NULL, getleftframe, &caps);//创建线程
    if(err!=0)
    {
        printf("left frame catch pthread_create error:%s\n",strerror(err));
        exit(-1);
    }

    sem_init(&sem_cap_id[0], 1, 0);
    sem_init(&sem_cap_release_id[0], 1, 0);

    if(g_config.iCameraCnt > 1)
    {
        err=pthread_create(&tid2,NULL, getrightframe, &caps);
        if(err!=0)
        {
            printf("right frame catch pthread_create error:%s\n",strerror(err));
            exit(-1);
        }

        sem_init(&sem_cap_id[1], 1, 0);
        sem_init(&sem_cap_release_id[1], 1, 0);
    }


    // show image
    while (1)
    {
        printf("sem_cap_id[0] wait ... \n");
        sem_wait(&sem_cap_id[0]);
        if(g_config.iCameraCnt > 1)
        {
            printf("sem_cap_id[1] wait ... \n");
            sem_wait(&sem_cap_id[1]);
        }

        if(g_iLeft == 1 && g_iRight == 1)
        {
            Mat mleft, mright;

            caps.frameL.copyTo(mleft);
            caps.frameR.copyTo(mright);

            caps.writeLeftMat(mleft);
            caps.writeRightMat(mright);

            imshow("VideoLeft", mleft);
            imshow("VideoRight", mright);

        }


        printf("sem_cap_release_id[0] post ... \n");
        sem_post(&sem_cap_release_id[0]);
        if(g_config.iCameraCnt > 1)
        {
            printf("sem_cap_release_id[1] post ... \n");
            sem_post(&sem_cap_release_id[1]);
        }

        int iRe = waitKey(10);
        if (iRe == 27)
        {
            g_iSignal = 1;
            printf("sem_cap_release_id[0] post ... \n");
            sem_post(&sem_cap_release_id[0]);
            if(g_config.iCameraCnt > 1)
            {
                printf("sem_cap_release_id[1] post ... \n");
                sem_post(&sem_cap_release_id[1]);
            }
            break;
        }


    }

    destroyAllWindows();
}


/*
 * add by yyq [2017-0317]
 * write picture function
 * */
void writePicture()
{
    pthread_t tid1,tid2, tid3;
    int err;
    void *tret;



    while(1)
    {
        Mat mleft, mright;
        break;
    }

}


// right thread
bool bwrite = false;


/*
 * add by yyq [2017-0317]
 * write picture function
 * */
void readVideoIntoPic()
{
    int cnt = 1;

    int iRe = init_caps();

    if (iRe == 0)
    {
        cout << "readVideoIntoPic() open video failed..." << endl;
        return;
    }

    bool bOpen = false;
    ofstream examplefile(g_config.strOut_filename);
    if (examplefile.is_open())
    {
        bOpen = true;
    }

    namedWindow(strLeftWndname, 1);
    namedWindow(strRightWndname, 1);


    Mat mleft, mright;

    while(1)
    {

        int iReLeft = caps.catchLeftFrame(mleft);
        int iReRight = caps.catchRightFrame(mright);
        caps.frameL.copyTo(mleft);
        caps.frameR.copyTo(mright);

        if(iReLeft == 0 || iReRight == 0)
        {
            cout << "video end..." << endl;
            caps.releaseVideo();
            break;
        }

        char strLeft[32], strRight[32];
        memset(strLeft, 0, 32);
        memset(strRight, 0, 32);

        sprintf(strLeft, "left%02d.jpg", cnt);
        sprintf(strRight, "right%02d.jpg", cnt);

        imshow("VideoLeft", mleft);
        imshow("VideoRight", mright);


        int iResult = waitKey(0);
        if(iResult == 27)
        {
            break;
        }
        else if(iResult == 'w')
        {
            imwrite(strLeft, mleft);
            imwrite(strRight, mright);
            examplefile << "\"" << strLeft << "\"" << endl;
            examplefile << "\"" << strRight << "\"" << endl;
            cnt += 1;
            bwrite = false;
        }

    }

    if(bOpen)
    {
        examplefile.close();
    }


    destroyAllWindows();
    caps.releaseVideo();

}