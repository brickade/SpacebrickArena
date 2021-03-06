#ifndef _EDITORCOLORFIELDS_H_
#define _EDITORCOLORFIELDS_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace sba
{
    class CSpriteReader;
    class CNavigation;
    class CSpaceship;
}

namespace TheBrick
{
    class CGameObject;
    class CBrickInstance;
}

namespace Editor
{
    class CColorFields
    {

    public:

    private:
        int m_PlayerIdx;

        std::vector<PuRe_Color> m_Colors;
        sba::CNavigation* m_pNavigation;

        PuRe_Quad* m_pQuad;

        float m_ElementSize = 20;
        PuRe_Vector2F m_ListStart = PuRe_Vector2F(/*80*/500, 360);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(60, -60);
        
        PuRe_IMaterial* m_pMaterial;

    public:
        CColorFields(int a_PlayerIdx);
        ~CColorFields();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer);
        void Render(PuRe_IGraphics& a_pGraphics, sba::CSpriteReader& a_rSpriteReader, float a_Visibility);

        PuRe_Color& GetCurrentColor();

    private:
        void Add(const PuRe_Color color);
        void Add(float r, float g, float b);
        void Remove(const PuRe_Color color);
    };
}

#endif /* _EDITORCOLORFIELDS_H_ */