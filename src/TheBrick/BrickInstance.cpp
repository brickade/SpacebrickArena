#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickInstance::CBrickInstance(CBrick* a_pBrick, ong::World* a_pWorld, PuRe_Color a_pColor)
    {
        this->m_pBrick = a_pBrick;
        for (size_t i = 0; i < this->m_pBrick->GetColliderData().size(); i++)
        {
            this->m_pCollider.push_back(a_pWorld->createCollider(this->m_pBrick->GetColliderData()[i]));
        }
        this->m_Color = a_pColor;
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance::~CBrickInstance()
    {
        
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickInstance::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        this->m_pBrick->Draw(a_pGraphics, a_pCamera, TheBrick::OngToPuRe(this->m_Transform.p), TheBrick::OngToPuRe(this->m_Transform.q).GetMatrix(), this->m_Color);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_List<SNub>* CBrickInstance::GetWorldSpaceNubs()
    {
        PuRe_List<SNub>* nubs = new PuRe_List<SNub>(this->m_pBrick->GetNubs()); //Copy nubs
        for (unsigned int i = 0; i < nubs->size(); i++)
        {
            SNub* n = &(*nubs)[i];
            n->Position += OngToPuRe(this->m_Transform.p);
            n->Direction *= OngToPuRe(this->m_Transform.q);
            //TODO Die Position der Nubs muss noch um den Pivot des Bricks rotiert werden
        }
        return nubs;
    }
}