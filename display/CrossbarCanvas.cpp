#include "CrossbarCanvas.h"

CrossbarCanvas::CrossbarCanvas(IAshitaCore* pAshitaCore, CrossbarSettings* pSettings, CrossbarBindings* pBindings)
    : pAshitaCore(pAshitaCore)
    , pBindings(pBindings)
    , pSettings(pSettings)
{
    pMainDisplay = new GdiDIB(pAshitaCore->GetDirect3DDevice(), (pSettings->pSubPanel->PanelWidth * 2) + pSettings->pSubPanel->PanelSpacing, pSettings->pSubPanel->PanelHeight);
    pMainDisplay->GetGraphics()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    pMainDisplay->GetGraphics()->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    pMainDisplay->GetGraphics()->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    pMainPrimitive = pAshitaCore->GetPrimitiveManager()->Create("CrossbarMain");
    pShoulderDisplay   = new GdiDIB(pAshitaCore->GetDirect3DDevice(), (pSettings->pSubPanel->PanelWidth * 2) + pSettings->pSubPanel->PanelSpacing, pSettings->pSubPanel->PanelHeight);
    pShoulderDisplay->GetGraphics()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    pShoulderDisplay->GetGraphics()->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    pShoulderDisplay->GetGraphics()->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    pShoulderPrimitive = pAshitaCore->GetPrimitiveManager()->Create("CrossbarShoulder");
    pSubDisplay = new GdiDIB(pAshitaCore->GetDirect3DDevice(), pSettings->pSubPanel->PanelWidth, pSettings->pSubPanel->PanelHeight);
    pSubDisplay->GetGraphics()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    pSubDisplay->GetGraphics()->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    pSubDisplay->GetGraphics()->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    pSubPrimitive = pAshitaCore->GetPrimitiveManager()->Create("CrossbarSub");
    mLastSingleMode = MacroMode::NoTrigger;
    
    uint32_t window_width = pAshitaCore->GetConfigurationManager()->GetFloat("boot", "ffxi.registry", "0001", 3840);
    uint32_t window_height  = pAshitaCore->GetConfigurationManager()->GetFloat("boot", "ffxi.registry", "0002", 2160);

    // main panel
    if (pSettings->mConfig.MainPanelX == -1)
    {
        pMainPrimitive->SetPositionX((window_width - ((pSettings->pSubPanel->PanelWidth * 2) + pSettings->pSubPanel->PanelSpacing)) / 2);
    }
    else
    {
        pMainPrimitive->SetPositionX(pSettings->mConfig.MainPanelX);
    }
    if (pSettings->mConfig.MainPanelY < 0)
    {
        pMainPrimitive->SetPositionY(window_height + pSettings->mConfig.MainPanelY - pSettings->pSubPanel->PanelHeight);
    }
    else
    {
        pMainPrimitive->SetPositionY(pSettings->mConfig.MainPanelY);
    }

    // shoulder panel
    if (pSettings->mConfig.ShoulderPanelX == -1)
    {
        pShoulderPrimitive->SetPositionX((window_width - ((pSettings->pSubPanel->PanelWidth * 2) + pSettings->pSubPanel->PanelSpacing)) / 2);
    }
    else
    {
        pShoulderPrimitive->SetPositionX(pSettings->mConfig.ShoulderPanelX);
    }
    if (pSettings->mConfig.ShoulderPanelY < 0)
    {
        pShoulderPrimitive->SetPositionY(window_height + pSettings->mConfig.ShoulderPanelY - pSettings->pSubPanel->PanelHeight);
    }
    else
    {
        pShoulderPrimitive->SetPositionY(pSettings->mConfig.ShoulderPanelY);
    }

    // sub panel
    if (pSettings->mConfig.SubPanelX == -1)
    {
        pSubPrimitive->SetPositionX((window_width - pSettings->pSubPanel->PanelWidth) / 2);
    }
    else
    {
        pSubPrimitive->SetPositionX(pSettings->mConfig.SubPanelX);
    }
    if (pSettings->mConfig.SubPanelY < 0)
    {
        pSubPrimitive->SetPositionY(window_height + pSettings->mConfig.SubPanelY - pSettings->pSubPanel->PanelHeight);
    }
    else
    {
        pSubPrimitive->SetPositionY(pSettings->mConfig.SubPanelY);
    }

    for (int x = 0; x < 9; x++)
    {
        pMacros[x] = new CrossbarMacroSet(pAshitaCore, pSettings, pBindings, (MacroMode)x);
    }
}
CrossbarCanvas::~CrossbarCanvas()
{
    pAshitaCore->GetPrimitiveManager()->Delete("CrossbarSub");
    delete pSubDisplay;
    pAshitaCore->GetPrimitiveManager()->Delete("CrossbarMain");
    delete pMainDisplay;
    pAshitaCore->GetPrimitiveManager()->Delete("CrossbarShoulder");
    delete pShoulderDisplay;
    for (int x = 0; x < 9; x++)
    {
        delete pMacros[x];
    }
}

void CrossbarCanvas::Draw(MacroMode mode)
{
    if (pBindings->mRedraw)
    {
        UpdatePalette();
        pBindings->mRedraw = false;
    }

    // if a single trigger/shoulder or no trigger is pressed, render the mainpanel and shoulderpanel
    if ((mode == MacroMode::LeftTrigger) || (mode == MacroMode::RightTrigger) || (mode == MacroMode::LeftShoulder) || (mode == MacroMode::RightShoulder) || (mode == MacroMode::NoTrigger))
    {
        pSubPrimitive->SetVisible(false);
        bool reapply = pMacros[(int)MacroMode::LeftTrigger]->Draw(pMainDisplay);
        if (pMacros[(int)MacroMode::RightTrigger]->Draw(pMainDisplay) || reapply)
        {
            pMainDisplay->ApplyToPrimitiveObject(pMainPrimitive);
        }
        pMainPrimitive->SetVisible(true);

        reapply = pMacros[(int)MacroMode::LeftShoulder]->Draw(pShoulderDisplay);
        if (pMacros[(int)MacroMode::RightShoulder]->Draw(pShoulderDisplay) || reapply)
        {
            pShoulderDisplay->ApplyToPrimitiveObject(pShoulderPrimitive);
        }
        pShoulderPrimitive->SetVisible(true);
    }
    // if a dual trigger is pressed, render the subpanel
    else
    {
        if (mLastSingleMode != mode)
        {
            pMacros[(int)mode]->ForceMacroRedraw();
            mLastSingleMode = mode;
        }

        pMainPrimitive->SetVisible(false);
        if (pMacros[(int)mode]->Draw(pSubDisplay))
        {
            pSubDisplay->ApplyToPrimitiveObject(pSubPrimitive);
        }
        pSubPrimitive->SetVisible(true);
    }
}
void CrossbarCanvas::HandleButton(MacroButton button, MacroMode mode)
{
    pMacros[(int)mode]->TriggerMacro(button);
}
void CrossbarCanvas::Hide()
{
    pMainPrimitive->SetVisible(false);
    pSubPrimitive->SetVisible(false);
    pShoulderPrimitive->SetVisible(false);
}
void CrossbarCanvas::UpdateBindings(CrossbarBindings* pNewBindings)
{
    delete pBindings;
    pBindings = pNewBindings;
    for (int x = 0; x < 9; x++)
    {
        delete pMacros[x];
        pMacros[x] = new CrossbarMacroSet(pAshitaCore, pSettings, pBindings, (MacroMode)x);
    }
}
void CrossbarCanvas::UpdatePalette()
{
    for (int x = 0; x < 9; x++)
    {
        delete pMacros[x];
        pMacros[x] = new CrossbarMacroSet(pAshitaCore, pSettings, pBindings, (MacroMode)x);
    }
}