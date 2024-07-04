#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "calibrate.h"
#include "disparity.h"
#include "three_dimensional_proc.h"


using namespace std;
using namespace cv;
using namespace cuda;


/// РћР±СЉСЏРІР»РµРЅРёРµ РїРµСЂРµРјРµРЅРЅС‹С… (РІСЂРµРјРµРЅРЅРѕ)
/// \todo РЈР±СЂР°С‚СЊ РіР»РѕР±Р°Р»СЊРЅС‹Рµ РїРµСЂРµРјРµРЅРЅС‹Рµ, СЃРґРµР»Р°С‚СЊ РёС… РїРµСЂРµРґР°С‡Сѓ РєР°Рє РїР°СЂР°РјРµС‚СЂРѕРІ
cv::Mat rectifiedLeft, rectifiedRight, imageLeft, imageRight;
cv::Mat disparity;
int click_counter = 0;

int mode = 1; // РЎРјРµРЅР° Р°Р»РіРѕСЂРёС‚РјРѕРІ Stereo


/// РћР±СЉСЏРІР»РµРЅРёРµ СЃС‚СЂСѓРєС‚СѓСЂ РґР»СЏ С…СЂР°РЅРµРЅРёСЏ РїР°СЂР°РјРµС‚СЂРѕРІ РєР°Р»РёР±СЂРѕРІРєРё
stereo_output_par_t stereo_par;

struct MouseCallbackData {
    //std::vector<KeyPoint> keypointsLeft;
    vector<vector<int>> leftKey;
    std::vector<Point2f> rightKey;
    cv::Mat disparity;
    cv::Mat depth;
    vector<vector<double>> points3D;
    double focalLenght;
    double baseline;
};


void onMouseClick(int event, int x, int y, int flags, void* userdata){
    cv::Mat* coords = static_cast<cv::Mat*>(userdata);

    if (event == cv::EVENT_LBUTTONDOWN){
        cv::Point clickPoint(x,y);
        //std::cout << third_coords(imageLeft, imageRight, clickPoint, *coords) << std::endl;
    }
}


int main(int argc, char** argv) {
    mono_output_par_t mono_parL;
    mono_output_par_t mono_parR;

    MouseCallbackData callbackData;

    std::vector<cv::String> imagesL, imagesR;   // РџРµСЂРµРјРµРЅРЅС‹Рµ, СЃРѕРґРµСЂР¶Р°С‰РёРµ РЅР°Р·РІР°РЅРёСЏ РёР·РѕР±СЂР°Р¶РµРЅРёР№ РІ РґР°С‚Р°СЃРµС‚Р°С…
    std::string pathL, pathR;                   // РџРµСЂРµРјРµРЅРЅС‹Рµ, СЃРѕРґРµСЂР¶Р°С‰РёРµ РїСѓС‚Рё Рє РґР°С‚Р°СЃРµС‚Р°Рј
    unsigned int num_set = 6;                   // РќРѕРјРµСЂ С‚РµРєСѓС‰РµРіРѕ РёСЃРїРѕР»СЊР·СѓРµРјРѕРіРѕ РєР°Р»РёР±СЂРѕРІРѕС‡РЅРѕРіРѕ РґР°С‚Р°СЃРµС‚Р°
    unsigned int num_set_stereo = 7;            // РќРѕРјРµСЂ С‚РµРєСѓС‰РµРіРѕ РёСЃРїРѕР»СЊР·СѓРµРјРѕРіРѕ С‚РµСЃС‚РѕРІРѕРіРѕ РґР°С‚Р°СЃРµС‚Р°
    int checkerboard_c;                         // Р§РёСЃР»Рѕ РєР»СЋС‡РµРІС‹С… С‚РѕС‡РµРє РїРѕ СЃС‚РѕР»Р±С†Р°Рј
    int checkerboard_r;                         // Р§РёСЃР»Рѕ РєР»СЋС‡РµРІС‹С… С‚РѕС‡РµРє РїРѕ СЃС‚СЂРѕРєР°Рј
    float square_size = 20.1;                   // Р Р°Р·РјРµСЂ РєРІР°РґСЂР°С‚Р° РєР°Р»РёР±СЂРѕРІРѕС‡РЅРѕР№ РґРѕСЃРєРё РІ РјРј
    std::string name;                           // РќР°РёРјРµРЅРѕРІР°РЅРёРµ РґР°С‚Р°СЃРµС‚Р° (Рё yml-С„Р°Р№Р»Р°)
    bool isCalibrate = false;                   // Р¤Р»Р°Рі РїСЂРёРЅСѓРґРёС‚РµР»СЊРЅРѕР№ РєР°Р»РёР±СЂРѕРІРєРё

    // Р’С‹Р±РѕСЂ РґР°С‚Р°СЃРµС‚Р° РґР»СЏ РєР°Р»РёР±СЂРѕРІРєРё
    switch (num_set){
    case 0:
        pathL = "../../Fotoset/T_rep/left";
        pathR = "../../Fotoset/T_rep/right";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "0";
        break;
    case 1:
        pathL = "../../Fotoset/lab_set/left";
        pathR = "../../Fotoset/lab_set/right";
        checkerboard_c = 7;
        checkerboard_r = 4;
        name = "1";
        break;
    case 2:
        checkerboard_c = 9;
        checkerboard_r = 6;
        pathL = "../../Fotoset/basler_2_png/left";
        pathR = "../../Fotoset/basler_2_png/right";
        name = "2";
        break;
    case 3:
        pathL = "../../Fotoset/dataset_res/left";
        pathR = "../../Fotoset/dataset_res/right";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "3";
        break;
    case 4:
        pathL = "../../Fotoset/basler_festo/left";
        pathR = "../../Fotoset/basler_festo/right";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "4";
        break;
    case 5:
        pathL = "../../Fotoset/pairs/left";
        pathR = "../../Fotoset/pairs/right";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "5";
        break;
    case 6:
        pathL = "../../Fotoset/FEDOR/L";
        pathR = "../../Fotoset/FEDOR/R";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "6";
        break;
    default:
        pathL = "../../Fotoset/basler_festo/left";
        pathR = "../../Fotoset/basler_festo/right";
        checkerboard_c = 9;
        checkerboard_r = 6;
        name = "7";
        break;
    }

    // РљР°Р»РёР±СЂРѕРІРєР° СЃС‚РµСЂРµРѕРїР°СЂС‹
    cv::FileStorage stereo_fs;
    if (stereo_fs.open("../../Calibration_parameters(stereo)/A" + name + "_stereo_camera_parameters.yml", cv::FileStorage::READ) && (!isCalibrate)){
        if (stereo_fs.isOpened()){
            stereo_fs["cameraMatrixL"]              >> stereo_par.cameraM1;
            stereo_fs["cameraMatrixR"]              >> stereo_par.cameraM2;
            stereo_fs["DistorsionCoeffsL"]          >> stereo_par.distCoeffs1;
            stereo_fs["DistorsionCoeffsR"]          >> stereo_par.distCoeffs2;
            stereo_fs["RotationMatrix"]             >> stereo_par.R;
            stereo_fs["TranslationMatrix"]          >> stereo_par.T;
            stereo_fs["EssentialMatrix"]            >> stereo_par.E;
            stereo_fs["FundamentalMatrix"]          >> stereo_par.F;
            stereo_fs["VectorOfRotationVectors"]    >> stereo_par.rvecs;
            stereo_fs["VectorOfTranslationVectors"] >> stereo_par.tvecs;
            stereo_fs["PerViewErrors"]              >> stereo_par.perViewErrors;
            stereo_fs["RMS"]                        >> stereo_par.RMS;
            stereo_fs.release();
        }
    } else {
        cout << "Stereo calibration procedure is running..." << endl;
        calibrate_with_mono(imagesL,imagesR, pathL, pathR, mono_parL, mono_parR, stereo_par, checkerboard_c, checkerboard_r,square_size);
    }

    // Р’С‹РІРѕРґ РїР°СЂР°РјРµС‚СЂРѕРІ СЃС‚РµСЂРµРѕРїР°СЂС‹
    print_stereo_camera_parameters(stereo_par);

    // Р—Р°РіСЂСѓР·РєР° С‚РµСЃС‚РѕРІС‹С… Р»РµРІРѕРіРѕ Рё РїСЂР°РІРѕРіРѕ РёР·РѕР±СЂР°Р¶РµРЅРёР№
    //cv::Mat imageLeft, imageRight;
    switch(num_set_stereo){
    case 0:
        imageLeft = cv::imread("../../Fotoset/Stereo/tests/left/L88.png");
        imageRight = cv::imread("../../Fotoset/Stereo/tests/right/R88.png");
        break;
    case 1:
        imageLeft = cv::imread("../../Fotoset/Stereo/tests/tele/left/R87.png");
        imageRight = cv::imread("../../Fotoset/Stereo/tests/tele/right/L87.png");
        break;
    case 2:
        imageLeft = cv::imread("../../Fotoset/Stereo/tests/any/view0.png");
        imageRight = cv::imread("../../Fotoset/Stereo/tests/any/view1.png");
        break;
    case 3:
        imageLeft = cv::imread("../../Fotoset/lab_set/left/left_var9_7.png");
        imageRight = cv::imread("../../Fotoset/lab_set/right/right_var9_7.png");
        break;
    case 4:
        imageLeft = cv::imread("../../Fotoset/Stereo/basler_festo/stereo_test/left/L1.png");
        imageRight = cv::imread("../../Fotoset/Stereo/basler_festo/stereo_test/right/R1.png");
        break;
    case 5:
        imageLeft = cv::imread("../../Fotoset/basler_2_png/test/left/LIm1.png");
        imageRight = cv::imread("../../Fotoset/basler_2_png/test/right/RIm1.png");
        break;
    case 6:
        imageLeft = cv::imread("../../Fotoset/Stereo/tests/fish/photo_left.png");
        imageRight = cv::imread("../../Fotoset/Stereo/tests/fish/photo_right.png");
        break;
    case 7:
        imageLeft = cv::imread("../../Fotoset/FEDOR/L_27_4_37.jpg");
        imageRight = cv::imread("../../Fotoset/FEDOR/R_27_4_37.jpg");
        break;
    default:
        break;
    }


    cv::Mat pointsAll;
    //point3d_finder(imageLeft, imageRight, pointsAll);


    //cv::cvtColor(imageLeft, imageLeft, cv::COLOR_GRAY2BGR);

    cv::imshow("Res", imageLeft);
    cv::setMouseCallback("Res", onMouseClick, &pointsAll);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}








