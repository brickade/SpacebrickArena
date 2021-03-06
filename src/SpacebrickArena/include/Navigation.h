#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/BrickTimer.h>

namespace sba
{
    class CNavigation
    {
        struct EDirection
        {
            enum Type
            {
                None,
                Up,
                Down,
                Left,
                Right,
            };
        };
    public:

    private:
        int m_ElementsPerLine;
        int m_LastElement;
        int m_FocusedElement;

        PuRe_Vector2F m_FocusPosition;
        EDirection::Type m_PreviousState;
        float m_InputThreshold;
        float m_ScrollingThreshold;
        float m_ScrollingSpeed;
        float m_ScrollingStartPause;
        TheBrick::CBrickTimer* m_pTimer;

    public:
        int GetElementsCountPerLine() const
        {
            return this->m_ElementsPerLine;
        }

        void SetElementsCountPerLine(int a_ElementsPerLine)
        {
            this->m_ElementsPerLine = a_ElementsPerLine;
        }

        int GetLastElementId() const
        {
            return this->m_LastElement;
        }

        void SetLastElementId(int a_LastElement)
        {
            this->m_LastElement = a_LastElement;
            this->ClampFocus();
        }

        int GetFocusedElementId() const
        {
            return this->m_FocusedElement;
        }

        void SetFocusedElementId(int a_FocusedElement)
        {
            this->m_FocusedElement = a_FocusedElement;
            this->ClampFocus();
        }

        int GetLineCount() const
        {
            return (int)ceil((this->m_LastElement + 1) / this->m_ElementsPerLine);
        }

        int GetFocusedLineId() const
        {
            return (int)floor(this->m_FocusedElement / this->m_ElementsPerLine);
        }

    public:
        CNavigation(int a_ElementsPerLine = 1, int a_LastElement = -1, float a_ScrollingSpeed = 13.0f, float a_ScrollingThreshold = 0.3f, float a_InputThreshold = 0.3f);
        ~CNavigation();

        void Update(PuRe_Timer& a_pTimer, PuRe_Vector2F a_InputVector, bool a_LinkEnds = true);
        void Navigate(EDirection::Type a_Direction, bool a_LinkEnds = true);
        void Scroll(PuRe_Vector2F a_Input, float a_DeltaSec, bool a_LinkEnds = true);
        EDirection::Type CNavigation::DirectionFromInputVector(PuRe_Vector2F a_ThumbInput) const;

        void AddElement(int a_Position = -1);
        void RemoveElement(int a_Position = -1);
        void AddLine(int a_Position = -1);
        void RemoveLine(int a_Position = -1);

    private:
        void ClampFocus(bool a_LinkEnds = false);

    };
}

#endif /* _NAVIGATION_H_ */