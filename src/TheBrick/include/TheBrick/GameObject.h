#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Body.h>
#include <Onager/myMath.h>

#include "BrickInstance.h"


namespace TheBrick
{
    class CGameObject
    {
    public:

    private:
        PuRe_List<CBrickInstance*>* m_pBricks;
        ong::Transform* m_pTransform;
        ong::Body* m_pBody;

    public:
        CGameObject();
        virtual ~CGameObject();

        virtual void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);
    };
}

#endif /* _GAMEOBJECT_H_ */