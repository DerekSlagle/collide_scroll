#include "xboxControl.h"

bool xboxControl::deadZoneOn = true;

bool xboxControl::UpdateControllerState()
{
 //   DWORD dwResult;
    bConnected = false;
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
    {
        // Simply get the state of the controller from XInput.
    //    dwResult = XInputGetState( i, &state );

    //    if( XInputGetState( i, &state ) == ERROR_SUCCESS )
    //    {
     //       bConnected = true;
     //       break;
     //   }
    }

    if( bConnected )
    {
        // Zero value if thumbsticks are within the dead zone
        if( deadZoneOn )
        {
            if( ( state.Gamepad.sThumbLX < INPUT_DEADZONE &&
                  state.Gamepad.sThumbLX > -INPUT_DEADZONE ) &&
                ( state.Gamepad.sThumbLY < INPUT_DEADZONE &&
                  state.Gamepad.sThumbLY > -INPUT_DEADZONE ) )
            {
                state.Gamepad.sThumbLX = 0;
                state.Gamepad.sThumbLY = 0;
            }
        }

        if( ( state.Gamepad.sThumbRX < INPUT_DEADZONE &&
              state.Gamepad.sThumbRX > -INPUT_DEADZONE ) &&
            ( state.Gamepad.sThumbRY < INPUT_DEADZONE &&
              state.Gamepad.sThumbRY > -INPUT_DEADZONE ) )
        {
            state.Gamepad.sThumbRX = 0;
            state.Gamepad.sThumbRY = 0;
        }

        thumbLx = state.Gamepad.sThumbLX/32768.0f;// 0 to 1 range
        thumbLy = state.Gamepad.sThumbLY/32768.0f;// 0 to 1 range
        thumbRx = state.Gamepad.sThumbRX/32768.0f;// 0 to 1 range
        thumbRy = state.Gamepad.sThumbRY/32768.0f;// 0 to 1 range

        WORD wButtons = state.Gamepad.wButtons;
        AdownLast = Adown; BdownLast = Bdown; SdownLast = Sdown; XdownLast = Xdown;
        Adown = wButtons & XINPUT_GAMEPAD_A;
        Bdown = wButtons & XINPUT_GAMEPAD_B;
        Sdown = wButtons & XINPUT_GAMEPAD_START;
        Xdown = wButtons & XINPUT_GAMEPAD_X;
    }

    return bConnected;
}
