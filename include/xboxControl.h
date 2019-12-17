#ifndef XBOXCONTROL_H_INCLUDED
#define XBOXCONTROL_H_INCLUDED

#include <XInput.h> // XInput API

// for Xbox360 controllers
#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

/*
struct CONTROLER_STATE
{
    XINPUT_STATE state;
    bool bConnected;
};  */

class xboxControl
{
    public:
    XINPUT_STATE state;
    bool bConnected;
    static bool deadZoneOn;
    bool Adown = false, Bdown = false, Sdown = false, Xdown = false;
    bool AdownLast = false, BdownLast = false, SdownLast = false, XdownLast = false;
 //   bool DPADdownLast = false, triggerDownLast = false, shoulderDownLast = false;

    float thumbLx, thumbLy, thumbRx, thumbRy;

    bool UpdateControllerState();
    bool event_A() const { return Adown && !AdownLast; }
    bool event_B() const { return Bdown && !BdownLast; }
    bool event_S() const { return Sdown && !SdownLast; }
    bool event_X() const { return Xdown && !XdownLast; }
};


#endif // XBOXCONTROL_H_INCLUDED
