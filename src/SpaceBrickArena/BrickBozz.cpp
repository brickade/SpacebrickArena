#include "include/BrickBozz.h"

namespace Game
{
    BrickBozz* BrickBozz::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    BrickBozz::BrickBozz()
    {
        this->BrickManager = new TheBrick::CBrickManager();
        this->World = new ong::World();
    }


    // **************************************************************************
    // **************************************************************************
    BrickBozz::~BrickBozz()
    {

    }

    // **************************************************************************
    // **************************************************************************
    BrickBozz* BrickBozz::Instance()
    {
        if (BrickBozz::g_pInstance == nullptr)
        {
            BrickBozz::g_pInstance = new BrickBozz();
        }
        return BrickBozz::g_pInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void BrickBozz::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        this->BrickManager->Initialize(a_pGraphics, a_pSoundPlayer);
    }
}