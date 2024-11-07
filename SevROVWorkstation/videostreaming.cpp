#include "videostreaming.h"

VideoStreaming::VideoStreaming(int port, std::string name)
{
    // инициализация
    this->streamer.start(port);
    this->name = "/" + name;
}

VideoStreaming::~VideoStreaming()
{
    // завершение работы
    this->streamer.stop();
}

int VideoStreaming::passImageToStreamer(cv::Mat image)
{
    std::cout << "stream thread started" << std::endl;
    // Задаем качество картинки
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90}; // пока так, потом перенести куда-нибудь, или сделать настройкой
    cv::imencode(".jpg", image, this->frameToDisplay, params);
    // Выгрузка изображения в поток
    streamer.publish(this->name, std::string(this->frameToDisplay.begin(), this->frameToDisplay.end()));
    std::cout << "stream thread ended" << std::endl;
    return 0;
}
