#include "matchStereo.h"
#include "CommonOpt.h"

// global function
void sharpenImage(const Mat &image, Mat &result)
{
    //创建并初始化滤波模板
    Mat kernel(3,3,CV_32F,cv::Scalar(0));
    kernel.at<float>(1,1) = 5.0;
    kernel.at<float>(0,1) = -1.0;
    kernel.at<float>(1,0) = -1.0;
    kernel.at<float>(1,2) = -1.0;
    kernel.at<float>(2,1) = -1.0;

    result.create(image.size(),image.type());

    //对图像进行滤波
    filter2D(image,result,image.depth(),kernel);
}


// matchStereos functions
matchStereos::matchStereos()
{
    m_iAlg = STEREO_SGBM;
    m_fScale = 1.0;
    m_szImg_size = cv::Size(480, 640);
    m_iChannels = 3;
    m_iNumberOfDisparities = 3;
    m_iBlockSz = 7;

}


matchStereos::matchStereos(int iAlg, String strIntrinsic, String strExtrinsic, int iWid, int iHei, int iChannel)
{
    if(iAlg > STEREO_3WAY)
        iAlg = STEREO_SGBM;

    m_iAlg = iAlg;
    m_fScale = 1.0;
    m_strInyml = strIntrinsic;
    m_strExyml = strExtrinsic;
    m_szImg_size = cv::Size(iHei, iWid);
    m_iChannels = iChannel;
    m_iBlockSz = 7;
    m_iNumberOfDisparities = 3;

}


matchStereos::~matchStereos()
{

}


void matchStereos::setIntrinsicYml(String strIntrinsic)
{
    m_strInyml = strIntrinsic;
}


void matchStereos::setBlockSize(int iBlockSz)
{
    m_iBlockSz = iBlockSz;
}


void matchStereos::setExtrinsicYml(String strExtrinsic)
{
    m_strExyml = strExtrinsic;
}


void matchStereos::setSize(Size &img_size)
{
    m_szImg_size = cv::Size(img_size.height, img_size.width);

}


void matchStereos::setSADWindowSize(int iSize)
{
    if(m_iAlg == STEREO_BM)
        m_iSADWindowSize = iSize > 0 ? iSize : 3;
    else
        m_iSADWindowSize = iSize > 0 ? iSize : 9;
}


void matchStereos::setNumberOfDisparities(int iNumberDisparity)
{
    m_iNumberOfDisparities = iNumberDisparity > 0 ? iNumberDisparity : ((m_szImg_size.width/8) + 15) & -16;
}


void matchStereos::setShowSub(int iShowSub)
{
    m_iShowSub = iShowSub;
}


void matchStereos::setChannel(int iChannel)
{
    m_iChannels = iChannel;
}

void matchStereos::setAlgorithm(int iAlg)
{
    if(iAlg > STEREO_3WAY)
        iAlg = STEREO_SGBM;
    m_iAlg = iAlg;
}


void matchStereos::init()
{
    printf("matchStereos::init() alg = %d\n", m_iAlg);
    switch(m_iAlg)
    {
    case STEREO_BM:
    {
        printf("STEREO_BM \n");
        match_stereo_bm_init();
        break;
    }
    case STEREO_SGBM:
    {        
        printf("STEREO_SGBM \n");
        match_stereo_sgbm_init();
        m_sgbm->setMode(StereoSGBM::MODE_SGBM);
        break;
    }
    case STEREO_HH:
    {        
        printf("STEREO_HH \n");
        match_stereo_sgbm_init();
        m_sgbm->setMode(StereoSGBM::MODE_HH);
        break;
    }
    case STEREO_3WAY:
    {        
        printf("STEREO_3WAY \n");
        match_stereo_sgbm_init();
        m_sgbm->setMode(StereoSGBM::MODE_SGBM_3WAY);
        break;
    }
    default:
        printf("DEFAULT \n");
        match_stereo_bm_init();
        break;
    }

    getYmlParameter();
}


void matchStereos::getYmlParameter()
{
    if( !m_strInyml.empty())
    {
        // reading intrinsic parameters
        FileStorage fs(m_strInyml, FileStorage::READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", m_strInyml.c_str());
            return ;
        }

        printf("open intrinsic.yml...\n");

        fs["M1"] >> m_mM1;
        fs["D1"] >> m_mD1;
        fs["M2"] >> m_mM2;
        fs["D2"] >> m_mD2;

        m_mM1 *= m_fScale;
        m_mM2 *= m_fScale;

        fs.open(m_strExyml, FileStorage::READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", m_strExyml.c_str());
            return ;
        }

        printf("open extrinsic.yml...\n");
        // Mat R, T, R1, P1, R2, P2;
        fs["R"] >> m_mR;
        fs["T"] >> m_mT;

        stereoRectify( m_mM1, m_mD1, m_mM2, m_mD2, m_szImg_size, m_mR, m_mT, m_mR1, m_mR2, m_mP1, m_mP2, m_mQ,
                       CALIB_ZERO_DISPARITY, -1, m_szImg_size, &m_recRoi1, &m_recRoi2 );

        // Mat map11, map12, map21, map22;
        initUndistortRectifyMap(m_mM1, m_mD1, m_mR1, m_mP1, m_szImg_size, CV_16SC2, m_mMap11, m_mMap12);
        initUndistortRectifyMap(m_mM2, m_mD2, m_mR2, m_mP2, m_szImg_size, CV_16SC2, m_mMap21, m_mMap22);

    }
}


void matchStereos::reMap(Mat &img1, Mat &img2)
{
    Mat img1r, img2r;

    if (m_fScale != 1.f)
    {
        Mat temp1, temp2;
        int method = m_fScale < 1 ? INTER_AREA : INTER_CUBIC;
        resize(img1, temp1, Size(), m_fScale, m_fScale, method);
        img1 = temp1;
        resize(img2, temp2, Size(), m_fScale, m_fScale, method);
        img2 = temp2;
    }

    remap(img1, img1r, m_mMap11, m_mMap12, INTER_LINEAR);
    remap(img2, img2r, m_mMap21, m_mMap22, INTER_LINEAR);
    img1 = img1r;
    img2 = img2r;
}


void matchStereos::match_stereo_bm_init()
{
    cout << "match_stereo_bm init..." << endl;
    m_bm = StereoBM::create(16,9);
    m_bm->setROI1(m_recRoi1);
    m_bm->setROI2(m_recRoi2);
    m_bm->setPreFilterCap(31);
    m_bm->setBlockSize(m_iBlockSz);
    m_bm->setMinDisparity(0);
    m_bm->setNumDisparities(m_iNumberOfDisparities*16);
    m_bm->setTextureThreshold(10);
    m_bm->setUniquenessRatio(15);
    m_bm->setSpeckleWindowSize(100);
    m_bm->setSpeckleRange(32);
    m_bm->setDisp12MaxDiff(1);
    cout << "match_stereo_bm end..." << endl;
}


void matchStereos::match_stereo_sgbm_init()
{

    cout << "sadwndsize = " << m_iSADWindowSize << endl;
    cout << "numberofdisparity = " << m_iNumberOfDisparities << endl;
    cout << "blocksize = " << m_iBlockSz << endl;
    m_sgbm = StereoSGBM::create(0,16,3);
    m_sgbm->setPreFilterCap(63);
    m_sgbm->setBlockSize(m_iBlockSz);

    m_sgbm->setP1(8* m_iChannels * m_iSADWindowSize * m_iSADWindowSize);
    m_sgbm->setP2(32* m_iChannels * m_iSADWindowSize * m_iSADWindowSize);

    m_sgbm->setMinDisparity(0);
    m_sgbm->setNumDisparities(16*m_iNumberOfDisparities);

    m_sgbm->setUniquenessRatio(10);
    m_sgbm->setSpeckleWindowSize(100);
    m_sgbm->setSpeckleRange(32);
    m_sgbm->setDisp12MaxDiff(1);

}


void matchStereos::match_processing(Mat &img1, Mat &img2, Mat &points, Mat &disp, Mat &norm_disp, bool bNorm)
{

    switch(m_iAlg)
    {
    case STEREO_BM:
    {
        m_bm->compute(img1, img2, disp);
        break;
    }
    case STEREO_SGBM:
    {
        m_sgbm->compute(img1, img2, disp);

        // m_sgbm->compute(mleftGray, mrightGray, disp);
        break;
    }
    case STEREO_HH:
    {
        m_sgbm->compute(img1, img2, disp);
        break;
    }
    case STEREO_3WAY:
    {
        m_sgbm->compute(img1, img2, disp);
        break;
    }
    default:
        break;
    }

    blur(disp, disp, Size(16, 16));

    if(bNorm)
    {
        normalize(disp, norm_disp, 0, 255, CV_MINMAX, CV_8UC1);
        reprojectImageTo3D(disp, points, m_mQ, true);
    }

}


void matchStereos::match_preProcess(Mat &mleft, Mat &mright)
{
    sharpenImage( mleft, mleft );
    sharpenImage( mright, mright );
}


void matchStereos::ReprojectImageTo3D(Mat &disp, Mat& norm_disp, Mat &points)
{
    normalize(disp, norm_disp, 0, 255, CV_MINMAX, CV_8UC1);
    reprojectImageTo3D(disp, points, m_mQ, true);
}
