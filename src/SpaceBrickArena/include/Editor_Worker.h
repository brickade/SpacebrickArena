#ifndef _EDITORWORKER_H_
#define _EDITORWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/GameObject.h"

#include "Spaceship.h"

#include "Editor_Camera.h"

#include "TheBrick/Brick.h"

namespace TheBrick
{
    class CGameObject;
    class CBrickInstance;
}

namespace Editor
{
    class CEditorScene;
    class CHistory;
    class CShipHandler;

    class CWorker
    {

    public:

    private:
        CCamera* m_pCamera;

        int m_currentBrickRotation;
        int m_maxBrickDistance;
        PuRe_Vector2I m_currentBrickPosition;
        PuRe_Vector2F m_currentPosition;
        int m_currentHeight;
        bool m_CurrentBrickHeightIstInvalid;
        PuRe_Color m_currentBrickColor;
        bool m_canPlaceHere = false; 
        float m_nubDockThreshold = 0.01f;
        float m_nubDirThreshold = 0.01f;

        float m_GamepadSpeedupTimer = 0;

        bool m_placeBelow;

        CHistory* m_pHistory;

        int m_playerIdx;

        TheBrick::CGameObject* m_pCurrentBrickObject;
        TheBrick::CBrickInstance* m_pCurrentBrick;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;

    public:
        CCamera* GetCamera() const
        {
            return this->m_pCamera;
        }

        float GetCurrentBrickRotation()
        {
            return this->m_currentBrickRotation * 90.0f;
        }
        PuRe_Vector2F GetCurrentBrickPosition()
        {
            return PuRe_Vector2F(this->m_currentBrickPosition.X * TheBrick::CBrick::SEGMENT_WIDTH, this->m_currentBrickPosition.Y * TheBrick::CBrick::SEGMENT_WIDTH);
        }

    public:
        CWorker(int a_playerIdx);
        ~CWorker();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor, CShipHandler& a_rShipHandler);
        void Render(CShipHandler& a_rShipHandler);
        void Suspend();

    private:
        void UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed);
        void UpdateRotation();
        void UpdateHeight(CShipHandler& a_rShipHandler);
        void ApplyToCurrentBrick();
        void UpdatePlacement(CShipHandler& a_rShipHandler);
    };
}

#endif /* _EDITORWORKER_H_ */