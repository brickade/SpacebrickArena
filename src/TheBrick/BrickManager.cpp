#include "include/TheBrick/BrickManager.h"

#include "include/TheBrick/Brick.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickManager::CBrickManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    CBrickManager::~CBrickManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Initialize(PuRe_IGraphics& a_pGraphics, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        this->m_pBrickMaterial = a_pGraphics.LoadMaterial("../data/effects/instanced/default");
        this->m_pBrickUIMaterial = a_pGraphics.LoadMaterial("../data/effects/editor/default");
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Load(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, ong::World& a_pWorld, PuRe_IMaterial& a_pMaterial, const char* a_pFolder)
    {
        CSerializer* serializer = new CSerializer();
        int i = 0;
        std::string file = a_pWindow.GetFileAtIndex(i, a_pFolder);
        while (file != "")
        {
            CBrick* brick = new CBrick(a_pMaterial);
            file.insert(0, a_pFolder);
            
            serializer->OpenRead(file.c_str());
            brick->Deserialize(*serializer, a_pGraphics, a_pWorld);
            serializer->Close();
            
            this->m_bricks[brick->GetBrickId()] = brick;
            
            i++;
            file = a_pWindow.GetFileAtIndex(i, a_pFolder);
        }
        delete serializer;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Render(PuRe_Renderer& a_rRenderer)
    {
        for (std::array<CBrick*, 200>::iterator it = this->m_bricks.begin(); it != this->m_bricks.end(); ++it)
        {
            if (*it != nullptr)
            {
                (*it)->Draw(a_rRenderer);
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::RebuildRenderInstances()
    {
        for (std::array<CBrick*, 200>::iterator it = this->m_bricks.begin(); it != this->m_bricks.end(); ++it)
        {
            if (*it != nullptr)
            {
                (*it)->RebuildRenderInstances();
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    CBrick& CBrickManager::GetBrick(int a_BrickId)
    {
        #ifdef DEBUG
            try //Bounds checking
            {
                return this->m_bricks.at(a_BrickId);
            }
            catch (std::out_of_range)
            {
                return nullptr;
            }
        #else
            return *this->m_bricks[a_BrickId];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    CBrick** CBrickManager::GetCategoryStart(int a_CategoryId)
    {
        #ifdef DEBUG
            try //Bounds checking
            {
                return this->m_bricks.at(a_CategoryId * 100);
            }
            catch (std::out_of_range)
            {
                return nullptr;
            }
        #else
            return &this->m_bricks[a_CategoryId * 100];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    int CBrickManager::GetCategoryCount()
    {
        return (int)ceil(this->m_bricks.max_size() / 100.0f);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_IMaterial* CBrickManager::GetBrickMaterial()
    {
        #ifdef DEBUG
            if (this->m_pBrickMaterial == nullptr)
            {
                print("CBrickManager::GetBrickMaterial() was called before initializing the BrickManager!!!");       
            }
        #endif
        return this->m_pBrickMaterial;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_IMaterial* CBrickManager::GetBrickUIMaterial()
    {
        #ifdef DEBUG
        if (this->m_pBrickUIMaterial == nullptr)
            {
                print("CBrickManager::GetBrickUIMaterial() was called before initializing the BrickManager!!!");
            }
        #endif
        return this->m_pBrickUIMaterial;
    }
}