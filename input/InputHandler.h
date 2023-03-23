#ifndef __ASHITA_CrossbarInputHandler_H_INCLUDED__
#define __ASHITA_CrossbarInputHandler_H_INCLUDED__
#include "../Crossbar.h"
#include "../config/CrossbarSettings.h"

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
    bool mRightTriggerFirst;
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