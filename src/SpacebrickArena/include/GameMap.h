#ifndef _GAMEMAP_H_
#define _GAMEMAP_H_

#include <string>
#include "Asteroid.h"
#include "Static.h"
#include "Item.h"

#include "TheBrick/Conversion.h"

namespace sba
{
    enum ELightType{ Directional,Point};
    struct SLightData
    {
        ELightType Type;
        PuRe_Vector3F Position;
        PuRe_Color Color;
        float Radius;
        float Intensity;
    };

    class CGameMap
    {
    private:
        std::string m_Name;
    public:
        void SetMap(const char* a_pFile);
    public:
        CGameMap();
        ~CGameMap();
    public:
        bool GetMapData(std::vector<CStatic*>& a_rStatics, std::vector<CAsteroid*>& a_rObjects, std::vector<CItem*>& a_rItems, std::vector<SLightData*>& a_rLights);
        std::string GetSkybox();
        std::string GetName();
    };
}

#endif /* _GAMEMAP_H_ */