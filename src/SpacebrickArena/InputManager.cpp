#include "include/InputManager.h"

#include <TheBrick/Serializer.h>
#include "include/Space.h"

namespace sba
{
    const float CInputManager::DEFAULT_GAMEPAD_THRESHOLD = 0.25f;

    // **************************************************************************
    // **************************************************************************
    CInputManager::CInputManager()
    {
        this->m_LastInputWasGamepad = false;
        this->m_LastGamepadDetection = 0;
        this->m_ConnectedGamepadsCount = 0;
    }

    // **************************************************************************
    // **************************************************************************
    CInputManager::~CInputManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Initialize()
    {
        this->m_pInput = sba_Application->GetInput();
        //this->Load("input.data");
        this->Reset();
        this->DetectGamepads();
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Reset()
    {
        //Direction
        memset(&this->m_pDirectionMapping, 0, sizeof(this->m_pDirectionMapping));
        this->m_pDirectionMapping[Input::EDirection::Navigate] =
        {
            Input::EGamepadDirection::DPad,
            Input::EKeyboardDirection::ArrowKeys
        };
        this->m_pDirectionMapping[Input::EDirection::EditorCameraRotate] =
        {
            Input::EGamepadDirection::RightThumb,
            Input::EKeyboardDirection::Mouse

        };
        this->m_pDirectionMapping[Input::EDirection::EditorMoveBrick] =
        {
            Input::EGamepadDirection::LeftThumb,
            Input::EKeyboardDirection::Mouse

        };
        this->m_pDirectionMapping[Input::EDirection::EditorChooseElement] =
        {
            Input::EGamepadDirection::DPad,
            Input::EKeyboardDirection::WSAD

        };
        this->m_pDirectionMapping[Input::EDirection::GameMove_1] =
        {
            Input::EGamepadDirection::RightThumb,
            Input::EKeyboardDirection::Mouse

        };
        this->m_pDirectionMapping[Input::EDirection::GameMove_2] =
        {
            Input::EGamepadDirection::LeftThumb,
            Input::EKeyboardDirection::Mouse

        };

        //Axis
        memset(&this->m_pAxisMapping, 0, sizeof(this->m_pAxisMapping));
        this->m_pAxisMapping[Input::EAxis::EditorCameraZoom] =
        {
            Input::EGamepadAxis::BothTrigger,
            Input::EKeyboardAxis::MouseScroll

        };
        this->m_pAxisMapping[Input::EAxis::EditorColorSlider] =
        {
            Input::EGamepadAxis::BothTrigger,
            Input::EKeyboardAxis::MouseScroll

        };
        this->m_pAxisMapping[Input::EAxis::GameShoot] =
        {
            Input::EGamepadAxis::RightTrigger,
            Input::EKeyboardAxis::MouseScroll

        };
        this->m_pAxisMapping[Input::EAxis::GamePauseSwitch] =
        {
            Input::EGamepadAxis::DPadY,
            Input::EKeyboardAxis::ArrowUD

        };
        this->m_pAxisMapping[Input::EAxis::GameThrust_1] =
        {
            Input::EGamepadAxis::LeftTrigger,
            Input::EKeyboardAxis::WS

        };
        this->m_pAxisMapping[Input::EAxis::GameThrust_2] =
        {
            Input::EGamepadAxis::RightThumbY,
            Input::EKeyboardAxis::WS,
        };
        this->m_pAxisMapping[Input::EAxis::GameThrust_3] =
        {
            Input::EGamepadAxis::LeftThumbY,
            Input::EKeyboardAxis::WS
        };
        this->m_pAxisMapping[Input::EAxis::GameSpin_1] =
        {
            Input::EGamepadAxis::LeftThumbX,
            Input::EKeyboardAxis::AD
        };
        this->m_pAxisMapping[Input::EAxis::GameSpin_2] =
        {
            Input::EGamepadAxis::RightThumbX,
            Input::EKeyboardAxis::AD
        };

        //Button
        memset(&this->m_pButtonMapping, 0, sizeof(this->m_pButtonMapping));
        this->m_pButtonMapping[Input::EButton::EditorPlaceBrick] =
        {
            Input::EGamepadButton::Pad_A,
            Input::EKeyboardButton::MouseLeft
        };
        this->m_pButtonMapping[Input::EButton::EditorDeleteBrick] =
        {
            Input::EGamepadButton::Pad_A,
            Input::EKeyboardButton::MouseLeft
        };
        this->m_pButtonMapping[Input::EButton::EditorTogglePlacementSide] =
        {
            Input::EGamepadButton::Pad_B,
            Input::EKeyboardButton::V
        };
        this->m_pButtonMapping[Input::EButton::EditorUndoRedoHold] =
        {
            Input::EGamepadButton::Pad_X,
            Input::EKeyboardButton::Ctrl
        };
        this->m_pButtonMapping[Input::EButton::EditorUndo] =
        {
            Input::EGamepadButton::DPAD_Left,
            Input::EKeyboardButton::Z
        };
        this->m_pButtonMapping[Input::EButton::EditorRedo] =
        {
            Input::EGamepadButton::DPAD_Right,
            Input::EKeyboardButton::Y
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickRight] =
        {
            Input::EGamepadButton::Right_Shoulder,
            Input::EKeyboardButton::E
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickLeft] =
        {
            Input::EGamepadButton::Left_Shoulder,
            Input::EKeyboardButton::Q
        };
        this->m_pButtonMapping[Input::EButton::EditorResetShip] =
        {
            Input::EGamepadButton::Left_Thumb,
            Input::EKeyboardButton::Delete
        };
        this->m_pButtonMapping[Input::EButton::EditorSaveShip] =
        {
            Input::EGamepadButton::Pad_Start,
            Input::EKeyboardButton::CtrlS
        };
        this->m_pButtonMapping[Input::EButton::EditorToggleNavigateCategories] =
        {
            Input::EGamepadButton::Right_Thumb,
            Input::EKeyboardButton::X
        };
        this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive] =
        {
            Input::EGamepadButton::Right_Thumb,
            Input::EKeyboardButton::MouseRight
        };
        this->m_pButtonMapping[Input::EButton::EditorFadeHold] =
        {
            Input::EGamepadButton::Pad_Y,
            Input::EKeyboardButton::Shift
        };
        this->m_pButtonMapping[Input::EButton::EditorFadeSupervisor] =
        {
            Input::EGamepadButton::DPAD_Left,
            Input::EKeyboardButton::A
        };
        this->m_pButtonMapping[Input::EButton::EditorFadeColors] =
        {
            Input::EGamepadButton::DPAD_Down,
            Input::EKeyboardButton::S
        };
        this->m_pButtonMapping[Input::EButton::EditorFadeMenu] =
        {
            Input::EGamepadButton::DPAD_Right,
            Input::EKeyboardButton::D
        };
        this->m_pButtonMapping[Input::EButton::EditorFadeDummy] =
        {
            Input::EGamepadButton::DPAD_Up,
            Input::EKeyboardButton::W
        };
        this->m_pButtonMapping[Input::EButton::EditorRenameShip] =
        {
            Input::EGamepadButton::Pad_Y,
            Input::EKeyboardButton::F2
        };
        this->m_pButtonMapping[Input::EButton::EditorAddNewShip] =
        {
            Input::EGamepadButton::Pad_Start,
            Input::EKeyboardButton::Insert
        };
        this->m_pButtonMapping[Input::EButton::EditorDeleteShip] =
        {
            Input::EGamepadButton::Pad_X,
            Input::EKeyboardButton::Delete
        };
        this->m_pButtonMapping[Input::EButton::NavigationSelect] =
        {
            Input::EGamepadButton::Pad_A,
            Input::EKeyboardButton::Enter
        };
        this->m_pButtonMapping[Input::EButton::NavigationBack] =
        {
            Input::EGamepadButton::Pad_B,
            Input::EKeyboardButton::Backspace
        };
        this->m_pButtonMapping[Input::EButton::MenuLeft] =
        {
            Input::EGamepadButton::DPAD_Left,
            Input::EKeyboardButton::Left
        };
        this->m_pButtonMapping[Input::EButton::MenuRight] =
        {
            Input::EGamepadButton::DPAD_Right,
            Input::EKeyboardButton::Right
        };
        this->m_pButtonMapping[Input::EButton::MenuUp] =
        {
            Input::EGamepadButton::DPAD_Up,
            Input::EKeyboardButton::Up
        };
        this->m_pButtonMapping[Input::EButton::MenuDown] =
        {
            Input::EGamepadButton::DPAD_Down,
            Input::EKeyboardButton::Down
        };
        this->m_pButtonMapping[Input::EButton::GameShoot] =
        {
            Input::EGamepadButton::DPAD_Up,
            Input::EKeyboardButton::MouseLeft
        };
        this->m_pButtonMapping[Input::EButton::GameUseLaser] =
        {
            Input::EGamepadButton::Pad_A,
            Input::EKeyboardButton::Down
        };
        this->m_pButtonMapping[Input::EButton::GameShootLaser] =
        {
            Input::EGamepadButton::DPAD_Down,
            Input::EKeyboardButton::One
        };
        this->m_pButtonMapping[Input::EButton::GameUseMine] =
        {
            Input::EGamepadButton::Pad_X,
            Input::EKeyboardButton::Three
        };
        this->m_pButtonMapping[Input::EButton::GameShootMine] =
        {
            Input::EGamepadButton::DPAD_Left,
            Input::EKeyboardButton::J
        };
        this->m_pButtonMapping[Input::EButton::GameUseRocket] =
        {
            Input::EGamepadButton::Pad_Y,
            Input::EKeyboardButton::Up
        };
        this->m_pButtonMapping[Input::EButton::GameShootRocket] =
        {
            Input::EGamepadButton::DPAD_Up,
            Input::EKeyboardButton::Four
        };
        this->m_pButtonMapping[Input::EButton::GameUseMG] =
        {
            Input::EGamepadButton::Pad_B,
            Input::EKeyboardButton::Right
        };
        this->m_pButtonMapping[Input::EButton::GameShootMG] =
        {
            Input::EGamepadButton::DPAD_Right,
            Input::EKeyboardButton::Two
        };
        this->m_pButtonMapping[Input::EButton::GameThrustB_1] =
        {
            Input::EGamepadButton::Left_Shoulder,
            Input::EKeyboardButton::S
        };
        this->m_pButtonMapping[Input::EButton::GameThrust] =
        {
            Input::EGamepadButton::Right_Shoulder,
            Input::EKeyboardButton::W
        };
        this->m_pButtonMapping[Input::EButton::GamePause] =
        {
            Input::EGamepadButton::Pad_Start,
            Input::EKeyboardButton::Enter
        };
        this->m_pButtonMapping[Input::EButton::GameClick] =
        {
            Input::EGamepadButton::Pad_A,
            Input::EKeyboardButton::Enter
        };
        this->m_pButtonMapping[Input::EButton::Exit] =
        {
            Input::EGamepadButton::Pad_Back,
            Input::EKeyboardButton::Escape
        };
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Load(const char*a_pFile)
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        serializer->OpenRead(a_pFile);
        this->Deserialize(*serializer);
        serializer->Close();
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Save(const char* a_pFile)
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        serializer->OpenWrite(a_pFile);
        this->Serialize(*serializer);
        serializer->Close();
    }


    // **************************************************************************
    // **************************************************************************
    void CInputManager::Deserialize(TheBrick::CSerializer& a_pSerializer)
    {
        //m_pDirectionMapping
        a_pSerializer.Read(this->m_pDirectionMapping, sizeof(this->m_pDirectionMapping));
        //m_pAxisMapping
        a_pSerializer.Read(this->m_pAxisMapping, sizeof(this->m_pAxisMapping));
        //m_pButtonMapping
        a_pSerializer.Read(this->m_pButtonMapping, sizeof(this->m_pButtonMapping));
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
        //m_pDirectionMapping
        a_pSerializer.Write(this->m_pDirectionMapping, sizeof(this->m_pDirectionMapping));
        //m_pAxisMapping
        a_pSerializer.Write(this->m_pAxisMapping, sizeof(this->m_pAxisMapping));
        //m_pButtonMapping
        a_pSerializer.Write(this->m_pButtonMapping, sizeof(this->m_pButtonMapping));
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::DetectGamepads()
    {
        this->m_ConnectedGamepadsCount = this->m_pInput->GetGamepads();
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::TryDetectGamepads(PuRe_Timer& a_rTimer, float a_RefreshRatePerSecond)
    {
        if (a_rTimer.GetTotalElapsedSeconds() - this->m_LastGamepadDetection >= 1 / a_RefreshRatePerSecond)
        {
            this->m_LastGamepadDetection = a_rTimer.GetTotalElapsedSeconds();
            this->DetectGamepads();
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::Direction(Input::EDirection::Type a_Direction, int a_PlayerIndex, float a_GamepadThreshold)
    {
        if (!INPUTMANAGER_VALIDIDX(a_PlayerIndex))
        {
            return PuRe_Vector2F::Zero();
        }
        Input::SDirectionMapping& mapping = this->m_pDirectionMapping[a_Direction]; //Catch Mouse pressed rotate camera stuff
        if (INPUTMANAGER_PASSTOGAMEPAD(a_PlayerIndex))
        {
            return this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
        }
        else
        {
            switch (a_Direction) //This switch case should be later moved outside this manager
            {
            case sba::Input::EDirection::EditorCameraRotate:
                if (this->GetKeyboardButtonIsPressed(this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive].Keyboard))
                {
                    return this->GetKeyboardDirection(mapping.Keyboard);
                }
                break;
            case sba::Input::EDirection::EditorMoveBrick:
                if (!this->GetKeyboardButtonIsPressed(this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive].Keyboard))
                {
                    return this->GetKeyboardDirection(mapping.Keyboard);
                }
                break;
            default:
                return this->GetKeyboardDirection(mapping.Keyboard);
                break;
            }
            return PuRe_Vector2F::Zero();
        }
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::Axis(Input::EAxis::Type a_Axis, int a_PlayerIndex, float a_GamepadThreshold)
    {
        if (!INPUTMANAGER_VALIDIDX(a_PlayerIndex))
        {
            return 0.0f;
        }
        Input::SAxisMapping& mapping = this->m_pAxisMapping[a_Axis];
        float result = 0;
        if (INPUTMANAGER_PASSTOGAMEPAD(a_PlayerIndex))
        {
            return this->GetGamepadAxis(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
        }
        else
        {
            return this->GetKeyboardAxis(mapping.Keyboard);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        if (!INPUTMANAGER_VALIDIDX(a_PlayerIndex))
        {
            return false;
        }
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        if (INPUTMANAGER_PASSTOGAMEPAD(a_PlayerIndex))
        {
            return this->GetGamepadButtonPressed(mapping.Gamepad, a_PlayerIndex);
        }
        else
        {
            return this->GetKeyboardButtonPressed(mapping.Keyboard);
        }
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonIsPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        if (!INPUTMANAGER_VALIDIDX(a_PlayerIndex))
        {
            return false;
        }
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        if (INPUTMANAGER_PASSTOGAMEPAD(a_PlayerIndex))
        {
            return this->GetGamepadButtonIsPressed(mapping.Gamepad, a_PlayerIndex);
        }
        else
        {
            return this->GetKeyboardButtonIsPressed(mapping.Keyboard);
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetKeyboardDirection(Input::EKeyboardDirection::Type a_Direction)
    {
        PuRe_Vector2F result = PuRe_Vector2F::Zero();
        switch (a_Direction)
        {
        case Input::EKeyboardDirection::ArrowKeys:
            result = this->CalcKeyboardDirectionFromKeys(this->m_pInput->Up, this->m_pInput->Down, this->m_pInput->Right, this->m_pInput->Left);
            break;
        case Input::EKeyboardDirection::WSAD:
            result = this->CalcKeyboardDirectionFromKeys(this->m_pInput->W, this->m_pInput->S, this->m_pInput->D, this->m_pInput->A);
            break;
        case Input::EKeyboardDirection::IKJL:
            result = this->CalcKeyboardDirectionFromKeys(this->m_pInput->I, this->m_pInput->K, this->m_pInput->L, this->m_pInput->J);
            break;
        case Input::EKeyboardDirection::Numpad:
            result = this->CalcKeyboardDirectionFromKeys(this->m_pInput->Eight, this->m_pInput->Five, this->m_pInput->Six, this->m_pInput->Four);
            break;
        case Input::EKeyboardDirection::Mouse:
            result = this->m_pInput->GetRelativeMousePosition() * PuRe_Vector2F(1, -1) * 0.05f;
            break;
        }
        return this->CheckLastInputIsKeyboard(result);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CInputManager::GetGamepadDirection(Input::EGamepadDirection::Type a_Direction, int a_Index, float a_Threshold)
    {
        PuRe_Vector2F result = PuRe_Vector2F::Zero();
        switch (a_Direction)
        {
        case Input::EGamepadDirection::LeftThumb:
            result = this->m_pInput->GetGamepadLeftThumb(a_Index);
            break;
        case Input::EGamepadDirection::RightThumb:
            result = this->m_pInput->GetGamepadRightThumb(a_Index);
            break;
        case Input::EGamepadDirection::DPad:
            result = this->CalcGamepadDirectionFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
            break;
        case Input::EGamepadDirection::Buttons:
            result = this->CalcGamepadDirectionFromButtons(this->m_pInput->Pad_Y, this->m_pInput->Pad_A, this->m_pInput->Pad_B, this->m_pInput->Pad_X, a_Index);
            break;
        }
        if (result.Length() < a_Threshold)
        {
            result = PuRe_Vector2F::Zero();
        }
        return this->CheckLastInputIsGamepad(result);
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetKeyboardAxis(Input::EKeyboardAxis::Type a_Axis)
    {
        float result = 0;
        switch (a_Axis)
        {
        case Input::EKeyboardAxis::WS:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->W, this->m_pInput->S);
            break;
        case Input::EKeyboardAxis::AD:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->D, this->m_pInput->A);
            break;
        case Input::EKeyboardAxis::RF:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->R, this->m_pInput->F);
            break;
        case Input::EKeyboardAxis::QE:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->E, this->m_pInput->Q);
            break;
        case Input::EKeyboardAxis::ArrowUD:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->Up, this->m_pInput->Down);
            break;
        case Input::EKeyboardAxis::ArrowLR:
            result = this->CalcKeyboardAxisFromKeys(this->m_pInput->Right, this->m_pInput->Left);
            break;
        case Input::EKeyboardAxis::MouseX:
            result = this->m_pInput->GetRelativeMousePosition().X;
            break;
        case Input::EKeyboardAxis::MouseY:
            result = this->m_pInput->GetRelativeMousePosition().Y;
            break;
        case Input::EKeyboardAxis::MouseScroll:
            result = this->m_pInput->GetMouseScroll() * 2.0f;
            break;
        }
        return this->CheckLastInputIsKeyboard(result);
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::GetGamepadAxis(Input::EGamepadAxis::Type a_Axis, int a_Index, float a_Threshold)
    {
        float result = 0;
        switch (a_Axis)
        {
        case Input::EGamepadAxis::LeftTrigger:
            result += this->m_pInput->GetGamepadLeftTrigger(a_Index);
            break;
        case Input::EGamepadAxis::RightTrigger:
            result += this->m_pInput->GetGamepadRightTrigger(a_Index);
            break;
        case Input::EGamepadAxis::BothTrigger:
            result += this->m_pInput->GetGamepadRightTrigger(a_Index) - this->m_pInput->GetGamepadLeftTrigger(a_Index);
            break;
        case Input::EGamepadAxis::LeftThumbX:
            result += this->m_pInput->GetGamepadLeftThumb(a_Index).X;
            break;
        case Input::EGamepadAxis::LeftThumbY:
            result += this->m_pInput->GetGamepadLeftThumb(a_Index).Y;
            break;
        case Input::EGamepadAxis::RightThumbX:
            result += this->m_pInput->GetGamepadRightThumb(a_Index).X;
            break;
        case Input::EGamepadAxis::RightThumbY:
            result += this->m_pInput->GetGamepadRightThumb(a_Index).Y;
            break;
        case Input::EGamepadAxis::DPadX:
            result += this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Right, this->m_pInput->DPAD_Left, a_Index);
            break;
        case Input::EGamepadAxis::DPadY:
            result += this->CalcGamepadAxisFromButtons(this->m_pInput->DPAD_Up, this->m_pInput->DPAD_Down, a_Index);
            break;
        }
        if (abs(result) < a_Threshold)
        {
            result = 0;
        }
        return this->CheckLastInputIsGamepad(result);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetKeyboardButtonPressed(Input::EKeyboardButton::Type a_Button)
    {
        bool result = false;
        if (a_Button <= Input::EKeyboardButton::PURE_LAST)
        {
            result = this->m_pInput->KeyPressed((PuRe_IInput::EKeys)a_Button);
        }
        else
        {
            switch (a_Button)
            {
            case Input::EKeyboardButton::CtrlS:
                result = this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
                break;
            case Input::EKeyboardButton::MouseLeft:
                result = this->m_pInput->MousePressed(this->m_pInput->LeftClick);
                break;
            case Input::EKeyboardButton::MouseMiddle:
                result = this->m_pInput->MousePressed(this->m_pInput->MiddleClick);
                break;
            case Input::EKeyboardButton::MouseRight:
                result = this->m_pInput->MousePressed(this->m_pInput->RightClick);
                break;
            }
        }
        return this->CheckLastInputIsKeyboard(result);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonPressed(Input::EGamepadButton::Type a_Button, int a_Index)
    {
        bool result = false;
        if (a_Button <= Input::EGamepadButton::PURE_LAST)
        {
            result = this->m_pInput->GamepadPressed((PuRe_IInput::EGamepad)a_Button, a_Index);
        }
        else
        {
            /*switch (a_Button)
            {
            default:*/
            //Nothing added yet
            result = false;
            /*}*/
        }
        return this->CheckLastInputIsGamepad(result);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetKeyboardButtonIsPressed(Input::EKeyboardButton::Type a_Button)
    {
        bool result = false;
        if (a_Button <= Input::EKeyboardButton::PURE_LAST)
        {
            result = this->m_pInput->KeyIsPressed((PuRe_IInput::EKeys)a_Button);
        }
        else
        {
            switch (a_Button)
            {
            case Input::EKeyboardButton::CtrlS:
                result = this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->S);
                break;
            case Input::EKeyboardButton::MouseLeft:
                result = this->m_pInput->MouseIsPressed(this->m_pInput->LeftClick);
                break;
            case Input::EKeyboardButton::MouseMiddle:
                result = this->m_pInput->MouseIsPressed(this->m_pInput->MiddleClick);
                break;
            case Input::EKeyboardButton::MouseRight:
                result = this->m_pInput->MouseIsPressed(this->m_pInput->RightClick);
                break;
            }
        }
        return this->CheckLastInputIsKeyboard(result);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::GetGamepadButtonIsPressed(Input::EGamepadButton::Type a_Button, int a_Index)
    {
        bool result = false;
        if (a_Button <= Input::EGamepadButton::PURE_LAST)
        {
            result = this->m_pInput->GamepadIsPressed((PuRe_IInput::EGamepad)a_Button, a_Index);
        }
        else
        {
            /*switch (a_Button)
            {
            default:*/
            //Nothing added yet
            result = false;
            /*}*/
        }
        return this->CheckLastInputIsGamepad(result);
    }
}