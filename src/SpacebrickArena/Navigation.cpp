#include "include/Navigation.h"
#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CNavigation::CNavigation(int a_ElementsPerLine, int a_LastElement, float a_ScrollingSpeed, float a_ScrollingThreshold, float a_InputThreshold)
    {
        this->m_ElementsPerLine = a_ElementsPerLine;
        this->m_LastElement = a_LastElement;
        this->m_FocusedElement = 0;
        this->m_FocusPosition = PuRe_Vector2F::Zero();
        this->m_PreviousState = EDirection::None;
        this->m_InputThreshold = a_InputThreshold;
        this->m_ScrollingThreshold = a_ScrollingThreshold;
        this->m_ScrollingSpeed = a_ScrollingSpeed;
        this->m_pTimer = new TheBrick::CBrickTimer();
    }

    // **************************************************************************
    // **************************************************************************
    CNavigation::~CNavigation()
    {
        SAFE_DELETE(this->m_pTimer);
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Update(PuRe_Timer& a_pTimer, PuRe_Vector2F a_InputVector, bool a_LinkEnds)
    {
        EDirection::Type a_NewDirection = this->DirectionFromInputVector(a_InputVector);
        //to play sound only when a new element is focused
        if (a_NewDirection != this->m_PreviousState)
        {
            if (a_NewDirection != EDirection::None)
            {
                this->Navigate(a_NewDirection, a_LinkEnds); //Navigate before scrolling
            }
            this->m_FocusPosition = PuRe_Vector2F::Zero(); //Stop/reset scrolling
            this->m_pTimer->Reset();
        }
        else if (a_NewDirection != EDirection::None && this->m_pTimer->GetTotalElapsedSeconds() > this->m_ScrollingThreshold) //Scrolling
        {
            this->Scroll(a_InputVector, a_pTimer.GetElapsedSeconds(), a_LinkEnds);
        }
        this->m_PreviousState = a_NewDirection;
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Navigate(EDirection::Type a_Direction, bool a_LinkEnds)
    {
        int cache = this->m_FocusedElement;
        switch (a_Direction)
        {
        case EDirection::Right:
            this->m_FocusedElement++;
            break;
        case EDirection::Left:
            this->m_FocusedElement--;
            break;
        case EDirection::Up:
            this->m_FocusedElement -= this->m_ElementsPerLine;
            break;
        case EDirection::Down:
            this->m_FocusedElement += this->m_ElementsPerLine;
            if (this->m_FocusedElement > this->m_LastElement && floor(this->m_FocusedElement / (float)this->m_ElementsPerLine) == floor(this->m_LastElement / (float)this->m_ElementsPerLine))
            { //Bei Nav in Leerposition der letzten Reihe in letztes Element der letzten Reihe springen
                this->m_FocusedElement = this->m_LastElement;
            }
            break;
        }
        this->ClampFocus(a_LinkEnds);
        if (this->m_FocusedElement != cache)
        {
            sba_SoundPlayer->PlaySound("menu_over", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::Scroll(PuRe_Vector2F a_Input, float a_DeltaSec, bool a_LinkEnds)
    {
        this->m_FocusPosition += a_Input * a_DeltaSec * this->m_ScrollingSpeed * 100.0f;
        if (this->m_FocusPosition.X > 100)
        {
            this->Navigate(EDirection::Right, a_LinkEnds);
            this->m_FocusPosition.X -= 100;
        }
        else if (this->m_FocusPosition.X < -100)
        {
            this->Navigate(EDirection::Left, a_LinkEnds);
            this->m_FocusPosition.X += 100;
        }
        else if (this->m_FocusPosition.Y > 100)
        {
            this->Navigate(EDirection::Up, a_LinkEnds);
            this->m_FocusPosition.Y -= 100;
        }
        else if (this->m_FocusPosition.Y < -100)
        {
            this->Navigate(EDirection::Down, a_LinkEnds);
            this->m_FocusPosition.Y += 100;
        }
    }

    // **************************************************************************
    // **************************************************************************
    CNavigation::EDirection::Type CNavigation::DirectionFromInputVector(PuRe_Vector2F a_InputVector) const
    {
        EDirection::Type state = EDirection::None;
        if (a_InputVector.Length() < this->m_InputThreshold)
        {
            return state;
        }
        a_InputVector.Normalize();
        float limit = sin(45 * 0.0174532925f);
        if (a_InputVector.X > limit)
        {
            state = EDirection::Right;
        }
        else if (a_InputVector.X < -limit)
        {
            state = EDirection::Left;
        }
        else if (a_InputVector.Y > limit)
        {
            state = EDirection::Up;
        }
        else if (a_InputVector.Y < -limit)
        {
            state = EDirection::Down;
        }
        return state;
    }

    // **************************************************************************
    // **************************************************************************
    void CNavigation::ClampFocus(bool a_LinkEnds)
    {
        if (this->m_FocusedElement > this->m_LastElement) //Over End
        {
            if (a_LinkEnds) //Jump
            {
                this->m_FocusedElement = 0;
            }
            else //Clamp
            {
                this->m_FocusedElement = this->m_LastElement;
            }
        }
        else if (this->m_FocusedElement < 0) //Over Start
        {
            if (a_LinkEnds) //Jump
            {
                this->m_FocusedElement = this->m_LastElement;
            }
            else //Clamp
            {
                this->m_FocusedElement = 0;
            }
        }
    }

    void CNavigation::AddElement(int a_Position)
    {
        this->m_LastElement++;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement++;
        }
    }

    void CNavigation::RemoveElement(int a_Position)
    {
        this->m_LastElement--;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement--;
        }
        this->ClampFocus();
    }

    void CNavigation::AddLine(int a_Position)
    {
        this->m_LastElement += this->m_ElementsPerLine;
        if (a_Position != -1 && a_Position < this->m_FocusedElement)
        {
            this->m_FocusedElement += this->m_ElementsPerLine;
        }
    }

    void CNavigation::RemoveLine(int a_Position)
    {
        this->m_LastElement -= this->m_ElementsPerLine;
        if (a_Position != -1 && a_Position <= this->m_FocusedElement)
        {
            this->m_FocusedElement -= this->m_ElementsPerLine;
        }
        this->ClampFocus();
    }
}