
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

uint8_t *buffer[4];


int iCameraInx[4] = {1,2,3,4};

static int xioctl(int fd, int request, void *arg)
{
    int r;

    do r = ioctl (fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}

int print_caps(int fd)
{
    struct v4l2_capability caps = {};
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps))
    {
        perror("Querying Capabilities");
        return 1;
    }

    printf( "Driver Caps:\n"
                    "  Driver: \"%s\"\n"
                    "  Card: \"%s\"\n"
                    "  Bus: \"%s\"\n"
                    "  Version: %d.%d\n"
                    "  Capabilities: %08x\n",
            caps.driver,
            caps.card,
            caps.bus_info,
            (caps.version>>16)&&0xff,
            (caps.version>>24)&&0xff,
            caps.capabilities);


    struct v4l2_cropcap cropcap = {0};
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
    {
        perror("Querying Cropping Capabilities");
        return 1;
    }

    printf( "Camera Cropping:\n"
                    "  Bounds: %dx%d+%d+%d\n"
                    "  Default: %dx%d+%d+%d\n"
                    "  Aspect: %d/%d\n",
            cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
            cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
            cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);

    int support_grbg10 = 0;

    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char fourcc[5] = {0};
    char c, e;
    printf("  FMT : CE Desc\n--------------------\n");
    while (0 == xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
    {
        strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
        if (fmtdesc.pixelformat == V4L2_PIX_FMT_SGRBG10)
            support_grbg10 = 1;
        c = fmtdesc.flags & 1? 'C' : ' ';
        e = fmtdesc.flags & 2? 'E' : ' ';
        printf("  %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);
        fmtdesc.index++;
    }
    /*
    if (!support_grbg10)
    {
        printf("Doesn't support GRBG10.\n");
        return 1;
    }*/

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 1280;
    fmt.fmt.pix.height = 480;
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
    {
        perror("Setting Pixel Format");
        return 1;
    }

    strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
    printf( "Selected Camera Mode:\n"
                    "  Width: %d\n"
                    "  Height: %d\n"
                    "  PixFmt: %s\n"
                    "  Field: %d\n",
            fmt.fmt.pix.width,
            fmt.fmt.pix.height,
            fourcc,
            fmt.fmt.pix.field);
    return 0;
}

int init_mmap(int fd, int iInx)
{
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        perror("Requesting Buffer");
        return 1;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
    {
        perror("Querying Buffer");
        return 1;
    }

    buffer[iInx] = (uint8_t *)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    printf("Length: %d\nAddress: %p\n", buf.length, buffer[iInx]);
    printf("Image Length: %d\n", buf.bytesused);

    return 0;
}


int single_caps_image(int fd, char *strWndName, int iInx)
{

    struct v4l2_buffer bufferinfo;

    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0; /* Queueing buffer index 0. */

// Put the buffer in the incoming queue.
    if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
        perror("VIDIOC_QBUF");
        exit(1);
    }

// Activate streaming
    int type = bufferinfo.type;
    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
        perror("VIDIOC_STREAMON");
        exit(1);
    }

    while(/* main loop */1){
        // Dequeue the buffer.
        if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0){
            perror("VIDIOC_QBUF");
            exit(1);
        }

        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        /* Set the index if using several buffers */

        // Queue the next one.
        if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
            perror("VIDIOC_QBUF");
            exit(1);
        }
    }

    // Deactivate streaming
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
        perror("VIDIOC_STREAMOFF");
        exit(1);
    }
}

int single_caps_image_show(int fds[], int iCameraCnt, char *strWndName, int iInx)
{

    struct v4l2_buffer bufferinfo;

    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0; /* Queueing buffer index 0. */

    int fd = 0;
    int type = 0;
    for(int i = 0; i < iCameraCnt; ++i)
    {
        fd = fds[i];
        // Put the buffer in the incoming queue.
        if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
            perror("VIDIOC_QBUF");
            exit(1);
        }

        // Activate streaming
        type = bufferinfo.type;
        if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
            perror("VIDIOC_STREAMON");
            exit(1);
        }
    }

    int iOut = 0;
    while(1)
    {

        for(int i = 0; i < iCameraCnt; ++i)
        {
            fd = fds[i];
            // Dequeue the buffer.
            if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0){
                perror("VIDIOC_QBUF");
                exit(1);
            }

            bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            bufferinfo.memory = V4L2_MEMORY_MMAP;
            /* Set the index if using several buffers */

            // Queue the next one.
            if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
                perror("VIDIOC_QBUF");
                exit(1);
            }
        }

    }

    for(int i = 0; i < iCameraCnt; ++i)
    {
        fd = fds[i];

        // Activate streaming
        type = bufferinfo.type;

        // Deactivate streaming
        if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
            perror("VIDIOC_STREAMOFF");
            exit(1);
        }
    }

}

int capture_image(int fd, char *strWndName, int iInx)
{
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    {
        perror("Query Buffer");
        return 1;
    }

    if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
    {
        perror("Start Capture");
        return 1;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(fd+1, &fds, NULL, NULL, &tv);
    if(-1 == r)
    {
        perror("Waiting for Frame");
        return 1;
    }

    if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
        perror("Retrieving Frame");
        return 1;
    }
    printf ("saving image\n");

    IplImage* frame;
    CvMat cvmat = cvMat(480, 1280, CV_8UC3, (void*)buffer[iInx]);
    frame = cvDecodeImage(&cvmat, 1);
    cvNamedWindow(strWndName,CV_WINDOW_AUTOSIZE);
    cvShowImage(strWndName, frame);
    cvSaveImage("sss.jpg", frame);

    int iRe = cvWaitKey(10);

    if(iRe == 27)
        return 1;
    // cvSaveImage("image.jpg", frame, 0);

    return 0;
}


int init_arrayCameras(int *fd, int iCameraCnt)
{
    int iRe = 1;
    for(int i = 0; i < iCameraCnt; ++i)
    {
        char strVideo[16];
        sprintf(strVideo, "/dev/video%d", iCameraInx[i]);
        fd[i] = open(strVideo, O_RDWR);

        printf("number: %d\n", i);
        if (fd[i] == -1)
        {
            perror("Opening video device");
            iRe = 0;
            break;
        }
        if(print_caps(fd[i]))
        {
            perror("Print video device");
            iRe = 0;
            break;
        }
        if(init_mmap(fd[i], i))
        {
            perror("init_mmap video device");
            iRe = 0;
            break;
        }
    }
    return iRe;
}

int main(int argc, char *argv[])
{
    int fd[4];
    printf("params number = %d\n", argc);

    int iCameraCnt = 1;
    int iRe = init_arrayCameras(fd, iCameraCnt);

    if(iRe == 0)
    {
        printf("init failed...\n");
        for(int i = 0; i < iCameraCnt; ++i)
            close(fd[i]);
        return 0;
    }

    int iRes = 1;
    for(; ; )
    {
        for(int i = 0; i < iCameraCnt; ++i)
        {
            char strWnd[16];
            sprintf(strWnd, "cap%d", i);
            if(capture_image(fd[i], strWnd, i))
            {
                iRes = 0;
                break;
            }
            //sleep(1);
        }

        if(iRes == 0)
            break;

    }

    for(int i = 0; i < iCameraCnt; ++i)
        close(fd[i]);

    return 0;
}