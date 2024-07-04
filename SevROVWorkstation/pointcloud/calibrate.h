/*!
* \file
* \brief Заголовочный файл с описанием структур и функций калибровки

Данный файл содержит в себе определения основных структур и функций,
используемых в main программе для моно и стереокалибровки.
*/
#pragma once
#ifndef CALIBRATE_H
#define CALIBRATE_H


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
//#include <filesystem>

using namespace std;


/// Структура для хранения параметров калибровки одной камеры
typedef struct MonoOutputParams{
    cv::Mat cameraMatrix;     // Матрица камеры
    cv::Mat distCoeffs;       // Вектор коэффициентов дисторсии
    cv::Mat rvecs;            // Кортеж векторов поворота для перехода из базиса объекта в базис камеры
    cv::Mat tvecs;            // Кортеж векторов смещения для перехода из базиса объекта в базис камеры
    cv::Mat stdDevIntrinsics; // Вектор оценок внутренних параметров камеры
    cv::Mat stdDevExtrinsics; // Вектор оценок внешних параметров камеры
    cv::Mat perViewErrors;    // Вектор среднеквадратической ошибки перепроецирования для каждого вида
    double RMS;               // Значение среднеквадратической ошибки перепроецирования
}mono_output_par_t;

/// Структура для хранения параметров калибровки двух камер
typedef struct StereOutputParams{
    cv::Mat cameraM1;       // Матрица камеры 1
    cv::Mat cameraM2;       // Матрица камеры 2
    cv::Mat distCoeffs1;    // Вектор коэффициентов дисторсии камеры 1
    cv::Mat distCoeffs2;    // Вектор коэффициентов дисторсии камеры 2
    cv::Mat R;              // Матрица поворотов
    cv::Mat T;              // Вектор смещений
    cv::Mat E;              // Матрица существенных параметров
    cv::Mat F;              // Фундаментальная матрица
    cv::Mat rvecs;          // Кортеж векторов поворота для перехода из базиса объекта в базис камеры
    cv::Mat tvecs;          // Кортеж векторов смещения для перехода из базиса объекта в базис камеры
    cv::Mat perViewErrors;  // Вектор среднеквадратической ошибки перепроецирования для каждого вида
    double RMS;             // Значение среднеквадратической ошибки перепроецирования
}stereo_output_par_t;


/// Функция калибровки одной камеры
void calibrate_camera(std::vector<cv::String> images, mono_output_par_t &mono_out, int checkerboard_w, int checkerboard_h, float square_size);

/// Функция калибровки стереопары
void calibrate_stereo(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathL, std::string pathR,
                      stereo_output_par_t &outp_params, int checkerboard_w, int checkerboard_h, float square_size);

/*!
 *  \brief Функция калибровки стереопары (с возвращением моно + стерео параметров)
 *
 *  \param[in] imagesL          Вектор строк для хранения имён изображений левого изображения
 *  \param[in] imagesR          Вектор строк для хранения имён изображений левого изображения
 *  \param[in] pathL            Строка для хранения пути к фотосету левой камере
 *  \param[in] pathR            Строка для хранения пути к фотосету левой камере
 *  \param[in] mono_outL        Структура для хранения параметров калбировки левой камеры
 *  \param[in] checkerboard_c   Число ключевых точек в столбце
 *  \param[in] checkerboard_r   Число ключевых точек в строке
 *  \param[in] square_size      Размер квадрата калибровочной доски в мм
 *
 *  \param[out] mono_outR       Структура для хранения параметров калбировки правой камеры
 *  \param[out] mono_outL       Структура для хранения параметров калбировки левой камеры
*/
void calibrate_with_mono(std::vector<cv::String> imagesL,std::vector<cv::String> imagesR, std::string pathL,std::string pathR,
                         mono_output_par_t &mono_outL, mono_output_par_t &mono_outR, stereo_output_par_t &stereo_output_params,
                         int checkerboard_c, int checkerboard_r, float square_size);


/*!
* \brief Функция чтения параметров калибровки одной камеры из файлов
*
* \param[in] filename Путь к файлу с калибровочными параметрами в формате строки
*
* \param[out] Структура с калибровочными параметрами
*/
mono_output_par_t read_mono_params(std::string filename);

/*!
* \brief Функция чтения параметров калибровки двух камер из файлов
*
* \param[in] filename Путь к файлу с калибровочными параметрами в формате строки
*
* \param[out] Структура с калибровочными параметрами
*/
stereo_output_par_t read_stereo_params(std::string filename);

/// Функция для записи параметров калибровки одной камеры
void write_mono_params(std::string filename, mono_output_par_t mono_params_struct);

/// Функция для записи параметров калибровки двух камер
void write_stereo_params(std::string filename, stereo_output_par_t stereo_params_struct);

/// Функция для отображения параметров одной камеры
void print_mono_camera_parameters(std::string name, mono_output_par_t mono_struct);

/// Функция для отображения параметров стерео камеры
void print_stereo_camera_parameters(stereo_output_par_t stereo_struct);

#endif // CALLIBRATE_H
