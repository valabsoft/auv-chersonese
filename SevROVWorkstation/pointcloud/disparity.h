/*!
 * \file
 * \brief Заголовочный файл с определением структур и функций 3д калибровки,
 *
Данный файл содержит определения структур и функций, используемых для вычисления
и вывода карт диспарантности и глубины.
*/
#pragma once
#ifndef DISPARITY_H
#define DISPARITY_H

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


/// Структура, содержащая параметры объектов StereoSGBM / BM
typedef struct StereoSGBMstruct{
    int minDisparity = 0;  // 40
    int numDisparities = 4;
    int blockSize = 2; // 0
    int P1_ = 0;
    int P2_ = 0;
    int disp12MaxDiff = 0;
    int preFilterCap = 0;
    int uniquenessRatio = 3;
    int speckleWindowSize = 80;
    int speckleRange = 2;
}stereo_sgbm_t;

typedef struct StereoBMstruct{
    int preFilterCap = 31;
    int preFilterSize = 7;
    int preFilterType = cv::StereoBM::PREFILTER_XSOBEL;
    //cv::Rect ROI1(10, 200, 1000, 700);
    //cv::Rect ROI2(200, 250, 800, 600);
    int blockSize = 7;
    int getTextureThreshhold = 10;
    int uniquenessRatio = 15;
    int numDisparities = 17;
}stereo_bm_t;


/*!
 *  \brief Функция для вычисления карты диспарантности методом SGBM
 *
 *  \param[in] rectifiedLeft Ректифицированное левое изображение
 *  \param[in] rectifiedRight Ректифицированное правое изображение
 *  \param[in] cameraMatrixLeft Матрица левой камеры
 *  \param[in] cameraMatrixRight Матрица правой камеры
 *  \param[in] T Матрица смещений, получаемая при калибровке
 *  \param[in] numDisparities Количество диспарантностей
 *  \param[in] minDisparity Минимальное значение диспарантности
 *  \param[in] stereo Объект StereoSGBM
 *
 *  \param[out] disparity Матрица значенией диспарантности
*/
void stereo_d_map(cv::Mat rectifiedLeft, cv::Mat rectifiedRight, cv::Mat &disparity, cv::Ptr<cv::StereoSGBM> &stereo);

void stereo_d_map(cv::Mat rectifiedImageLeft, cv::Mat rectifiedImageRight, cv::Mat &disparity, cv::Ptr<cv::StereoBM> &stereo);

#endif // DISPARITY_H
