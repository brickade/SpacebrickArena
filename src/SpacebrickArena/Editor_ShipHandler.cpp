#include "include/Editor_ShipHandler.h"

#include "TheBrick\Serializer.h"
#include "include/Spaceship.h"
#include "TheBrick\BrickInstance.h"
#include "TheBrick\Brick.h"
#include "TheBrick/GameObject.h"
#ifdef EDITOR_DEV
#include "include/ShipManager.h"
#endif

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CShipHandler::CShipHandler(int a_playerIdx)
    {
        this->m_playerIdx = a_playerIdx;
        this->m_State = ShipHandlerState::Select;
        int elements = sba_ShipManager->GetShipCount() - 1;
        /*if (elements < 0)
        {
            elements = 0;
        }*/
        this->m_pNavigation = new sba::CNavigation(1, elements);
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(0);
        this->UpdateCurrentShipData();
        this->m_pInputField = new sba::CInputField((sba::CInputField::InputTypes::Type)(sba::CInputField::InputTypes::NUMBERS | sba::CInputField::InputTypes::LETTERS_UPPERCASE | sba::CInputField::InputTypes::LETTERS_LOWERCASE | sba::CInputField::InputTypes::UNDERLINE));
        if (this->GetCurrentSpaceShip() == nullptr)
        {
            this->AddShip("");
        }
    }

    // **************************************************************************
    // **************************************************************************
    CShipHandler::~CShipHandler()
    {
        SAFE_DELETE(this->m_pInputField);
        SAFE_DELETE(this->m_pCurrentSpaceship);
        SAFE_DELETE(this->m_pNavigation);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::Update()
    {
        switch (this->m_State)
        {
        case ShipHandlerState::Select:
            { //focus initialization throws error. Dumb compiler.
                int focus = this->m_pNavigation->GetFocusedElementId();
                this->m_pNavigation->Update(*sba_Application->GetTimer(), sba_Input->Direction(sba_Direction::Navigate, this->m_playerIdx), false);
                if (this->m_pNavigation->GetFocusedElementId() != focus)
                { //Load other ship
                    SAFE_DELETE(this->m_pCurrentSpaceship);
                    this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
                    this->UpdateCurrentShipData();
                }        
            }
#ifdef EDITOR_DEV
            //Batch Rendering
            if (sba_Application->GetInput()->KeyPressed(PuRe_IInput::F7)) //qHD
            {
                sba_ShipManager->BatchRenderShip(*this->m_pCurrentSpaceship, 960, 540);
            }
            else if (sba_Application->GetInput()->KeyPressed(PuRe_IInput::F8)) //FHD
            {
                sba_ShipManager->BatchRenderShip(*this->m_pCurrentSpaceship, 1920, 1080);
            }
            else if (sba_Application->GetInput()->KeyPressed(PuRe_IInput::F9)) //UHD
            {
                sba_ShipManager->BatchRenderShip(*this->m_pCurrentSpaceship, 3840, 2160);
            }
            else if (sba_Application->GetInput()->KeyPressed(PuRe_IInput::F10)) //FUHD
            {
                sba_ShipManager->BatchRenderShip(*this->m_pCurrentSpaceship, 7680, 4320);
            }
#endif
            if (sba_Input->ButtonPressed(sba_Button::EditorAddNewShip, this->m_playerIdx))
            {
                this->AddShip("");
            }
            if (sba_Input->ButtonPressed(sba_Button::EditorRenameShip, this->m_playerIdx) && this->GetCurrentSpaceShip() != nullptr)
            {
                this->m_State = ShipHandlerState::Rename;
                this->m_OldShipName = this->m_pCurrentSpaceship->GetName();
                this->m_pInputField->SetValue(this->m_OldShipName);
            }
            if (sba_Input->ButtonPressed(sba_Button::EditorDeleteShip, this->m_playerIdx) && this->GetCurrentSpaceShip() != nullptr)
            {
                this->m_State = ShipHandlerState::Delete;
                this->m_OldShipName = this->m_pCurrentSpaceship->GetName();
                this->m_pInputField->SetValue(this->m_OldShipName);
            }
        break;
        case ShipHandlerState::Rename:
            if (this->m_pInputField->Update(*sba_Application->GetInput(), this->m_playerIdx))
            { //Close Rename
                this->m_State = ShipHandlerState::Select;
                if (this->m_pInputField->GetValue().size() == 0)
                {
                    this->m_pInputField->SetValue("GiveMeAName_Idiot");
                }
                this->m_pCurrentSpaceship->SetName(this->m_pInputField->GetValue()); //Update Name
                if (this->m_pCurrentSpaceship->GetName() != this->m_OldShipName)
                {
                    this->UpdateCurrentShipName(); //Save to disk
                }
                break;
            }
            this->m_pCurrentSpaceship->SetName(this->m_pInputField->GetValue()); //Update Name
            break;
        case ShipHandlerState::Delete:
            if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, this->m_playerIdx))
            {
                this->DeleteCurrentShip();
                this->m_State = ShipHandlerState::Select;
            }
            else if (sba_Input->ButtonPressed(sba_Button::NavigationBack, this->m_playerIdx))
            {
                this->m_State = ShipHandlerState::Select;
            }
            break;
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CShipHandler::Render(sba::CSpriteReader& a_rSpriteReader)
    {
        PuRe_Vector2F center(sba_Width / 2, sba_Height / 2);
        if (this->GetCurrentSpaceShip() == nullptr)
        {
            sba_Space->RenderFont("Add a ship", center + this->m_TextOffset + this->m_TextStep, 24);
            return;
        }
        int focus = this->m_pNavigation->GetFocusedElementId();
        //Left
        if (focus > 0)
        {
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus - 1], sba_Space->SpriteMaterial, PuRe_Vector3F(0 + this->m_OuterPreviewInset, center.Y + this->m_OuterPreviewYOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0), PuRe_Vector3F(this->m_OuterPreviewSize, this->m_OuterPreviewSize, this->m_OuterPreviewSize));
        }
        //Center
        a_rSpriteReader.Draw(1, sba_Renderer, "farb_kasten_editor", sba_Space->FontMaterial, PuRe_Vector3F(1920 / 2, 1080 / 2 - 100, 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.9f, 0.9f));
        switch (this->m_State)
        {
        case ShipHandlerState::Select:
                sba_Space->RenderFont("Slot " + std::to_string(focus + 1) + "/" + std::to_string(sba_ShipManager->GetShipCount()), center + this->m_TextOffset + this->m_TextStep * 0, 18);
                sba_Space->RenderFont("\"" + this->m_pCurrentSpaceship->GetName() + "\"", center + this->m_TextOffset + this->m_TextStep * 1, 18);
                sba_Space->RenderFont(std::to_string(this->m_pCurrentSpaceship->m_pBricks.size()) + " Bricks", center + this->m_TextOffset + this->m_TextStep * 2, 18);
                sba_Space->RenderFont(std::to_string(this->m_CurrentShipDataCache.Cockpits) + " Cockpits", center + this->m_TextOffset + this->m_TextStep * 3, 14);
                sba_Space->RenderFont(std::to_string(this->m_CurrentShipDataCache.Engines) + " Engines", center + this->m_TextOffset + this->m_TextStep * 3.5f, 14);
                sba_Space->RenderFont(std::to_string(this->m_CurrentShipDataCache.Weapons) + " Weapons", center + this->m_TextOffset + this->m_TextStep * 4, 14);
            break;
        case ShipHandlerState::Rename:
            a_rSpriteReader.Draw(1, sba_Renderer, "editor_buttons", sba_Space->FontMaterial, PuRe_Vector3F(center + this->m_TextOffset + PuRe_Vector2F(150, -45), 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.9f, 0.9f));
            sba_Space->RenderFont("\"" + this->m_pCurrentSpaceship->GetName() + "\"", center + this->m_TextOffset + this->m_TextStep * 1, 24);
            sba_Space->RenderFont("Confirm [Enter]", center + this->m_TextOffset + this->m_TextStep * 3, 18);
            sba_Space->RenderFont("Cancel [Esc]", center + this->m_TextOffset + this->m_TextStep * 3.5f, 18);
            break;
        case ShipHandlerState::Delete:
            sba_Space->RenderFont("Kill me?", center + this->m_TextOffset + this->m_TextStep * 1, 24);
            sba_Space->RenderFont("Confirm [Enter][A]", center + this->m_TextOffset + this->m_TextStep * 3, 18);
            sba_Space->RenderFont("Cancel [Backspace][B]", center + this->m_TextOffset + this->m_TextStep * 3.5f, 18);
            break;
        }
        sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus], sba_Space->SpriteMaterial, PuRe_Vector3F(center + this->m_PreviewOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0));
        //Right
        if (focus < this->m_pNavigation->GetLastElementId())
        {
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus + 1], sba_Space->SpriteMaterial, PuRe_Vector3F(sba_Width - this->m_OuterPreviewInset, center.Y + this->m_OuterPreviewYOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0), PuRe_Vector3F(this->m_OuterPreviewSize, this->m_OuterPreviewSize, this->m_OuterPreviewSize));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::AddShip(const char* a_pName)
    {
        SAFE_DELETE(this->m_pCurrentSpaceship);
        sba_ShipManager->AddNewShip(a_pName);
        this->m_pNavigation->AddElement();
        this->m_pNavigation->SetFocusedElementId(this->m_pNavigation->GetLastElementId());
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
        this->m_State = ShipHandlerState::Rename;
        assert(this->m_pCurrentSpaceship != nullptr);
        this->m_OldShipName = this->m_pCurrentSpaceship->GetName();
        this->m_pInputField->SetValue(this->m_OldShipName);
        this->UpdateCurrentShipData();
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::SaveCurrentShip()
    {
        sba_ShipManager->SaveShipToFile(*this->GetCurrentSpaceShip());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::ResetCurrentShip()
    {
        sba_ShipManager->ResetShip(*this->GetCurrentSpaceShip());
        this->UpdateCurrentShipData();
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::DeleteCurrentShip()
    {
        sba_ShipManager->DeleteShip(*this->GetCurrentSpaceShip());
        this->m_pNavigation->RemoveElement(this->m_pNavigation->GetFocusedElementId());
        SAFE_DELETE(this->m_pCurrentSpaceship);
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
        this->UpdateCurrentShipData();
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrickInstance* CShipHandler::AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate)
    {
        TheBrick::CBrickInstance* brickInstance = a_pTemplate.m_pBrick->CreateInstance(*this->GetCurrentSpaceShip(), *sba_World);
        brickInstance->SetTransform(a_pTemplate.GetTransform());
        brickInstance->m_Color = a_pTemplate.m_Color;
        return brickInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::UpdateCurrentShipPreview()
    {
        sba_ShipManager->UpdateSprite(*this->GetCurrentSpaceShip());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::UpdateCurrentShipName()
    {
        sba_ShipManager->UpdateShipName(*this->GetCurrentSpaceShip(), this->m_OldShipName);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::UpdateCurrentShipData()
    {
        if (m_pCurrentSpaceship == nullptr)
        {
            memset(&m_CurrentShipDataCache, 0, sizeof(ShipDataCache));
            return;
        }
        std::vector<TheBrick::CBrickInstance**> brickCache;
        this->m_pCurrentSpaceship->GetCockpits(brickCache);
        this->m_CurrentShipDataCache.Cockpits = brickCache.size();
        brickCache.clear();
        this->m_pCurrentSpaceship->GetEngines(brickCache);
        this->m_CurrentShipDataCache.Engines = brickCache.size();
        brickCache.clear();
        this->m_pCurrentSpaceship->GetWeapons(brickCache);
        this->m_CurrentShipDataCache.Weapons = brickCache.size();
        //brickCache.clear();
    }
}