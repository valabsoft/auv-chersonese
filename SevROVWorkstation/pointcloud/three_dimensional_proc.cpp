#include "three_dimensional_proc.h"

std::vector<std::vector<double>> point3d_finder(cv::Mat imageL, cv::Mat imageR, stereo_output_par_t calib_par, const stereo_sgbm_t SGBM_par){

    // Стереоректификация изображений
    cv::Mat mapLx, mapLy, mapRx, mapRy;
    cv::Mat Q, R1, R2, P1, P2;

    // Перевод изображений из цветного формата в монохромный
    cv::Mat grayImageLeft = imageL;
    cv::Mat grayImageRight = imageR;
    cv::cvtColor(imageL, grayImageLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageR, grayImageRight, cv::COLOR_BGR2GRAY);

    //cv::Size targetSize = cv::Size(1280, 1024);

    // Ректификация и устранение искажений
    cv::stereoRectify(calib_par.cameraM1, calib_par.distCoeffs1, calib_par.cameraM2, calib_par.distCoeffs2,
                      grayImageLeft.size(), calib_par.R, calib_par.T, R1, R2, P1, P2, Q, cv::CALIB_ZERO_DISPARITY);

    cv::initUndistortRectifyMap(calib_par.cameraM1, calib_par.distCoeffs1, R1, P1,
                                grayImageLeft.size(), CV_32FC1, mapLx, mapLy);
    cv::initUndistortRectifyMap(calib_par.cameraM2, calib_par.distCoeffs2, R2, P2,
                                grayImageLeft.size(), CV_32FC1, mapRx, mapRy);

    cv::Mat rectifiedLeft, rectifiedRight;
    cv::remap(grayImageLeft, rectifiedLeft, mapLx, mapLy, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);
    cv::remap(grayImageRight, rectifiedRight, mapRx, mapRy, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);

    // Рассчёт карты диспаратности методом SGBM
    cv::Mat disparity;

    cv::Ptr<cv::StereoSGBM> stereo = cv::StereoSGBM::create();
    int cn = imageL.channels();
    stereo->setNumDisparities(SGBM_par.numDisparities);
    stereo->setBlockSize(SGBM_par.blockSize);
    stereo->setMinDisparity(SGBM_par.minDisparity);
    stereo->setPreFilterCap(SGBM_par.preFilterCap);
    stereo->setUniquenessRatio(SGBM_par.uniquenessRatio);
    stereo->setSpeckleWindowSize(SGBM_par.speckleWindowSize);
    stereo->setSpeckleRange(SGBM_par.speckleRange);
    stereo->setDisp12MaxDiff(SGBM_par.disp12MaxDiff);
    stereo->setP1(8*cn*SGBM_par.P1_*SGBM_par.P1_);
    stereo->setP1(32*cn*SGBM_par.P2_*SGBM_par.P2_);
    stereo->setMode(SGBM_par.mode);

    stereo_d_map(rectifiedLeft, rectifiedRight, disparity, stereo);

    // Поиск 3д точек
    cv::Mat points23D;
    cv::reprojectImageTo3D(disparity, points23D, Q, false, CV_32F);

    std::vector<std::vector<double>> result;
    std::vector<double> limit_outlierArea {-5.0e3, -5.0e3, 250, 5.0e3, 5.0e3, 5.0e3};
    const double minDistance = 100.0;
    std::deque<std::vector<double>> lastPoints;

    // Лямбда-фукнция для проверки входа 3д-точки в рабочую область камеры
    auto isValidPoint = [&](const cv::Vec3f& point) -> bool {
        return point[0] >= limit_outlierArea[0] && point[0] <= limit_outlierArea[3] &&
               point[1] >= limit_outlierArea[1] && point[1] <= limit_outlierArea[4] &&
               point[2] >= limit_outlierArea[2] && point[2] <= limit_outlierArea[5];
    };

    // Лямбда-функция для нахождения эвклидового расстояния между соседними точками
    auto calculateDistance = [](const std::vector<double>& point1, const std::vector<double>& point2) -> double {
        double dx = point1[2] - point2[2]; // X
        double dy = point1[3] - point2[3]; // Y
        double dz = point1[4] - point2[4]; // Z
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    };

    for (int v = 0; v < points23D.rows; v++) {
        for (int u = 0; u < points23D.cols; u++) {
            cv::Vec3f xyz3D = points23D.at<cv::Vec3f>(v, u);

            // Фильтрация по границам рабочей области
            if (!isValidPoint(xyz3D)) continue;

            // Добавление новой точки
            std::vector<double> newPoint = {static_cast<double>(u), static_cast<double>(v),
                                            static_cast<double>(xyz3D[0]),
                                            static_cast<double>(xyz3D[1]),
                                            static_cast<double>(xyz3D[2])};

            // Проверка расстояния только до последних 10 добавленных точек
            bool isFarEnough = true;
            for (const auto& existingPoint : lastPoints) {
                if (calculateDistance(newPoint, existingPoint) < minDistance) {
                    isFarEnough = false;
                    break;
                }
            }

            // Добавление точки в итоговый массив, если она достаточно удалена от соседних
            if (isFarEnough) {
                result.push_back(newPoint);

                // Добавление точки в промежуточный буффер для сравнения
                lastPoints.push_back(newPoint);

                // "Вытисниение" последней точки из буффера сравнения при добавления новой
                if (lastPoints.size() > 10) {
                    lastPoints.pop_front();
                }
            }
        }
    }
    return result;
}


cv::Vec3f third_coords(cv::Mat imageL, cv::Mat imageR, cv::Point xy, stereo_output_par_t calib_par) {

    // Перевод изображений из цветного формата в монохромный
    cv::Mat rectifiedLeft, rectifiedRight;
    cv::Mat grayImageLeft, grayImageRight;
    cv::cvtColor(imageL, grayImageLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageR, grayImageRight, cv::COLOR_BGR2GRAY);

    // Стереоректификация изображений
    cv::Mat mapLx, mapLy, mapRx, mapRy;
    cv::Mat Q, R1, R2, P1, P2;

    cv::stereoRectify(calib_par.cameraM1, calib_par.distCoeffs1, calib_par.cameraM2, calib_par.distCoeffs2,
                      cv::Size(grayImageLeft.cols, grayImageLeft.rows), calib_par.R, calib_par.T, R1, R2, P1, P2, Q, cv::CALIB_ZERO_DISPARITY);

    cv::initUndistortRectifyMap(calib_par.cameraM1, calib_par.distCoeffs1, R1, P1,
                                cv::Size(imageL.cols, imageL.rows), CV_32FC1, mapLx, mapLy);

    cv::initUndistortRectifyMap(calib_par.cameraM2, calib_par.distCoeffs2, R2, P2,
                                cv::Size(imageR.cols, imageR.rows), CV_32FC1, mapRx, mapRy);


    // Ректификация и устранение искажений
    cv::remap(grayImageLeft, rectifiedLeft, mapLx, mapLy, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);
    cv::remap(grayImageRight, rectifiedRight, mapRx, mapRy, cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);


    // Рассчёт карты диспаратности методом SGBM
    stereo_sgbm_t SGBM_par;
    cv::Mat disparity;

    cv::Ptr<cv::StereoSGBM> stereo = cv::StereoSGBM::create();
    int cn = imageL.channels();
    stereo->setNumDisparities(SGBM_par.numDisparities*64);
    stereo->setBlockSize(SGBM_par.blockSize);
    stereo->setMinDisparity(SGBM_par.minDisparity);
    stereo->setPreFilterCap(SGBM_par.preFilterCap);
    stereo->setUniquenessRatio(SGBM_par.uniquenessRatio);
    stereo->setSpeckleWindowSize(SGBM_par.speckleWindowSize);
    stereo->setSpeckleRange(SGBM_par.speckleRange);
    stereo->setDisp12MaxDiff(SGBM_par.disp12MaxDiff);
    stereo->setP1(8*cn*SGBM_par.P1_*SGBM_par.P1_);
    stereo->setP1(32*cn*SGBM_par.P2_*SGBM_par.P2_);

    stereo_d_map(imageL, imageR, disparity, stereo);


    cv::Mat coords3d;

    // Поиск 3д точек
    cv::reprojectImageTo3D(disparity, coords3d, Q, true, CV_32F);

    if (xy.x >= 0 && xy.x < coords3d.cols && xy.y >= 0 && xy.y < coords3d.rows ) {
        cv::Vec3f point3D = coords3d.at<cv::Vec3f>(xy);
        if (point3D[2] != 0 && point3D[2] < 10000) {
            return point3D;
        }
    }

    return cv::Vec3f(0, 0, 0);
}

void write_coords_file(std::vector<std::vector<double>> points23D,std::string filename){

    std::vector<double> limit_outlierArea {-8.0e3, -8.0e3, 250, 8.0e3, 8.0e3, 15.20e3};

    std::ofstream xyz_fs;
    xyz_fs.open(filename);

    int u,v;
    double x,y,z;

    if (xyz_fs.is_open()) {
        for(int i = 0; i < (int)points23D.size(); i++)
        {

            v = points23D[i][0];
            u = points23D[i][1];

            x = points23D[i][2];
            y = points23D[i][3];
            z = points23D[i][4];

            xyz_fs << v <<"\t"<< u << "\t" << " \t" << x << "\t" << y << "\t" << z<< std::endl;
        }

        xyz_fs.close();

    } else {
        std::cerr << "Error while reading the " << filename << "." << std::endl;
    }
}
