#ifndef VIDEOSTREAMING_H
#define VIDEOSTREAMING_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "mjpeg_streamer.hpp"

class VideoStreaming
{
public:
    VideoStreaming(int port, std::string name);
    ~VideoStreaming();
    int passImageToStreamer(cv::Mat image);
    int updateStream();
    nadjieb::MJPEGStreamer streamer;
    cv::Mat passedImage;
private:
    std::vector<uchar> frameToDisplay; // транслируемый кадр
    std::string name;
};

#endif // VIDEOSTREAMING_H
