#include "include/MainScene.h"
namespace Game
{
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_pApplication = a_pApplication;
        this->m_pEditorScene = new CEditorScene(a_pApplication);
        this->m_pGameScene = new CGameScene(a_pApplication);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Scenes
        this->m_pEditorScene->Initialize(a_pGraphics, a_pSoundPlayer);
        this->m_pGameScene->Initialize(a_pGraphics, a_pSoundPlayer);

        this->m_pActiveScene = this->m_pGameScene;
        //this->m_pActiveScene = this->m_pEditorScene;
    }

    // **************************************************************************
    // **************************************************************************
    bool CMainScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC))
        {
            return true;
        }

        if (this->m_pActiveScene->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer))
        {
            if (this->m_pActiveScene == this->m_pGameScene)
            {
                this->m_pActiveScene = this->m_pEditorScene;
            }
            else
            {
                this->m_pActiveScene = this->m_pGameScene;
            }
        }

        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pActiveScene->Render(a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Exit()
    {
        this->m_pEditorScene->Exit();
        this->m_pGameScene->Exit();
        SAFE_DELETE(this->m_pEditorScene);
        SAFE_DELETE(this->m_pGameScene);
    }
}