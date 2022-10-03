#include "InputHandler.h"

InputHandler::InputHandler(Crossbar* pCrossbar)
	: pCrossbar(pCrossbar)
	, mConfig(InputConfig_t())
	, mLastState(InputData_t())
	, mRightShoulderFirst(false)
	, mLeftTap(false)
	, mLeftTapTimer(std::chrono::steady_clock::now())
	, mRightTap(false)
	, mRightTapTimer(std::chrono::steady_clock::now())
	, mMenuCombo(MenuComboState::Inactive)
	, mMenuTimer(std::chrono::steady_clock::now())
    , mLastActiveState(GetForegroundWindow() == pCrossbar->AshitaCore()->GetProperties()->GetFinalFantasyHwnd())
{
	
}

bool InputHandler::GetMenuActive()
{
	return pCrossbar->GetMenuActive();
}
bool InputHandler::GetGameMenuActive()
{
    return ((mConfig.AllowButtonsInMenu) && (pCrossbar->GetGameMenuActive()));
}
void InputHandler::HandleState(InputData_t input)
{
    bool activeState = (GetForegroundWindow() == pCrossbar->AshitaCore()->GetProperties()->GetFinalFantasyHwnd());
    if (!mLastActiveState && activeState)
    {
		// prevent acting on buttons that are already held down when sitching windows
        mLastActiveState = activeState;
        mLastState = input;
        return;
    }
    mLastActiveState = activeState;
    pCrossbar->SetMacroMode(GetMacroMode(input));
    HandleMenuCombo(input);
    HandleButtons(input);
    mLastState = input;
}

MacroMode InputHandler::GetMacroMode(InputData_t input)
{
	if (input.LeftTrigger)
    {
        //Only process taps and shoulder priority on a fresh hit from no triggers.
        if ((!mLastState.LeftTrigger) && (!mLastState.RightTrigger))
		{
			if (std::chrono::steady_clock::now() < mLeftTapTimer)
			{
				mLeftTap = true;
			}
			mLeftTapTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.TapDuration);
			mRightShoulderFirst = false;
		}
	}
	else
	{
		mLeftTap = false;
	}

	if (input.RightTrigger)
	{
		//Only process taps and shoulder priority on a fresh hit from no triggers.
        if ((!mLastState.LeftTrigger) && (!mLastState.RightTrigger))
		{
			if (std::chrono::steady_clock::now() < mRightTapTimer)
			{
				mRightTap = true;
			}
			mRightTapTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.TapDuration);
			mRightShoulderFirst = true;
        }
	}
	else
	{
		mRightTap = false;
	}


	if (input.LeftTrigger)
	{
		if (input.RightTrigger)
		{
			//Entering double trigger mode cancels all taps.
            mLeftTap = false;
            mRightTap     = false;
            mLeftTapTimer = std::chrono::steady_clock::now() - std::chrono::milliseconds(1);
            mRightTapTimer = std::chrono::steady_clock::now() - std::chrono::milliseconds(1);
			if ((mConfig.AllowPriority) && (mRightShoulderFirst))
				return MacroMode::BothTriggersRT;
			else
				return MacroMode::BothTriggersLT;
		}
		else if ((mConfig.AllowDoubleTap) && (mLeftTap))
			return MacroMode::LeftTriggerDT;
		else
			return MacroMode::LeftTrigger;
	}
	else if (input.RightTrigger)
	{
		if ((mConfig.AllowDoubleTap) && (mRightTap))
			return MacroMode::RightTriggerDT;
		else
			return MacroMode::RightTrigger;
	}
    else if (input.LeftShoulder)
    {
        return MacroMode::LeftShoulder;
    }
    else if (input.RightShoulder)
    {
        return MacroMode::RightShoulder;
    }
	else
	{
		return MacroMode::NoTrigger;
	}
}

void InputHandler::HandleMenuCombo(InputData_t input)
{
	if (input.LeftShoulder && input.LeftTrigger && input.RightShoulder && input.RightTrigger)
	{
		if (mMenuCombo == MenuComboState::Inactive)
		{
			mMenuCombo = MenuComboState::Waiting;
			mMenuTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.MenuDuration);
		}
		else if ((mMenuCombo == MenuComboState::Waiting) && (std::chrono::steady_clock::now() > mMenuTimer))
		{
			pCrossbar->HandleMenuCombo();
			mMenuCombo = MenuComboState::Triggered;
		}
	}
	else
	{
		mMenuCombo = MenuComboState::Inactive;
	}
}
void InputHandler::HandleButtons(InputData_t input)
{
    if ((input.Share) && (!mLastState.Share))
		pCrossbar->HandleButtonPress(MacroButton::Share);
    if ((input.Option) && (!mLastState.Option))
		pCrossbar->HandleButtonPress(MacroButton::Option);
    if ((input.LeftStickPress) && (!mLastState.LeftStickPress))
        pCrossbar->HandleButtonPress(MacroButton::LeftStickPress);
    if ((input.RightStickPress) && (!mLastState.RightStickPress))
        pCrossbar->HandleButtonPress(MacroButton::RightStickPress);
    if ((input.PlayStation) && (!mLastState.PlayStation))
        pCrossbar->HandleButtonPress(MacroButton::PlayStation);
    if ((input.TouchPadPress) && (!mLastState.TouchPadPress))
        pCrossbar->HandleButtonPress(MacroButton::TouchPadPress);
	for (int x = 0; x < 4; x++)
	{
		if ((input.Dpad[x]) && (!mLastState.Dpad[x]))
			pCrossbar->HandleButtonPress((MacroButton)x);
		if ((input.Buttons[x]) && (!mLastState.Buttons[x]))
			pCrossbar->HandleButtonPress((MacroButton)(x + 4));
	}
}
void InputHandler::LoadConfig(InputConfig_t config)
{
	mConfig = config;
}