#include "calibration.h"

/**
 * @brief Функция общей калибровки.
 * @param imagesL
 * @param imagesR
 * @param pathToImagesL
 * @param pathToImagesR
 * @param calibrationParametersL
 * @param calibrationParametersR
 * @param calibrationParameters
 * @param chessboardColCount
 * @param chessboardRowCount
 * @param chessboardSquareSize
 */
void cameraCalibration(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathToImagesL, std::string pathToImagesR, CalibrationParametersMono& calibrationParametersL, CalibrationParametersMono& calibrationParametersR, CalibrationParametersStereo& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize)
{
    // Объявление вектора ключевых точек
    std::vector<std::vector<cv::Point3f>> keyPoints;

    // Объявление вектора для хранения координат 2D точек для каждого изображения шахматной доски
    std::vector<std::vector<cv::Point2f>> imgPointsL;
    std::vector<std::vector<cv::Point2f>> imgPointsR;

    // Определение координат мировых 3D точек
    std::vector<cv::Point3f> points3D;
    for (int i{ 0 }; i < chessboardRowCount; i++)
    {
        for (int j{ 0 }; j < chessboardColCount; j++)
        {
            points3D.push_back(cv::Point3f((float)j * chessboardSquareSize, (float)i * chessboardSquareSize, 0.f));
        }
    }

    // Загрузка путей изображений
    cv::glob(pathToImagesL, imagesL);
    cv::glob(pathToImagesR, imagesR);

    cv::Mat frameL;
    cv::Mat frameR;
    cv::Mat grayL;
    cv::Mat grayR;

    std::vector<cv::Point2f> cornerPointsL;
    std::vector<cv::Point2f> cornerPointsR;

    bool successL;
    bool successR;

    for (int i{ 0 }; i < (int)imagesL.size(); i++)
    {
        frameL = cv::imread(imagesL[i]);
        cv::cvtColor(frameL, grayL, cv::COLOR_BGR2GRAY);

        frameR = cv::imread(imagesR[i]);
        cv::cvtColor(frameR, grayR, cv::COLOR_BGR2GRAY);

        // Нахождение углов шахматной доски
        // Если на изображении найдено нужное число углов success = true
        successL = cv::findChessboardCorners(grayL, cv::Size(chessboardColCount, chessboardRowCount), cornerPointsL, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);
        successR = cv::findChessboardCorners(grayR, cv::Size(chessboardColCount, chessboardRowCount), cornerPointsR, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);

        if (successL && successR)
        {
            cv::TermCriteria terminateCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1e-6);

            // Уточнение координат пикселей для заданных двумерных точек
            cv::cornerSubPix(grayL, cornerPointsL, cv::Size(11, 11), cv::Size(-1, -1), terminateCriteria);
            cv::cornerSubPix(grayR, cornerPointsR, cv::Size(11, 11), cv::Size(-1, -1), terminateCriteria);

            // Отображение обнаруженных угловых точек на шахматной доске
            cv::drawChessboardCorners(frameL, cv::Size(chessboardColCount, chessboardRowCount), cornerPointsL, successL);
            cv::drawChessboardCorners(frameR, cv::Size(chessboardColCount, chessboardRowCount), cornerPointsR, successL);

            keyPoints.push_back(points3D);
            imgPointsL.push_back(cornerPointsL);
            imgPointsR.push_back(cornerPointsR);

        }
    }

    cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, DBL_EPSILON);
    int flags = 0;

    // Калибровка левой камеры
    calibrationParametersL.RMS = cv::calibrateCamera(keyPoints, imgPointsL, cv::Size(grayL.cols, grayL.rows), calibrationParametersL.cameraMatrix, calibrationParametersL.distCoeffs, calibrationParametersL.rvecs, calibrationParametersL.tvecs, calibrationParametersL.stdDevIntrinsics, calibrationParametersL.stdDevExtrinsics, calibrationParametersL.perViewErrors, flags, criteria);

    // Калибровка правой камеры
    calibrationParametersR.RMS = cv::calibrateCamera(keyPoints, imgPointsR, cv::Size(grayL.cols, grayL.rows), calibrationParametersR.cameraMatrix, calibrationParametersR.distCoeffs, calibrationParametersR.rvecs, calibrationParametersR.tvecs, calibrationParametersR.stdDevIntrinsics, calibrationParametersR.stdDevExtrinsics, calibrationParametersR.perViewErrors, flags, criteria);

    // Калибровка двух камер
    calibrationParameters.RMS = cv::stereoCalibrate(keyPoints, imgPointsL, imgPointsR, calibrationParameters.cameraMatrixL, calibrationParameters.distCoeffsL, calibrationParameters.cameraMatrixR, calibrationParameters.distCoeffsR, cv::Size(grayL.cols, grayL.rows), calibrationParameters.R, calibrationParameters.T, calibrationParameters.E, calibrationParameters.F, calibrationParameters.perViewErrors, 0, criteria);
}

/**
 * @brief Функция калибровки одиночной камеры.
 * @param images
 * @param pathToImages
 * @param calibrationParameters
 * @param chessboardColCount
 * @param chessboardRowCount
 * @param chessboardSquareSize
 */
void cameraCalibrationMono(std::vector<cv::String> images, std::string pathToImages, CalibrationParametersMono& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize)
{
    std::vector<std::vector<cv::Point3f>> keyPoints;
    std::vector<std::vector<cv::Point2f> > imgPoints;
    std::vector<cv::Point3f> points3D;

    for (int i{ 0 }; i < chessboardRowCount; i++)
    {
        for (int j{ 0 }; j < chessboardColCount; j++)
        {
            points3D.push_back(cv::Point3f(j * chessboardSquareSize, i * chessboardSquareSize, 0));
        }
    }

    cv::glob(pathToImages, images);

    cv::Mat frame;
    cv::Mat gray;

    std::vector<cv::Point2f> cornerPoints;

    bool success;

    // Цикл по изображениям в папке изображений
    for (int i{ 0 }; i < (int)images.size(); i++)
    {
        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Нахождение углов шахматной доски
        // Если на изображении найдено нужное число углов success = true
        success = cv::findChessboardCorners(
            gray,
            cv::Size(chessboardColCount, chessboardRowCount),
            cornerPoints,
            cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS
            );

        if (success)
        {
            cv::TermCriteria terminateCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

            cv::cornerSubPix(gray, cornerPoints, cv::Size(11, 11), cv::Size(-1, -1), terminateCriteria);

            cv::drawChessboardCorners(frame, cv::Size(chessboardColCount, chessboardRowCount), cornerPoints, success);

            keyPoints.push_back(points3D);
            imgPoints.push_back(cornerPoints);
        }
    }

    cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, DBL_EPSILON);

    calibrationParameters.RMS = cv::calibrateCamera(
        keyPoints,
        imgPoints,
        cv::Size(gray.rows, gray.cols),
        calibrationParameters.cameraMatrix,
        calibrationParameters.distCoeffs,
        calibrationParameters.rvecs,
        calibrationParameters.tvecs,
        calibrationParameters.stdDevIntrinsics,
        calibrationParameters.stdDevExtrinsics,
        calibrationParameters.perViewErrors,
        0,
        criteria
        );
}

/**
 * @brief Функция калибровки стерео пары.
 * @param imagesL
 * @param imagesR
 * @param pathToImagesL
 * @param pathToImagesR
 * @param calibrationParameters
 * @param chessboardColCount
 * @param chessboardRowCount
 * @param chessboardSquareSize
 */
/*
void cameraCalibrationStereo(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathToImagesL, std::string pathToImagesR, CalibrationParametersStereo& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize)
{
    std::vector<std::vector<cv::Point3f>> keyPoints;
    std::vector<std::vector<cv::Point2f>> imgPointsL;
    std::vector<std::vector<cv::Point2f>> imgPointsR;

    cv::glob(pathToImagesL, imagesL);
    cv::glob(pathToImagesR, imagesR);

    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    std::vector<cv::Point3f> points3D;
    for (int i{ 0 }; i < chessboardRowCount; i++)
    {
        for (int j{ 0 }; j < chessboardColCount; j++)
        {
            points3D.push_back(cv::Point3f(j * chessboardSquareSize, i * chessboardSquareSize, 0));
        }
    }

    std::vector<cv::Mat> cornerImagesL;
    std::vector<cv::Mat> cornerImagesR;

    for (int i{ 0 }; i < (int)imagesL.size(); i++)
    {
        cv::Mat cornerImageL = cv::imread(imagesL[i], 1);
        cornerImagesL.push_back(cornerImageL);

        cv::Mat cornerImageR = cv::imread(imagesR[i], 1);
        cornerImagesR.push_back(cornerImageR);
    }

    cv::Mat grayL;
    cv::Mat grayR;

    for (size_t i = 0; i < imagesL.size(); i++)
    {
        cv::resize(cornerImagesL[i], cornerImagesL[i], cv::Size(), 0.5, 0.5, cv::INTER_AREA);
        cv::resize(cornerImagesR[i], cornerImagesR[i], cv::Size(), 0.5, 0.5, cv::INTER_AREA);

        cv::cvtColor(cornerImagesL[i], grayL, cv::COLOR_BGR2GRAY);
        cv::cvtColor(cornerImagesR[i], grayR, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> cornersL;
        std::vector<cv::Point2f> cornersR;

        bool cornerPositionFoundL = cv::findChessboardCorners(grayL, cv::Size(chessboardColCount, chessboardRowCount), cornersL, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);
        bool cornerPositionFoundR = cv::findChessboardCorners(grayR, cv::Size(chessboardColCount, chessboardRowCount), cornersR, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);

        if (cornerPositionFoundL && cornerPositionFoundR)
        {
            cv::cornerSubPix(grayL, cornersL, cv::Size(11, 11), cv::Size(-1, -1), criteria);
            cv::cornerSubPix(grayR, cornersR, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            cv::drawChessboardCorners(cornerImagesL[i], cv::Size(chessboardColCount, chessboardRowCount), cornersL, cornerPositionFoundL);
            cv::drawChessboardCorners(cornerImagesR[i], cv::Size(chessboardColCount, chessboardRowCount), cornersR, cornerPositionFoundR);

            keyPoints.push_back(points3D);
            imgPointsL.push_back(cornersL);
            imgPointsR.push_back(cornersR);
        }
    }

    calibrationParameters.RMS = cv::stereoCalibrate(
        keyPoints,
        imgPointsL,
        imgPointsR,
        calibrationParameters.cameraMatrixL,
        calibrationParameters.distCoeffsL,
        calibrationParameters.cameraMatrixR,
        calibrationParameters.distCoeffsR,
        cv::Size(grayL.cols, grayL.rows),
        calibrationParameters.R,
        calibrationParameters.T,
        calibrationParameters.E,
        calibrationParameters.F,
        calibrationParameters.perViewErrors,
        0,
        criteria);
}
*/
void cameraCalibrationStereo(std::vector<cv::String> imagesL, std::vector<cv::String> imagesR, std::string pathToImagesL, std::string pathToImagesR, CalibrationParametersStereo& calibrationParameters, int chessboardColCount, int chessboardRowCount, float chessboardSquareSize)
{
    cv::Size targetResolution = cv::Size(900, 600);
    std::vector<std::vector<cv::Point3f>> keyPoints;
    std::vector<std::vector<cv::Point2f>> imgPointsL;
    std::vector<std::vector<cv::Point2f>> imgPointsR;

    cv::glob(pathToImagesL, imagesL);
    cv::glob(pathToImagesR, imagesR);

    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    // Генерация 3D точек шахматной доски
    std::vector<cv::Point3f> points3D;
    for (int i{ 0 }; i < chessboardRowCount; i++)
    {
        for (int j{ 0 }; j < chessboardColCount; j++)
        {
            points3D.push_back(cv::Point3f(j * chessboardSquareSize, i * chessboardSquareSize, 0));
        }
    }

    // Загрузка изображений
    std::vector<cv::Mat> cornerImagesL;
    std::vector<cv::Mat> cornerImagesR;
    cv::Size originalSize;

    for (int i{ 0 }; i < (int)imagesL.size(); i++)
    {
        cv::Mat cornerImageL = cv::imread(imagesL[i], 1);
        cv::Mat cornerImageR = cv::imread(imagesR[i], 1);

        if (i == 0) {
            originalSize = cornerImageL.size(); // Сохраняем оригинальный размер
        }

        // Ресайз изображений
        cv::resize(cornerImageL, cornerImageL, targetResolution, 0, 0, cv::INTER_AREA);
        cv::resize(cornerImageR, cornerImageR, targetResolution, 0, 0, cv::INTER_AREA);

        cornerImagesL.push_back(cornerImageL);
        cornerImagesR.push_back(cornerImageR);
    }

    cv::Mat grayL, grayR;

    for (size_t i = 0; i < imagesL.size(); i++)
    {
        cv::cvtColor(cornerImagesL[i], grayL, cv::COLOR_BGR2GRAY);
        cv::cvtColor(cornerImagesR[i], grayR, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> cornersL;
        std::vector<cv::Point2f> cornersR;

        // Поиск углов шахматной доски
        bool cornerPositionFoundL = cv::findChessboardCorners(
            grayL,
            cv::Size(chessboardColCount, chessboardRowCount),
            cornersL,
            cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);

        bool cornerPositionFoundR = cv::findChessboardCorners(
            grayR,
            cv::Size(chessboardColCount, chessboardRowCount),
            cornersR,
            cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FILTER_QUADS);

        if (cornerPositionFoundL && cornerPositionFoundR)
        {
            cv::cornerSubPix(grayL, cornersL, cv::Size(11, 11), cv::Size(-1, -1), criteria);
            cv::cornerSubPix(grayR, cornersR, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            cv::drawChessboardCorners(cornerImagesL[i], cv::Size(chessboardColCount, chessboardRowCount), cornersL, cornerPositionFoundL);
            cv::drawChessboardCorners(cornerImagesR[i], cv::Size(chessboardColCount, chessboardRowCount), cornersR, cornerPositionFoundR);

            keyPoints.push_back(points3D);

            // Масштабирование углов обратно к исходному разрешению
            double scaleX = static_cast<double>(originalSize.width) / targetResolution.width;
            double scaleY = static_cast<double>(originalSize.height) / targetResolution.height;

            for (auto& pt : cornersL) {
                pt.x *= scaleX;
                pt.y *= scaleY;
            }
            for (auto& pt : cornersR) {
                pt.x *= scaleX;
                pt.y *= scaleY;
            }

            imgPointsL.push_back(cornersL);
            imgPointsR.push_back(cornersR);
        }
    }

    calibrationParameters.RMS = cv::stereoCalibrate(
        keyPoints,
        imgPointsL,
        imgPointsR,
        calibrationParameters.cameraMatrixL,
        calibrationParameters.distCoeffsL,
        calibrationParameters.cameraMatrixR,
        calibrationParameters.distCoeffsR,
        originalSize, // Используем оригинальный размер для калибровки
        calibrationParameters.R,
        calibrationParameters.T,
        calibrationParameters.E,
        calibrationParameters.F,
        calibrationParameters.perViewErrors,
        0,
        criteria);
}


/**
 * @brief Функция чтения параметров калибровки одиночной камеры.
 * @param fileName
 * @return
 */
CalibrationParametersMono readCalibrationParametersMono(std::string fileName)
{
    CalibrationParametersMono parameters = {};

    cv::FileStorage fileStrorage;
    fileStrorage.open(fileName, cv::FileStorage::READ);
    if (fileStrorage.isOpened()) {
        fileStrorage << "cameraMatrix" << parameters.cameraMatrix;
        fileStrorage << "distCoeffs" << parameters.distCoeffs;
        fileStrorage << "PerViewErrors" << parameters.perViewErrors;
        fileStrorage << "STDIntrinsics" << parameters.stdDevIntrinsics;
        fileStrorage << "STDExtrinsics" << parameters.stdDevExtrinsics;
        fileStrorage << "RMS" << parameters.RMS;
        fileStrorage.release();
    }
    else
    {
        std::cerr << "Ошибка при открытии файла " << fileName << std::endl;
    }

    return parameters;
}

/**
 * @brief Функция записи параметров калибровки одиночной камеры.
 * @param fileName
 * @param parameters
 */
void writeCalibrationParametersMono(std::string fileName, CalibrationParametersMono parameters)
{
    cv::FileStorage fileStrorage;
    fileStrorage.open(fileName, cv::FileStorage::WRITE);
    if (fileStrorage.isOpened())
    {
        fileStrorage << "cameraMatrix" << parameters.cameraMatrix;
        fileStrorage << "distCoeffs" << parameters.distCoeffs;
        fileStrorage << "PerViewErrors" << parameters.perViewErrors;
        fileStrorage << "STDIntrinsics" << parameters.stdDevIntrinsics;
        fileStrorage << "STDExtrinsics" << parameters.stdDevExtrinsics;
        fileStrorage << "RMS" << parameters.RMS;
        fileStrorage.release();
    }
    else
    {
        std::cerr << "Ошибка при открытии файла " << fileName << " для записи." << std::endl;
    }
}

/**
 * @brief Функция записи параметров калибровки стерео пары
 * @param fileName
 * @param parameters
 */
void writeCalibrationParametersStereo(std::string fileName, CalibrationParametersStereo parameters)
{
    cv::FileStorage fileStrorage;
    fileStrorage.open(fileName, cv::FileStorage::WRITE);
    if (fileStrorage.isOpened())
    {
        if (fileName.find(".xml") != std::string::npos)
        {
            // TODO: Имена полей должны быть одинкаовыми (.xml и .yml)
            fileStrorage << "M1" << parameters.cameraMatrixL;
            fileStrorage << "M2" << parameters.cameraMatrixR;
            fileStrorage << "D1" << parameters.distCoeffsL;
            fileStrorage << "D2" << parameters.distCoeffsR;
            fileStrorage << "R" << parameters.R;
            fileStrorage << "T" << parameters.T;
            fileStrorage << "E" << parameters.E;
            fileStrorage << "F" << parameters.F;
            fileStrorage << "rmsStereo" << parameters.RMS;
        }
        else if (fileName.find(".yml") != std::string::npos)
        {
            fileStrorage << "cameraMatrixL" << parameters.cameraMatrixL;
            fileStrorage << "cameraMatrixR" << parameters.cameraMatrixR;
            fileStrorage << "DistorsionCoeffsL" << parameters.distCoeffsL;
            fileStrorage << "DistorsionCoeffsR" << parameters.distCoeffsR;
            fileStrorage << "RotationMatrix" << parameters.R;
            fileStrorage << "TranslationMatrix" << parameters.T;
            fileStrorage << "EssentialMatrix" << parameters.E;
            fileStrorage << "FundamentalMatrix" << parameters.F;
            fileStrorage << "RMS" << parameters.RMS;
        }
        fileStrorage.release();
    }
    else
    {
        std::cerr << "Ошибка при открытии файла " << fileName << "." << std::endl;
    }
}

/**
 * @brief Функция чтения параметров калибровки стерео пары
 * @param fileName
 * @return
 */
CalibrationParametersStereo readCalibrationParametersStereo(std::string fileName)
{
    CalibrationParametersStereo parameters = {};
    cv::FileStorage fileStrorage;
    if (fileStrorage.open(fileName, cv::FileStorage::READ))
    {
        if (fileStrorage.isOpened())
        {
            if (fileName.find(".xml") != std::string::npos)
            {
                // TODO: Имена полей должны быть одинкаовыми (.xml и .yml)
                fileStrorage["M1"] >> parameters.cameraMatrixL;
                fileStrorage["M2"] >> parameters.cameraMatrixR;
                fileStrorage["D1"] >> parameters.distCoeffsL;
                fileStrorage["D2"] >> parameters.distCoeffsR;
                fileStrorage["R"] >> parameters.R;
                fileStrorage["T"] >> parameters.T;
                fileStrorage["E"] >> parameters.E;
                fileStrorage["F"] >> parameters.F;
                fileStrorage["rmsStereo"] >> parameters.RMS;
            }
            else if (fileName.find(".yml") != std::string::npos) {
                fileStrorage["cameraMatrixL"] >> parameters.cameraMatrixL;
                fileStrorage["cameraMatrixR"] >> parameters.cameraMatrixR;
                fileStrorage["DistorsionCoeffsL"] >> parameters.distCoeffsL;
                fileStrorage["DistorsionCoeffsR"] >> parameters.distCoeffsR;
                fileStrorage["RotationMatrix"] >> parameters.R;
                fileStrorage["TranslationMatrix"] >> parameters.T;
                fileStrorage["EssentialMatrix"] >> parameters.E;
                fileStrorage["FundamentalMatrix"] >> parameters.F;
                fileStrorage["RMS"] >> parameters.RMS;
            }
            fileStrorage.release();
        }
        else
        {
            std::cerr << "Ошибка при открытии файла " << fileName << std::endl;
        }
    }
    return parameters;
}

int readCalibrartionConfigFile(std::string pathToConfigFile, CalibrationConfig& config)
{
    std::ifstream file(pathToConfigFile);

    if (!file.is_open())
    {
        return 1;
    }

    // Парсинг файла с настройками процедуры калибровки
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        char eq;
        if (iss >> key >> eq)
        {
            if (eq != '=')
            {
                continue;
            }
            if (key == "image_count")
            {
                iss >> config.image_count;
            }
            else if (key == "folder_name")
            {
                iss >> config.folder_name;
                // Удаление кавычек, если они есть
                config.folder_name.erase(std::remove(config.folder_name.begin(), config.folder_name.end(), '\"'), config.folder_name.end());
            }
            else if (key == "keypoints_c")
            {
                iss >> config.keypoints_c;
            }
            else if (key == "keypoints_r")
            {
                iss >> config.keypoints_r;
            }
            else if (key == "square_size")
            {
                iss >> config.square_size;
            }
        }
    }

    file.close();

    return EXIT_SUCCESS;
}
