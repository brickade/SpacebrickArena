#ifndef _BRICKSUPERVISOR_H_
#define _BRICKSUPERVISOR_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <cassert>

#include "BrickCategory.h"
#include "BrickBozz.h"

namespace Game
{
    class CBrickSupervisor
    {
    public:
        int m_PlayerIdx;
        std::array<CBrickCategory*, 10> m_Categories;
        CBrickCategory* m_pActiveCategory;
        PuRe_Camera* m_pCamera;

    public:
        CBrickSupervisor(int a_PlayerIdx);
        ~CBrickSupervisor();

        void Initialize(PuRe_IGraphics* a_pGraphics);

        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer);
        void Render(PuRe_IGraphics* a_pGraphics);
    };
}

#endif /* _BRICKSUPERVISOR_H_ */