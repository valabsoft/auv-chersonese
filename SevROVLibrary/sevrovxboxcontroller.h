#ifndef SEVROVXBOXCONTROLLER_H
#define SEVROVXBOXCONTROLLER_H

#include <QThread>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <cstdlib>
#include "sevrovlibrary.h"

class SevROVXboxController : public QThread
{
    Q_OBJECT
public:
    SevROVXboxController(QObject *parent = nullptr);
    ~SevROVXboxController();

    void OpenJoystick(int index);
    void CloseJoystick();

    bool isRunning = true;

    QList<QString> GetJoystickList();
private:
    XboxGamepad state;

    SDL_Joystick *joystick;
    SDL_Event event;
signals:
    void OnAxisLStickX(short);
    void OnAxisLStickY(short);
    void OnAxisRStickX(short);
    void OnAxisRStickY(short);
    void OnAxisLTrigger(short);
    void OnAxisRTrigger(short);

    void OnButtonA(short);
    void OnButtonB(short);
    void OnButtonX(short);
    void OnButtonY(short);
    void OnButtonLBumper(short);
    void OnButtonRBumper(short);
    void OnButtonView(short);
    void OnButtonMenu(short);

    void OnDPad(short);
protected:
    void run() override;
};

#endif // SEVROVXBOXCONTROLLER_H
