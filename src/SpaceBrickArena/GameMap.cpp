#include "include/GameMap.h"

#include "include/Space.h"

namespace sba
{

    // **************************************************************************
    // **************************************************************************
    CGameMap::CGameMap()
    {
        this->m_Name = "cygnus";
    }

    // **************************************************************************
    // **************************************************************************
    CGameMap::~CGameMap()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CGameMap::SetMap(const char* a_pDirectory)
    {
        this->m_Name = a_pDirectory;
    }

    // **************************************************************************
    // **************************************************************************
    std::string CGameMap::GetName()
    {
        return this->m_Name;
    }

    // **************************************************************************
    // **************************************************************************
    std::string CGameMap::GetSkybox()
    {
        return "../data/textures/skybox/" + this->m_Name + "/";
    }

    // **************************************************************************
    // **************************************************************************
    bool CGameMap::GetMapData(std::vector<CAsteroid*>& a_rObjects)
    {
        FILE* pFile;
        std::string mapPath = "../data/maps/" + this->m_Name + ".map";
        pFile = fopen(mapPath.c_str(), "r");

        if (pFile != NULL)
        {
            long size = 0;
            char* buffer = nullptr;
            //Get file size
            fseek(pFile, 0, SEEK_END);
            size = ftell(pFile);
            rewind(pFile);

            //allocate buffer and copy file data into it
            buffer = (char*)malloc(sizeof(char)*size);
            fread(buffer, 1, size, pFile);
            //close file
            fclose(pFile);

            std::string objectName;
            PuRe_Vector3F pos, vel, rot;
            enum read { Name, Pos, Vel, Rot };
            enum readC { One, Two };

            std::string buff = "";
            read readWhere = Name;
            readC readCount = One;
            PuRe_Vector3F* changed = NULL;
            for (int i = 0; i <= size; i++)
            {
                if (i == size||buffer[i] == '\n')
                {
                    changed->Z = std::stof(buff);
                    if (objectName != "")
                    {
                        sba::CAsteroid* asteroid = new sba::CAsteroid(*sba_World, TheBrick::PuReToOng(pos));
                        TheBrick::CSerializer serializer;
                        serializer.OpenRead(objectName.c_str());
                        asteroid->Deserialize(serializer, sba_BrickManager->GetBrickArray(), *sba_World);
                        a_rObjects.push_back(asteroid);
                        serializer.Close();
                    }
                    readWhere = read::Name;
                    readCount = readC::One;
                    changed = &pos;
                    objectName = "";
                    buff = "";
                }
                else if (buffer[i] == ';')
                {
                    switch (readWhere)
                    {
                    case read::Name:
                        objectName = "../data/objects/" + buff + ".object";
                        readWhere = read::Pos;
                        readCount = readC::One;
                        changed = &pos;
                        break;
                    case read::Pos:
                        changed->Z = std::stof(buff);
                        readWhere = read::Vel;
                        readCount = readC::One;
                        changed = &vel;
                        break;
                    case read::Vel:
                        changed->Z = std::stof(buff);
                        readWhere = read::Rot;
                        readCount = readC::One;
                        changed = &rot;
                        break;
                    }
                    buff = "";
                }
                else if (buffer[i] == ',')
                {
                    if (readWhere == read::Name)
                        buff += buffer[i];
                    else
                    {
                        switch (readCount)
                        {
                        case readC::One:
                            changed->X = std::stof(buff);
                            readCount = readC::Two;
                            break;
                        case readC::Two:
                            changed->Y = std::stof(buff);
                            break;
                        }
                        buff = "";
                    }
                }
                else
                    buff += buffer[i];
            }
        }
        else
            return false;
        return true;
    }
}