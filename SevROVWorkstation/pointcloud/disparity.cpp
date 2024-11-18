#include "disparity.h"

int loadSGBMParams(const std::string& filename, stereo_sgbm_t& params) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть конфигурационный файл " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        int value;

        if (iss >> key >> value) {
            if (key == "numDisparities") params.numDisparities = value;
            else if (key == "blockSize") params.blockSize = value;
            else if (key == "minDisparity") params.minDisparity = value;
            else if (key == "preFilterCap") params.preFilterCap = value;
            else if (key == "uniquenessRatio") params.uniquenessRatio = value;
            else if (key == "speckleWindowSize") params.speckleWindowSize = value;
            else if (key == "speckleRange") params.speckleRange = value;
            else if (key == "disp12MaxDiff") params.disp12MaxDiff = value;
            else if (key == "P1") params.P1_ = value;
            else if (key == "P2") params.P2_ = value;
            else if (key == "mode") params.mode = value;
        }
    }

    file.close();

    return 0;
}


/// ====================================
/// Рассчёт карты диспаратности без CUDA
/// ====================================
void stereo_d_map(cv::Mat rectifiedImageLeft, cv::Mat rectifiedImageRight, cv::Mat &disparity,
                  cv::Ptr<cv::StereoSGBM> &stereo){

    cv::Mat coloredDispMap, disparityMap, disparity_norm;
    stereo->compute(rectifiedImageLeft, rectifiedImageRight, disparityMap);

    disparityMap.convertTo(disparity,CV_32F,0.0625f);

    cv::normalize(disparity, disparity_norm, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    #ifdef QT_DEBUG
        //cv::imshow("Filtered disparity Map", filtered);
        //cv::applyColorMap(filtered, filtered, cv::COLORMAP_JET);
        //cv::imshow("Colorized filter Map", filtered);

    #endif

    cv::applyColorMap(disparity_norm, coloredDispMap, cv::COLORMAP_JET);
    cv::imshow("Colored disparity Map", coloredDispMap);
    //cv::imshow("Rectified left", rectifiedImageLeft);
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
