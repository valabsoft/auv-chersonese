#include "disparity.h"


/// ====================================
/// Рассчёт карты диспаратности без CUDA
/// ====================================
void stereo_d_map(cv::Mat rectifiedImageLeft, cv::Mat rectifiedImageRight, cv::Mat &disparity,
                  cv::Ptr<cv::StereoSGBM> &stereo){

    cv::Mat coloredDispMap, disparityMap, disparity_norm;
    stereo->compute(rectifiedImageLeft, rectifiedImageRight, disparityMap);

    disparityMap.convertTo(disparity,CV_32F,0.0625f);
    //disparity = (disparity/32.0f - (double)stereo->getMinDisparity())/((double)stereo->getNumDisparities());

    cv::normalize(disparity, disparity_norm, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    cv::Mat filtered;
    cv::medianBlur(disparity_norm, filtered, 11);
 
    #ifdef QT_DEBUG
        cv::imshow("Disparity Map", disparity);
        
        cv::applyColorMap(disparity_norm, coloredDispMap, cv::COLORMAP_JET);
        
        cv::imshow("Colored disparity Map", coloredDispMap);
        cv::imshow("Filtered disparity Map", filtered);

        cv::applyColorMap(filtered, filtered, cv::COLORMAP_JET);
        cv::imshow("Colorized filter Map", filtered);
    #endif

    disparity = filtered;
}

void stereo_d_map(cv::Mat rectifiedImageLeft, cv::Mat rectifiedImageRight, cv::Mat &disparity,
                  cv::Ptr<cv::StereoBM> &stereo){

    cv::Mat coloredDispMap, disparityMap, disparity_norm;

    stereo->compute(rectifiedImageLeft, rectifiedImageRight, disparityMap);

    disparityMap.convertTo(disparity,CV_32F,1.0f);
    //disparity = (disparity/32.0f - (double)stereo->getMinDisparity())/((double)stereo->getNumDisparities());

    cv::medianBlur(disparity, disparity, 5);

    cv::normalize(disparity, disparity_norm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    
    #ifdef QT_DEBUG
        cv::imshow("Disparity Map", disparity);

        cv::applyColorMap(disparity_norm, coloredDispMap, cv::COLORMAP_JET);
        cv::imshow("Colored disparity Map", coloredDispMap);
    #endif
}
