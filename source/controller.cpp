#include "controller.hpp"
#include "script_init.hpp"
#include "script_provider.hpp"
#include "script_populator.hpp"

TasController::TasController()
{
    // Device is a procon
    device.type = BIT(0);

    // Colors
    device.singleColorBody = RGBA8_MAXALPHA(61,82,242);
    device.singleColorButtons = RGBA8_MAXALPHA(80,217,203);

    // Charge is max (not working?)
    state.batteryCharge = 4;

    // Set Buttons and Joysticks
    state.buttons = 0;
    state.joysticks[JOYSTICK_LEFT].dx = 0;
    state.joysticks[JOYSTICK_LEFT].dy = 0;
    state.joysticks[JOYSTICK_RIGHT].dx = 0;
    state.joysticks[JOYSTICK_RIGHT].dy = 0;

    // Attach the controller
    Result rc = hiddbgAttachHdlsVirtualDevice(&HdlsHandle, &device);
    if (R_FAILED(rc))
        fatalSimple(rc);

    // Update the state
    rc = hiddbgSetHdlsState(HdlsHandle, &state);
    if(R_FAILED(rc))
        fatalSimple(rc);
}

TasController::~TasController()
{
    // Detatch Controller
    Result rc = hiddbgDetachHdlsVirtualDevice(HdlsHandle);
    if (R_FAILED(rc))
        fatalSimple(rc);
}

//This also resets the state of the controller after pressing so only to be used when pairing and not running a script. checker to come soon probably once scripts get implemented.
void TasController::pressA()
{
    int frames = 0;

    state.buttons = KEY_A;

    Result rc = hiddbgSetHdlsState(HdlsHandle, &state);
    if(R_FAILED(rc))
        fatalSimple(rc);

    bool loopFlag = true;

    while(loopFlag)
    {
        if(frames >= 2)
        {
            state.buttons = 0;

            Result rc = hiddbgSetHdlsState(HdlsHandle, &state);
            if(R_FAILED(rc))
                fatalSimple(rc);

            loopFlag = false;
        }

        Result rc = eventWait(&vsync_event, U64_MAX);
        if(R_FAILED(rc))
            fatalSimple(rc);

        frames++;
    }
}

void TasController::waitForVsync()
{
    Result rc = eventWait(&vsync_event, U64_MAX);
    if(R_FAILED(rc))
        fatalSimple(rc);
}
void TasController::setInputNextFrame()
{
    waitForVsync();
    Result rc = hiddbgSetHdlsState(HdlsHandle, &state);
    if(R_FAILED(rc))
        fatalSimple(rc);
}

void TasController::runMsg(std::shared_ptr<struct controlMsg> msg)
{
    state.buttons = msg->keys;
    state.joysticks[JOYSTICK_LEFT].dx = msg->joy_l_x;
    state.joysticks[JOYSTICK_LEFT].dy = msg->joy_r_x;
    state.joysticks[JOYSTICK_RIGHT].dx = msg->joy_l_y;
    state.joysticks[JOYSTICK_RIGHT].dy = msg->joy_r_y;
}
void emptyMsg()
{
    state.buttons = 0;
    state.joysticks[JOYSTICK_LEFT].dx = 0;
    state.joysticks[JOYSTICK_LEFT].dy = 0;
    state.joysticks[JOYSTICK_RIGHT].dx = 0;
    state.joysticks[JOYSTICK_RIGHT].dy = 0;
}
