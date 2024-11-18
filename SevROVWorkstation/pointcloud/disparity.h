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
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>


/// Структура, содержащая параметры объектов StereoSGBM / BM
typedef struct StereoSGBMstruct{
    int minDisparity = 0;
    int numDisparities = 250;
    int blockSize = 15;
    int P1_ = 0;
    int P2_ = 0;
    int disp12MaxDiff = 0;
    int preFilterCap = 0;
    int uniquenessRatio = 5;
    int speckleWindowSize = 10;
    int speckleRange = 2;
    int mode = cv::StereoSGBM::MODE_SGBM;
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

/*!
 *  \brief Функция для вычисления карты диспарантности методом BM
 *
 *  \param[in] rectifiedLeft Ректифицированное левое изображение
 *  \param[in] rectifiedRight Ректифицированное правое изображение
 *  \param[in] cameraMatrixLeft Матрица левой камеры
 *  \param[in] cameraMatrixRight Матрица правой камеры
 *  \param[in] T Матрица смещений, получаемая при калибровке
 *  \param[in] numDisparities Количество диспарантностей
 *  \param[in] minDisparity Минимальное значение диспарантности
 *  \param[in] stereo Объект StereoBM
 *
 *  \param[out] disparity Матрица значенией диспарантности
*/
void stereo_d_map(cv::Mat rectifiedImageLeft, cv::Mat rectifiedImageRight, cv::Mat &disparity, cv::Ptr<cv::StereoBM> &stereo);

/**
 * @brief Функция для чтения параметров объекта SGBM для метода рассчёта карты диспаратности
 *
 * @param filename  - путь к файлу конфигурации
 * @param params    - структура парамтеров объекта SGBM
 * @return - код 0, если чтение произведено успешно. 1 - если безуспешно
 */
int loadSGBMParams(const std::string& filename, stereo_sgbm_t& params);

#endif // DISPARITY_H
