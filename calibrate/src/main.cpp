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

#include "writeVideo.h"


int main(int argc, const char** argv)
{

    getParameters(g_config);

    switch(g_config.iCapture)
    {
        case WRITE_VIDEO:
        {
            writeVideo();
            break;
        }
        case WRITE_PIC:
        {
            writePicture();
            break;
        }
        case CALIBRATE:
        {
            stereo_calibrate();
            break;
        }
        case READ_VIDEO_INTO_PIC:
        {
            readVideoIntoPic();
            break;
        }
        default:
            break;

    }

    cout << "exit()..." << endl;
    return 0;
}
