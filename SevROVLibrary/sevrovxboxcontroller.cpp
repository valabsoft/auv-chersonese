#include "sevrovxboxcontroller.h"

SevROVXboxController::SevROVXboxController(QObject *parent)
    : QThread(parent)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) >= 0)
    {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
        SDL_JoystickEventState(SDL_ENABLE);
    }
}

SevROVXboxController::~SevROVXboxController()
{
    if ((joystick != nullptr))
    {        
        SDL_Quit();
    }
}

void SevROVXboxController::OpenJoystick(int index)
{
    joystick = SDL_JoystickOpen(index);
}

void SevROVXboxController::CloseJoystick()
{
    if (joystick != nullptr)
        SDL_JoystickClose(joystick);
}

void SevROVXboxController::run()
{
    // TODO: Проверить нужно ли добавить mutex для объекта state
    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                switch (event.jbutton.button)
                {
                case xbox_butn::A:
                    state.A = (short)event.jbutton.state;
                    emit OnButtonA(state.A);
                    break;
                case xbox_butn::B:
                    state.B = (short)event.jbutton.state;
                    emit OnButtonB(state.B);
                    break;
                case xbox_butn::X:
                    state.X = (short)event.jbutton.state;
                    emit OnButtonX(state.X);
                    break;
                case xbox_butn::Y:
                    state.Y = (short)event.jbutton.state;
                    emit OnButtonY(state.Y);
                    break;
                case xbox_butn::LBumper:
                    state.LBumper = (short)event.jbutton.state;
                    emit OnButtonLBumper(state.LBumper);
                    break;
                case xbox_butn::RBumper:
                    state.RBumper = (short)event.jbutton.state;
                    emit OnButtonRBumper(state.RBumper);
                    break;
                case xbox_butn::View:
                    state.View = (short)event.jbutton.state;
                    emit OnButtonView(state.View);
                    break;
                case xbox_butn::Menu:
                    state.Menu = (short)event.jbutton.state;
                    emit OnButtonMenu(state.Menu);
                    break;
                default:
                    // Nothing to do
                    break;
                }
                break;
            case SDL_JOYAXISMOTION:
                switch (event.jaxis.axis)
                {
                case xbox_axis::LStickX:
                    state.LStickX = (short)event.jaxis.value;
                    emit OnAxisLStickX(abs(state.LStickX) < JOYSTICK_DEAD_ZONE ? 0 : state.LStickX);
                    break;
                case xbox_axis::LStickY:
                    state.LStickY = (short)event.jaxis.value;
                    emit OnAxisLStickY(abs(state.LStickY) < JOYSTICK_DEAD_ZONE ? 0 : state.LStickY);
                    break;
                case xbox_axis::RStickX:
                    state.RStickX = (short)event.jaxis.value;
                    emit OnAxisRStickX(abs(state.RStickX) < JOYSTICK_DEAD_ZONE ? 0 : state.RStickX);
                    break;
                case xbox_axis::RStickY:
                    state.RStickY = (short)event.jaxis.value;
                    emit OnAxisRStickY(abs(state.RStickY) < JOYSTICK_DEAD_ZONE ? 0 : state.RStickY);
                    break;
                case xbox_axis::LTrigger:
                    state.LTrigger = (short)event.jaxis.value;
                    emit OnAxisLTrigger(abs(state.LTrigger) < JOYSTICK_DEAD_ZONE ? 0 : state.LTrigger);
                    break;
                case xbox_axis::RTrigger:
                    state.RTrigger = (short)event.jaxis.value;
                    emit OnAxisRTrigger(abs(state.RTrigger) < JOYSTICK_DEAD_ZONE ? 0 : state.RTrigger);
                    break;
                default:
                    // Nothing to do
                    break;
                }
                break;
            case SDL_JOYHATMOTION:
                state.DPad = (short)event.jhat.value;
                emit OnDPad(state.DPad);
                break;
            default:
                // Nothing to do
                break;
            }
        }
        // TODO: Проверить, нужно ли использовать задержку.
        // Если нужно - подобрать оптимальный интервал для задержки
        SDL_Delay(100);
    }
}

QList<QString> SevROVXboxController::GetJoystickList()
{
    QList<QString> list;

    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        SDL_Joystick *js = SDL_JoystickOpen(i);
        list.append(SDL_JoystickName(js));
        SDL_JoystickClose(js);
    }

    return list;
}
