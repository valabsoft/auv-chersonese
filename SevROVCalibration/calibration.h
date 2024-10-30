#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// Калибровка
/////////////////////////////////////////////////////////////////////////////

// Структура для хранения параметров калибровки одиночной камеры
struct CalibrationParametersMono
{
    cv::Mat cameraMatrix;     // Матрица камеры
    cv::Mat distCoeffs;       // Вектор коэффициентов дисторсии
    cv::Mat rvecs;            // Кортеж векторов поворота для перехода из базиса объекта в базис камеры
    cv::Mat tvecs;            // Кортеж векторов смещения для перехода из базиса объекта в базис камеры
    cv::Mat stdDevIntrinsics; // Вектор оценок внутренних параметров камеры
    cv::Mat stdDevExtrinsics; // Вектор оценок внешних параметров камеры
    cv::Mat perViewErrors;    // Вектор среднеквадратической ошибки перепроецирования для каждого вида
    double RMS;               // Значение среднеквадратической ошибки перепроецирования
};

// Структура для хранения параметров калибровки стерео камеры
struct CalibrationParametersStereo {
    cv::Mat cameraMatrixL;	// Матрица левой камеры
    cv::Mat cameraMatrixR;	// Матрица правой камеры
    cv::Mat distCoeffsL;	// Вектор коэффициентов дисторсии левой камеры
    cv::Mat distCoeffsR;	// Вектор коэффициентов дисторсии правой камеры
    cv::Mat R;				// Матрица поворотов
    cv::Mat T;				// Вектор смещений
    cv::Mat E;				// Матрица существенных параметров
    cv::Mat F;				// Фундаментальная матрица
    cv::Mat rvecs;			// Кортеж векторов поворота для перехода из базиса объекта в базис камеры
    cv::Mat tvecs;			// Кортеж векторов смещения для перехода из базиса объекта в базис камеры
    cv::Mat perViewErrors;	// Вектор среднеквадратической ошибки перепроецирования для каждого вида
    double RMS;				// Значение среднеквадратической ошибки перепроецирования
};

// Структура конфигурационного файла для калибровки
struct CalibrationConfig
{
    std::string folder_name = "../calibration-images/";	// Путь к конфигурационному файлу
    int keypoints_c = 9;								// Число ключевых точек вдоль одного столбца калибровочной доски
    int keypoints_r = 6;								// Число ключевых точек вдоль одной строки калибровочной доски
    float square_size = 20.1;							// Размер квадрата калибровочной доски в мм
    int image_count = 50;								// Общее число пар изображений в фотосете
};

/**
 * @brief Функция общей калибровки.
 * @param imagesL - Вектор строк имён изображений левой камеры.
 * @param imagesR - Вектор строк имён изображений правой камеры.
 * @param pathToImagesL - Путь к папке с изображениями левой камеры.
 * @param pathToImagesR - Путь к папке с изображениями правой камеры.
 * @param calibrationParametersL - Структура для хранения калибровочных параметров левой камеры.
 * @param calibrationParametersR - Структура для хранения калибровочных параметров правой камеры.
 * @param calibrationParameters - Структура для хранения калибровочных параметров стерео пары.
 * @param chessboardColCount - Количество ключевых точек калибровочной доски по столбцам.
 * @param chessboardRowCount - Количество ключевых точек калибровочной доски по строкам.
 * @param chessboardSquareSize - Размер поля калиброчно доски в мм.
 */
void cameraCalibration(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathToImagesL, std::string pathToImagesR, CalibrationParametersMono& calibrationParametersL, CalibrationParametersMono& calibrationParametersR, CalibrationParametersStereo& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize);

/**
 * @brief Функция калибровки одиночной камеры.
 * @param images - Вектор строк имён изображений камеры.
 * @param pathToImages - Путь к папке с изображениями камеры.
 * @param calibrationParameters - Структура для хранения калибровочных параметров камеры.
 * @param chessboardColCount - Количество ключевых точек калибровочной доски по столбцам.
 * @param chessboardRowCount - Количество ключевых точек калибровочной доски по строкам.
 * @param chessboardSquareSize - Размер поля калиброчно доски в мм.
 */
void cameraCalibrationMono(std::vector<cv::String> images, std::string pathToImages, CalibrationParametersMono& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize);

/**
 * @brief Функция калибровки стерео пары.
 * @param imagesL - Вектор строк имён изображений левой камеры.
 * @param imagesR - Вектор строк имён изображений правой камеры.
 * @param pathToImagesL - Путь к папке с изображениями левой камеры.
 * @param pathToImagesR - Путь к папке с изображениями правой камеры.
 * @param calibrationParameters - Структура для хранения калибровочных параметров стерео пары.
 * @param chessboardColCount - Количество ключевых точек калибровочной доски по столбцам.
 * @param chessboardRowCount - Количество ключевых точек калибровочной доски по строкам.
 * @param chessboardSquareSize - Размер поля калиброчно доски в мм.
 */
void cameraCalibrationStereo(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathToImagesL, std::string pathToImagesR, CalibrationParametersStereo& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize);

/**
 * @brief Функция чтения параметров калибровки одиночной камеры.
 * @param fileName - Полный путь к файлу калибровочных параметров.
 * @return
 */
CalibrationParametersMono readCalibrationParametersMono(std::string fileName);

/**
 * @brief Функция записи параметров калибровки одиночной камеры.
 * @param fileName - Полный путь к файлу калибровочных параметров.
 * @param parameters - Структура для хранения калибровочных параметров.
 */
void writeCalibrationParametersMono(std::string fileName, CalibrationParametersMono parameters);

/**
 * @brief Функция записи параметров калибровки стерео пары
 * @param fileName - Полный путь к файлу калибровочных параметров.
 * @param parameters - Структура для хранения калибровочных параметров.
 */
void writeCalibrationParametersStereo(std::string fileName, CalibrationParametersStereo parameters);

/**
 * @brief Функция чтения параметров калибровки стерео пары
 * @param fileName - Полный путь к файлу калибровочных параметров.
 * @return - Структура для хранения калибровочных параметров.
 */
CalibrationParametersStereo readCalibrationParametersStereo(std::string fileName);
/**
 * @brief Функция чтения конфигурационного файла для калибровки
 * @param pathToConfigFile - Полный путь к конфигурационному файлу.
 * @return - Структура для хранения параметров процедуры калибровки.
 */
int readCalibrartionConfigFile(std::string pathToConfigFile, CalibrationConfig& config);
/////////////////////////////////////////////////////////////////////////////
#endif // CALIBRATION_H
