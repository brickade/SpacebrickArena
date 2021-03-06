#include "include/TheBrick/GameObject.h"

#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Serializer.h"
#include "include/TheBrick/DebugDraw.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CGameObject::CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc)
    {
        if (a_pBodyDesc == nullptr)
        {
            ong::BodyDescription body;
            body.transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
            body.type = ong::BodyType::Dynamic;
            body.angularMomentum = ong::vec3(0, 0, 0);
            body.linearMomentum = ong::vec3(0, 0, 0);
			body.continuousPhysics = false;
            a_pBodyDesc = &body;
        }
        ong::Body* body = a_rWorld.createBody(*a_pBodyDesc);
        body->setUserData(this);
        this->m_pBody = body;
    }

    // **************************************************************************
    // **************************************************************************
    CGameObject::~CGameObject()
    {
        //Delete BrickInstances

        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            //BrickInstance calls "RemoveBrickInstance", which removes the brick instance from the vector
            //so DELETE also calls "ERASE" from vector here!!
            SAFE_DELETE(this->m_pBricks[i]);
            i--;
        }
        this->m_pBricks.clear();
        this->m_pBody->getWorld()->destroyBody(this->m_pBody);
        this->m_pBody = nullptr;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Update(float a_DeltaTime)
    {

    }

    // **************************************************************************
    // **************************************************************************
    SBrickData CGameObject::GetBrick(unsigned int a_Index)
    {
        SBrickData BrickData;
        if (this->m_pBricks.size() > a_Index)
        {
            CBrickInstance* brick = this->m_pBricks[a_Index];

            //Brick ID
            BrickData.ID = brick->m_pBrick->GetBrickId();
            //m_Transform
            BrickData.Transform = brick->GetTransform();
            //m_Color
            BrickData.Color = brick->m_Color;
        }
        return BrickData;
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::AddBrick(SBrickData a_Brick, BrickArray& a_rBricks, ong::World& a_pWorld)
    {
        CBrickInstance* brick = new CBrickInstance(*a_rBricks[a_Brick.ID], *this, a_pWorld);
        //m_Transform
        brick->SetTransform(a_Brick.Transform);
        //m_Color
        brick->m_Color = a_Brick.Color;
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::Deserialize(CSerializer& a_pSerializer, BrickArray& a_rBricks, ong::World& a_pWorld)
    {
        //m_pBricks
        unsigned int bricksSize = a_pSerializer.ReadIntUnsigned();
        for (unsigned int i = 0; i < bricksSize; i++)
        {
            //m_pBrick
            int brickId = a_pSerializer.ReadInt();
            CBrickInstance* brick = new CBrickInstance(*a_rBricks[brickId], *this, a_pWorld);
            //m_Transform
            ong::Transform transform;
            a_pSerializer.Read(&transform, sizeof(transform));
            brick->SetTransform(transform);
            //m_Color
            a_pSerializer.Read(&brick->m_Color, sizeof(*&brick->m_Color));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Serialize(CSerializer& a_pSerializer)
    {
        //m_pBricks
        a_pSerializer.Write(this->m_pBricks.size());
        for (unsigned int i = 0; i < this->m_pBricks.size(); i++)
        {
            //m_pBrick
            CBrickInstance* brick = this->m_pBricks[i];
            a_pSerializer.Write(brick->m_pBrick->GetBrickId());
            //m_Transform
            a_pSerializer.Write(&brick->GetTransform(), sizeof(*&brick->GetTransform()));
            //m_Color
            a_pSerializer.Write(&brick->m_Color, sizeof(*&brick->m_Color));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld)
    {
        for (size_t i = 0; i < a_pBrickInstance->m_pCollider.size(); i++)
        {
            this->m_pBody->addCollider(a_pBrickInstance->m_pCollider[i]);
        }
        this->m_pBricks.push_back(a_pBrickInstance);
        a_pBrickInstance->SetGameObject(this);
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::RemoveBrickInstance(const CBrickInstance& a_BrickInstance)
    {
        for (size_t i = 0; i < a_BrickInstance.m_pCollider.size(); i++)
        {
            this->m_pBody->removeCollider(a_BrickInstance.m_pCollider[i]);
        }
        for (std::vector<CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if (*it == &a_BrickInstance)
            {
                this->m_pBricks.erase(it);
                break;
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CGameObject::PosToObjectSpace(const PuRe_Vector3F& a_rWorldSpacePosition) const
    {
        return OngToPuRe(ong::invTransformVec3(PuReToOng(a_rWorldSpacePosition), this->GetTransform()));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CGameObject::DirToObjectSpace(const PuRe_Vector3F& a_rWorldSpaceDir) const
    {
        return OngToPuRe(ong::rotate(PuReToOng(a_rWorldSpaceDir), ong::conjugate(this->GetTransform().q)));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CGameObject::PosToWorldSpace(const PuRe_Vector3F& a_rObjectSpacePosition) const
    {
        return OngToPuRe(ong::transformVec3(PuReToOng(a_rObjectSpacePosition), this->GetTransform()));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CGameObject::DirToWorldSpace(const PuRe_Vector3F& a_rObjectSpaceDir) const
    {
        return OngToPuRe(ong::rotate(PuReToOng(a_rObjectSpaceDir), this->GetTransform().q));
    }
}