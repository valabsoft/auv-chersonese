#ifndef SEVROVCONTROLLER_H
#define SEVROVCONTROLLER_H

#include "enumclasses.h"

class SevROVController
{
public:
    SevROVController();
    bool isConnected = false;
    CameraView cameraView = CameraView::MONO;
};

#endif // SEVROVCONTROLLER_H
