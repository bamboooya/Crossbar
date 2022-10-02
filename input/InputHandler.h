#ifndef __ASHITA_CrossbarInputHandler_H_INCLUDED__
#define __ASHITA_CrossbarInputHandler_H_INCLUDED__
#include "../Crossbar.h"
#include "../config/CrossbarSettings.h"

struct InputData_t
{
    bool LeftTrigger;
    bool RightTrigger;
    bool LeftShoulder;
    bool RightShoulder;
    bool Dpad[4];
    bool Buttons[4];
    bool Share;
    bool Option;
    bool LeftStickPress;
    bool RightStickPress;
    bool PlayStation;
    bool TouchPadPress;

    InputData_t()
        : LeftTrigger(false)
        , RightTrigger(false)
        , LeftShoulder(false)
        , RightShoulder(false)
        , Share(false)
        , Option(false)
        , LeftStickPress(false)
        , RightStickPress(false)
        , PlayStation(false)
        , TouchPadPress(false)
    {
        for (int x = 0; x < 4; x++)
        {
            Dpad[x] = false;
            Buttons[x] = false;
        }
    }
};

enum class MenuComboState
{
    Inactive = 0,
    Waiting = 1,
    Triggered = 2
};

class InputHandler
{
private:
    Crossbar* pCrossbar;
    InputConfig_t mConfig;

    //State
    InputData_t mLastState;
    bool mRightShoulderFirst;
    bool mLeftTap;
    std::chrono::steady_clock::time_point mLeftTapTimer;
    bool mRightTap;
    std::chrono::steady_clock::time_point mRightTapTimer;
    MenuComboState mMenuCombo;
    std::chrono::steady_clock::time_point mMenuTimer;
    std::chrono::steady_clock::time_point mActiveTimer;
    bool mLastActiveState;

public:
    InputHandler(Crossbar* pCrossbar);

    bool GetMenuActive();
    bool GetGameMenuActive();
    void HandleButtons(InputData_t input);
    void HandleMenuCombo(InputData_t input);
    void HandleState(InputData_t input);
    MacroMode GetMacroMode(InputData_t input);

    void LoadConfig(InputConfig_t config);
};

#endif