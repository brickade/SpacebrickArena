#include "include/ShipManager.h"

#include "TheBrick/Serializer.h"
#include "TheBrick/Brick.h"
#include "include/BrickManager.h"

#include "include/Space.h"
#include "include/EditorScene.h"
#include "include/Editor_Camera.h"

#include "include/Asteroid.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CShipManager::CShipManager(const char* a_pFolder)
    {
        this->m_FolderPath = a_pFolder;
    }

    // **************************************************************************
    // **************************************************************************
    CShipManager::~CShipManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::Load()
    { //Load all sprites and paths from folder
        Editor::CEditorScene::InitCommonRenderComponents(&this->m_pPostMaterial, &this->m_pDirectionalLight, &this->m_pDirectionalLightMaterial); //Load everything for rendering
        PuRe_GraphicsDescription gdesc = sba_Application->GetGraphics()->GetDescription();
        this->m_pCamera = new Editor::CCamera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective, 0);
        this->m_pCamera->Initialize(PuRe_Vector3F(20, 135, 0), PuRe_Vector3F(-1, 0, 0));
        this->m_pCamera->Update(sba_Application->GetGraphics(), sba_Application->GetWindow(), sba_Application->GetTimer(), true);

        int i = 0;
        PuRe_IWindow* window = sba_Application->GetWindow();
        std::string file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        while (file != "")
        {
            i++;
            if (file.substr(file.find_last_of(".") + 1) != "ship")
            {
                file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
                continue;
            }
            std::string namePath = file.substr(0, file.find_last_of(".")).insert(0, this->m_FolderPath);
            std::string previewPath = std::string(namePath).append(".dds");

            std::pair<std::string, PuRe_Sprite*> pair;
            pair.first = namePath; //Set Path

            //Check if preview exists
            std::ifstream preview(previewPath.c_str());
            if (!preview.good())
            { //Create new Sprite on disk
                //Load Ship
                sba::CSpaceship* ship = new sba::CSpaceship(*sba_World, "");
                TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
                serializer->OpenRead(std::string(namePath).append(".ship").c_str());
                ship->Deserialize(*serializer, sba_BrickManager->GetBrickArray(), *sba_World);
                serializer->Close();
                delete serializer;
                //Create & Save Sprite
                PuRe_Sprite* newSprite = this->GetSpriteFromShip(*ship);
                newSprite->GetTexture()->SaveTextureToFile(std::string(namePath).append(".dds").c_str());
                pair.second = newSprite;
                delete ship;
                i++; //Dont load file twice: .dds is positioned before .ship
            }
            else
            {
                //Load Sprite
                pair.second = new PuRe_Sprite(sba_Application->GetGraphics(), std::string(pair.first).append(".dds")); //Load Texture
            }
            this->m_Sprites.push_back(pair);
            file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        }
        if (this->m_Sprites.size() == 0)
        {
            this->AddNewShip("Banana");
        }
        sba_BrickManager->RebuildRenderInstances();
    }


    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddShip(sba::CSpaceship& a_rShip)
    { //Add ship sprite and path and save ship file
        this->m_Sprites.push_back(std::make_pair(std::string(a_rShip.GetName()).insert(0, this->m_FolderPath), this->GetSpriteFromShip(a_rShip)));
        this->SaveShipToFile(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddNewShip(const char* a_pName)
    { //Add default ship with specified name
        std::string name = a_pName;
        bool unique = false;
        while (!unique)
        {
            unique = true;
            std::string path = this->PathFromName(name.c_str());
            for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
            {
                if ((*it).first.compare(path) == 0)
                {
                    unique = false;
                    name += "_";
                    break;
                }
            }
        }
        sba::CSpaceship* ship = new sba::CSpaceship(*sba_World, name.c_str());
        //Register
        this->m_Sprites.push_back(std::make_pair(this->PathFromShip(*ship), this->GetSpriteFromShip(*ship)));
        //Set to default
        this->ResetShip(*ship);
        //Save to file
        this->SaveShipToFile(*ship);
        delete ship;

    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::ResetShip(sba::CSpaceship& a_rShip)
    { //Reset given ship to default and update sprite
        a_rShip.Reset(/*sba_BrickManager->GetBrick(341), *sba_World*/); //Plate6x6
        this->UpdateSprite(a_rShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShipToFile(sba::CSpaceship& a_rShip)
    { //Save given ship to file
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        std::string tmp = std::string(this->PathFromShip(a_rShip)).append(".ship");
        if (serializer->OpenWrite(std::string(this->PathFromShip(a_rShip)).append(".ship").c_str()))
        {
            a_rShip.Serialize(*serializer);
            serializer->Close();
        }
        delete serializer;
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath.append(".dds");
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(path) == 0)
            {
                (*it).second = this->GetSpriteFromShip(a_rShip);
                (*it).second->GetTexture()->SaveTextureToFile(spritePath.c_str());
                return;
            }
        }
    }

#ifdef EDITOR_DEV
    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShipToFileAsObject(sba::CSpaceship& a_rShip)
    { //Save given ship to file
        TheBrick::CSerializer serializer = TheBrick::CSerializer();
        if (serializer.OpenWrite(std::string("../data/objects/").append(a_rShip.GetName() + ".object").c_str()))
        {
            reinterpret_cast<TheBrick::CGameObject&>(a_rShip).Serialize(serializer);
            serializer.Close();
        }
    }
#endif

    // **************************************************************************
    // **************************************************************************
    void CShipManager::DeleteShip(sba::CSpaceship& a_rShip)
    { //Delete given ship from disk and delete sprite and path
        if (remove(std::string(this->PathFromShip(a_rShip)).append(".ship").c_str()) != 0)
        {
            printf("cant delete ship bro");
        }
        std::string spritePath = this->PathFromShip(a_rShip);
        spritePath.append(".dds");
        if (remove(spritePath.c_str()) != 0)
        {
            printf("cant delete dds bro");
        }
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(path) == 0)
            {
                this->m_Sprites.erase(it);
                break;
            }
        }

    }

    // **************************************************************************
    // **************************************************************************
    sba::CSpaceship* CShipManager::GetShip(size_t a_Index)
    { //Load actual ship from disk
        //assert(a_Index >= 0 && a_Index < this->GetShipCount());
        if (a_Index < 0 || a_Index >= this->GetShipCount())
        {
            return nullptr;
        }
        sba::CSpaceship* ship = new sba::CSpaceship(*sba_World, this->m_Sprites[a_Index].first.substr(this->m_Sprites[a_Index].first.find_last_of("/") + 1));
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenRead(std::string(this->m_Sprites[a_Index].first).append(".ship").c_str()))
        {
            ship->Deserialize(*serializer, sba_BrickManager->GetBrickArray(), *sba_World);
            serializer->Close();
        }
        delete serializer;
        return ship;
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::UpdateSprite(sba::CSpaceship& a_rShip)
    {
        std::string path = this->PathFromShip(a_rShip);
        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(path) == 0)
            {
                (*it).second = this->GetSpriteFromShip(a_rShip);
                return;
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::UpdateShipName(sba::CSpaceship& a_rShip, std::string& a_rOldShipName)
    {
        std::string oldPath = this->PathFromName(a_rOldShipName.c_str());
        std::string newPath = this->PathFromShip(a_rShip);
        if (rename(std::string(oldPath).append(".ship").c_str(), std::string(newPath).append(".ship").c_str()) != 0)
        {
            printf("cant rename ship bro");
        }
        if (rename(std::string(oldPath).append(".dds").c_str(), std::string(newPath).append(".dds").c_str()) != 0)
        {
            printf("cant rename dds bro");
        }

        for (std::vector<std::pair<std::string, PuRe_Sprite*>>::iterator it = this->m_Sprites.begin(); it != this->m_Sprites.end(); ++it)
        {
            if ((*it).first.compare(oldPath) == 0)
            {
                it->first = newPath;
                break;
            }
        }
        //!!!#####!!!
        //###Texture Path in PuRe_Sprite in m_Sprites.second is now invalid. Currently doesnt matter. When someone uses that info, he needs to reload the sprite here!
        //!!!#####!!!
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::BatchRenderShip(const sba::CSpaceship& a_rShip, int a_Width, int a_Height) const
    {
        sba_Space->ChangeRenderResolution(a_Width, a_Height);
        sba_BrickManager->RebuildRenderInstances();

        std::string spritePath = std::string("../render/SpacebrickArena_").append(a_rShip.GetName() + "_");
        PuRe_Sprite* sprite;
        Editor::CCamera* camera;
        int c = 0;
        for (int y = -60; y <= 60; y += 20)
        {
            for (int x = 0; x <= 360; x += 20)
            {
                //Path
                std::string path = std::string(spritePath).append(std::to_string(c++) + "_" + std::to_string(y) + "_" + std::to_string(x));
                
                //Setup Camera
                camera = new Editor::CCamera(PuRe_Vector2F((float)a_Width, (float)a_Height), PuRe_Camera_Perspective, 0);
                camera->Initialize(PuRe_Vector3F((float)y, (float)x, 0), PuRe_Vector3F(0, 0, 3.0f));
                camera->Update(sba_Application->GetGraphics(), sba_Application->GetWindow(), sba_Application->GetTimer(), true);
                
                //Render
                Editor::CEditorScene::PreRender(this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, false);
                sba_BrickManager->Render();
                Editor::CEditorScene::PostRender(camera, nullptr, nullptr, this->m_pPostMaterial);
                sprite = new PuRe_Sprite(sba_Application->GetGraphics(), sba_Renderer->GetResult(), true);

                //Save
                sprite->GetTexture()->SaveTextureToFile(path.append(".dds").c_str());

                //Delete
                delete sprite;
                delete camera;
                
                printf("Rendered %i: %s\n", c, path.c_str());
            }
        }
        sba_Space->ChangeRenderResolution();
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Sprite* CShipManager::GetSpriteFromShip(const sba::CSpaceship& a_rShip) const
    {
        sba_Space->ChangeRenderResolution(1024, 576);
        sba_BrickManager->RebuildRenderInstances();
        Editor::CEditorScene::PreRender(this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, false);
        sba_BrickManager->Render();
        Editor::CEditorScene::PostRender(this->m_pCamera, nullptr, nullptr, this->m_pPostMaterial);
        PuRe_Sprite* sprite = new PuRe_Sprite(sba_Application->GetGraphics(), sba_Renderer->GetResult(), true);
        sba_Space->ChangeRenderResolution();
        return sprite;
    }
}