#include "include/InputManager.h"

#include <TheBrick/Serializer.h>

namespace sba
{
    const float CInputManager::DEFAULT_GAMEPAD_THRESHOLD = 0.25f;

    // **************************************************************************
    // **************************************************************************
    CInputManager::CInputManager()
    {
        this->m_lastInputWasGamepad = false;
    }

    // **************************************************************************
    // **************************************************************************
    CInputManager::~CInputManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Initialize(PuRe_IInput* a_pInput)
    {
        this->m_pInput = a_pInput;
        //this->Load("input.data");
        this->Reset();
    }

    // **************************************************************************
    // **************************************************************************
    void CInputManager::Reset()
    {
        //Direction
        memset(&this->m_pDirectionMapping, 0, sizeof(this->m_pDirectionMapping));
        this->m_pDirectionMapping[Input::EDirection::EditorCameraRotate] =
        {
            Input::EGamepadDirection::RightThumb,
            {
                Input::EKeyboardDirection::Mouse,
                Input::EKeyboardDirection::ArrowKeys,
                Input::EKeyboardDirection::ArrowKeys,
                Input::EKeyboardDirection::ArrowKeys
            }
        };
        this->m_pDirectionMapping[Input::EDirection::EditorMoveBrick] =
        {
            Input::EGamepadDirection::LeftThumb,
            {
                Input::EKeyboardDirection::Mouse,
                Input::EKeyboardDirection::WSAD,
                Input::EKeyboardDirection::WSAD,
                Input::EKeyboardDirection::WSAD
            }
        };
        this->m_pDirectionMapping[Input::EDirection::EditorNavigateCategory] =
        {
            Input::EGamepadDirection::DPad,
            {
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL,
                Input::EKeyboardDirection::IKJL
            }
        };

        //Axis
        memset(&this->m_pAxisMapping, 0, sizeof(this->m_pAxisMapping));
        this->m_pAxisMapping[Input::EAxis::EditorCameraZoom] =
        {
            Input::EGamepadAxis::BothTrigger,
            {
                Input::EKeyboardAxis::MouseScroll,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF
            }
        };
        this->m_pAxisMapping[Input::EAxis::EditorColorSlider] =
        {
            Input::EGamepadAxis::BothTrigger,
            {
                Input::EKeyboardAxis::MouseScroll,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF,
                Input::EKeyboardAxis::RF
            }
        };

        //Button
        memset(&this->m_pButtonMapping, 0, sizeof(this->m_pButtonMapping));
        this->m_pButtonMapping[Input::EButton::EditorPlaceBrick] =
        {
            Input::EGamepadButton::Pad_A,
            {
                Input::EKeyboardButton::MouseLeft,
                Input::EKeyboardButton::Enter,
                Input::EKeyboardButton::Enter,
                Input::EKeyboardButton::Enter
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorTogglePlacementSide] =
        {
            Input::EGamepadButton::Pad_B,
            {
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space,
                Input::EKeyboardButton::Space
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorUndo] =
        {
            Input::EGamepadButton::Pad_X,
            {
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ,
                Input::EKeyboardButton::CtrlZ
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRedo] =
        {
            Input::EGamepadButton::Pad_Y,
            {
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY,
                Input::EKeyboardButton::CtrlY
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickRight] =
        {
            Input::EGamepadButton::Right_Shoulder,
            {
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E,
                Input::EKeyboardButton::E
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorRotateBrickLeft] =
        {
            Input::EGamepadButton::Left_Shoulder,
            {
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q,
                Input::EKeyboardButton::Q
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorResetShip] =
        {
            Input::EGamepadButton::Pad_Start,
            {
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete,
                Input::EKeyboardButton::Delete
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorSaveShip] =
        {
            Input::EGamepadButton::Left_Thumb,
            {
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS,
                Input::EKeyboardButton::CtrlS
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorToggleNavigateCategories] =
        {
            Input::EGamepadButton::Right_Thumb,
            {
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X,
                Input::EKeyboardButton::X
            }
        };
        this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive] =
        {
            Input::EGamepadButton::Right_Thumb,
            {
                Input::EKeyboardButton::MouseRight,
                Input::EKeyboardButton::MouseRight,
                Input::EKeyboardButton::MouseRight,
                Input::EKeyboardButton::MouseRight
            }
        };
        this->m_pButtonMapping[Input::EButton::Exit] =
        {
            Input::EGamepadButton::Pad_Back,
            {
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape,
                Input::EKeyboardButton::Escape
            }
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
    PuRe_Vector2F CInputManager::Direction(Input::EDirection::Type a_Direction, int a_PlayerIndex, float a_GamepadThreshold)
    {
        Input::SDirectionMapping& mapping = this->m_pDirectionMapping[a_Direction]; //Catch Mouse pressed rotate camera stuff
        PuRe_Vector2F result = PuRe_Vector2F::Zero();
        switch (a_Direction)
        {
        case sba::Input::EDirection::EditorCameraRotate:
            result += this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
            if (this->GetKeyboardButtonIsPressed(this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive].Keyboard[a_PlayerIndex]))
            {
                result += this->GetKeyboardDirection(mapping.Keyboard[a_PlayerIndex]);
            }
            break;
        case sba::Input::EDirection::EditorMoveBrick:
            result += this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
            if (!this->GetKeyboardButtonIsPressed(this->m_pButtonMapping[Input::EButton::EditorCameraRotateActive].Keyboard[a_PlayerIndex]))
            {
                result += this->GetKeyboardDirection(mapping.Keyboard[a_PlayerIndex]);
            }
            break;
        default:
            result += this->GetGamepadDirection(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
            result += this->GetKeyboardDirection(mapping.Keyboard[a_PlayerIndex]);
            break;
        }
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    float CInputManager::Axis(Input::EAxis::Type a_Axis, int a_PlayerIndex, float a_GamepadThreshold)
    {
        Input::SAxisMapping& mapping = this->m_pAxisMapping[a_Axis];
        float result = 0;
        switch (a_Axis)
        {
        default:
            result += this->GetGamepadAxis(mapping.Gamepad, a_PlayerIndex, a_GamepadThreshold);
            result += this->GetKeyboardAxis(mapping.Keyboard[a_PlayerIndex]);
            break;
        }
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonPressed(mapping.Keyboard[a_PlayerIndex]);
    }

    // **************************************************************************
    // **************************************************************************
    bool CInputManager::ButtonIsPressed(Input::EButton::Type a_Button, int a_PlayerIndex)
    {
        Input::SButtonMapping& mapping = this->m_pButtonMapping[a_Button];
        return this->GetGamepadButtonIsPressed(mapping.Gamepad, a_PlayerIndex) + this->GetKeyboardButtonIsPressed(mapping.Keyboard[a_PlayerIndex]);
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
            result = this->m_pInput->GetRelativeMousePosition() * PuRe_Vector2F(1, -1) * 0.1f;
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
            case Input::EKeyboardButton::CtrlZ:
                result = this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
                break;
            case Input::EKeyboardButton::CtrlY:
                result = this->CalcKeyboardKeyPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
                break;
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
            switch (a_Button)
            {
            default:
                //Nothing added yet
                result = false;
            }
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
            case Input::EKeyboardButton::CtrlZ:
                result = this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Z);
                break;
            case Input::EKeyboardButton::CtrlY:
                result = this->CalcKeyboardKeyIsPressedWithHoldKey(this->m_pInput->Ctrl, this->m_pInput->Y);
                break;
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
            switch (a_Button)
            {
            default:
                //Nothing added yet
                result = false;
            }
        }
        return this->CheckLastInputIsGamepad(result);
    }
}