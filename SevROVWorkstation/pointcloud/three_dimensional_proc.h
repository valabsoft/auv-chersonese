#pragma once
#ifndef THREE_DIMENSIONAL_PROC_H
#define THREE_DIMENSIONAL_PROC_H

#include "opencv2/core.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>

#include "../pointcloud/disparity.h"
#include "../pointcloud/calibrate.h"

/*!
 *  \brief Функция для нахождения двумерных и трёхмерных координат точек на стереоизображении
 *
 *  \param[in] imageL Ректифицированное левое изображение
 *  \param[in] imageR Ректифицированное правое изображение
 *  \param[in] calib_par Структура с параметрами стереокалибровки
 *
 *  \param[out] disparity Двумерный вектор, содержащий 2D и 3D координаты точек на изображении
*/
std::vector<std::vector<double>> point3d_finder(cv::Mat imageL, cv::Mat imageR, stereo_output_par_t calib_par, const stereo_sgbm_t SGBM_par);

/*!
 *  \brief Функция для нахождения трёхмерных координат точки по 2д клику
 *
 *  \param[in] imageL Ректифицированное левое изображение
 *  \param[in] imageR Ректифицированное правое изображение
 *  \param[in] xy Координаты 2д клика мыши
 *  \param[in] calib_par Структура с параметрами стереокалибровки
 *
 *  \param[out] point3D Вектор координат найденных 3д точек на изображении
*/
cv::Vec3f third_coords(cv::Mat imageL, cv::Mat imageR, cv::Point xy, stereo_output_par_t calib_par);

/*!
 *  \brief Функция для нахождения двумерных и трёхмерных координат точек на стереоизображении
 *
 *  \param[in] points23D Вектор двумерных и трёхмерных координат точек
 *  \param[in] filename Имя файла для записи точек
*/
void write_coords_file(std::vector<std::vector<double>> points23D,std::string filename);

#endif // THREE_DIMENSIONAL_PROC_H
