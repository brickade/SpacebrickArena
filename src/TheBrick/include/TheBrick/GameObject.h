#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Body.h>
#include <Onager/myMath.h>
#include "BrickData.h"

#include "Brick.h"


namespace sba
{
	class CDestructibleObject;
}

namespace TheBrick
{
    class CBrickInstance;
    class CSerializer;
    enum EGameObjectType {Ship, Bullet, Object, Item};
    class CGameObject
    {
    public:
        std::vector<CBrickInstance*> m_pBricks;
        ong::Body* m_pBody;
        EGameObjectType m_Type;

    private:

    public:
        const ong::Transform GetTransform() const
        {
            return this->m_pBody->getTransform();
        }

    public:
        CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc);
        virtual ~CGameObject();

        virtual void Update(float a_DeltaTime);

        virtual void Deserialize(CSerializer& a_pSerializer, BrickArray& a_rBricks, ong::World& a_pWorld);
        virtual void Serialize(CSerializer& a_pSerializer);

        SBrickData GetBrick(unsigned int a_Index);
        void AddBrick(SBrickData a_Brick, BrickArray& a_rBricks, ong::World& a_pWorld);

        void AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld);
        void RemoveBrickInstance(const CBrickInstance& a_BrickInstance);


		// returns 0 if not destructible else cast to destructible
		virtual sba::CDestructibleObject* GetDestructible() { return nullptr; };

        PuRe_Vector3F PosToObjectSpace(const PuRe_Vector3F& a_rWorldSpacePosition) const;
        PuRe_Vector3F DirToObjectSpace(const PuRe_Vector3F& a_rWorldSpaceDir) const;
        PuRe_Vector3F PosToWorldSpace(const PuRe_Vector3F& a_rObjectSpacePosition) const;
        PuRe_Vector3F DirToWorldSpace(const PuRe_Vector3F& a_rObjectSpaceDir) const;
    };
}

#endif /* _GAMEOBJECT_H_ */