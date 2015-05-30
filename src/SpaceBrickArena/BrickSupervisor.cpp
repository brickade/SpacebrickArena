#include "include/BrickSupervisor.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickSupervisor::CBrickSupervisor(int a_PlayerIdx)
    {
        this->m_PlayerIdx = a_PlayerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    CBrickSupervisor::~CBrickSupervisor()
    {
        SAFE_DELETE(this->m_pCamera);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Initialize(PuRe_IGraphics& a_pGraphics)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics.GetDescription();
        this->m_pCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pCamera->setNearFar(PuRe_Vector2F(0.1f, 1000));
        this->m_CategoriesCount = BrickBozz::Instance()->BrickManager->GetCategoryCount();
        assert(this->m_Categories.max_size() >= this->m_CategoriesCount);
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            this->m_Categories[i] = new CBrickCategory(i);
        }
        this->m_pNavigation = new CNavigation(1, this->m_CategoriesCount - 1);
        this->m_pActiveCategory = this->m_Categories[0];
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_IInput& a_pInput, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        
        if (a_pInput.GamepadIsPressed(a_pInput.Right_Thumb, this->m_PlayerIdx) || a_pInput.KeyIsPressed(a_pInput.X))
        {
            this->m_NavigateTabs = true;
        }
        else
        {
            this->m_NavigateTabs = false;
            this->m_TabRotation = 0.4f;
        }
        if (this->m_NavigateTabs)
        {
            PuRe_Vector2F navInput = PuRe_Vector2F::Zero();
            //Gamepad
            if (a_pInput.GamepadIsPressed(a_pInput.DPAD_Right, this->m_PlayerIdx))
            {
                navInput.X += 1;
            }
            if (a_pInput.GamepadIsPressed(a_pInput.DPAD_Left, this->m_PlayerIdx))
            {
                navInput.X -= 1;
            }
            if (a_pInput.GamepadIsPressed(a_pInput.DPAD_Up, this->m_PlayerIdx))
            {
                navInput.Y += 1;
            }
            if (a_pInput.GamepadIsPressed(a_pInput.DPAD_Down, this->m_PlayerIdx))
            {
                navInput.Y -= 1;
            }

            //Keyboard
            if (a_pInput.KeyPressed(a_pInput.Right))
            {
                navInput.X += 1.0f;
            }
            if (a_pInput.KeyPressed(a_pInput.Left))
            {
                navInput.X -= 1.0f;
            }
            if (a_pInput.KeyPressed(a_pInput.Up))
            {
                navInput.Y += 1.0f;
            }
            if (a_pInput.KeyPressed(a_pInput.Down))
            {
                navInput.Y -= 1.0f;
            }
            this->m_pNavigation->Update(a_pTimer, navInput);
            this->m_pActiveCategory = this->m_Categories[this->m_pNavigation->GetFocusedElementId()];
            this->m_TabRotation += a_pTimer.GetElapsedSeconds() * this->m_RotationSpeed;
            this->m_TabRotation = fmod(this->m_TabRotation, 6.28318531f);
        }
        else
        {
            this->m_pActiveCategory->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer, this->m_PlayerIdx, this->m_RotationSpeed);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickSupervisor::Render(PuRe_IGraphics& a_pGraphics)
    {
        for (int i = 0; i < this->m_CategoriesCount; i++)
        {
            bool selected = false;
            if (this->m_Categories[i] == this->m_pActiveCategory)
            {
                selected = true;
            }
            this->m_Categories[i]->RenderTab(a_pGraphics, *this->m_pCamera, this->m_TabRotation, selected);
        }
        this->m_pActiveCategory->Render(a_pGraphics, *this->m_pCamera);
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrick* CBrickSupervisor::GetSelectedBrick() const
    {
        return this->m_pActiveCategory->GetFocusedBrick();
    }
}