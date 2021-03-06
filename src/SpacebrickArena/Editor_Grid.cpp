#include "include/Editor_Grid.h"

#include <TheBrick/Brick.h>

#include "include/Space.h"
#include "include/Spaceship.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CGrid::CGrid(PuRe_IGraphics& a_rGraphics)
    {
        float zfightingOffset = 0.1f;
        this->m_pGridQuad = new PuRe_Quad(&a_rGraphics);
        
        //Dummy Sprite because there is currently no other way to load textures from file
        PuRe_Sprite* dummySprite = new PuRe_Sprite(&a_rGraphics, "../data/textures/nub_top.png");
        dummySprite->JoinThread();
        this->m_GridTextureTop = a_rGraphics.CreateTexture2D(dummySprite->GetTexture()->GetDescription());
        dummySprite->GetTexture()->Copy(this->m_GridTextureTop, false);
        delete dummySprite;
        
        dummySprite = new PuRe_Sprite(&a_rGraphics, "../data/textures/nub_bottom.png");
        dummySprite->JoinThread();
        this->m_GridTextureBottom = a_rGraphics.CreateTexture2D(dummySprite->GetTexture()->GetDescription());
        dummySprite->GetTexture()->Copy(this->m_GridTextureBottom, false);
        delete dummySprite;

        this->m_pGridMaterial = a_rGraphics.LoadMaterial("../data/effects/editor/grid");
        this->m_GridRenderInstanceCount = (sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1) * (sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1);
        this->m_pGridRenderInstancesTop = new PuRe_RenderInstance[this->m_GridRenderInstanceCount];
        this->m_pGridRenderInstancesBottom = new PuRe_RenderInstance[this->m_GridRenderInstanceCount];


        float s = TheBrick::CBrick::SEGMENT_WIDTH * 0.5f;

        PuRe_Vector3F size = PuRe_Vector3F(s, s, s);
        PuRe_Color color = PuRe_Color(1, 1, 1, 1);
        PuRe_MatrixF rotTop = PuRe_MatrixF::Rotation(1.57079633f, 0, 0); //90 deg
        PuRe_MatrixF rotBottom = PuRe_MatrixF::Rotation(-1.57079633f, 0, 0); //-90 deg
        for (int x = 0; x < sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1; x++)
        {
            for (int y = 0; y < sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1; y++)
            {
                PuRe_RenderInstance& instanceTop = this->m_pGridRenderInstancesTop[y * (sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1) + x];
                PuRe_RenderInstance& instanceBottom = this->m_pGridRenderInstancesBottom[y * (sba::CSpaceship::MAX_BRICK_WIDTH * 2 + 1) + x];

                instanceTop.Position = PuRe_Vector3F((x - sba::CSpaceship::MAX_BRICK_WIDTH) * TheBrick::CBrick::SEGMENT_WIDTH, -zfightingOffset, (y - sba::CSpaceship::MAX_BRICK_WIDTH) * TheBrick::CBrick::SEGMENT_WIDTH);
                instanceTop.Size = size;
                instanceTop.Color = color;
                instanceTop.Center = PuRe_Vector3F::Zero();
                instanceTop.Rotation = rotTop;

                instanceBottom.Position = instanceTop.Position;
                instanceBottom.Size = size;
                instanceBottom.Color = color;
                instanceBottom.Center = PuRe_Vector3F::Zero();
                instanceBottom.Rotation = rotBottom;
            }
        }



        dummySprite = new PuRe_Sprite(&a_rGraphics, "../data/textures/grid_arrow.png");
        dummySprite->JoinThread();
        this->m_ArrowTexture = a_rGraphics.CreateTexture2D(dummySprite->GetTexture()->GetDescription());
        dummySprite->GetTexture()->Copy(this->m_ArrowTexture, false);
        delete dummySprite;

        this->m_ArrowRenderInstanceCount = ((sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1) * ((sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1);
        this->m_pArrowRenderInstancesTop = new PuRe_RenderInstance[this->m_ArrowRenderInstanceCount];
        this->m_pArrowRenderInstancesBottom = new PuRe_RenderInstance[this->m_ArrowRenderInstanceCount];

        s = TheBrick::CBrick::SEGMENT_WIDTH * 0.5f * this->m_ArrowSize;
        size = PuRe_Vector3F(s, s, s);
        color = PuRe_Color(1, 1, 1, 1);
        rotTop = PuRe_MatrixF::Rotation(1.57079633f, 0, 0); //90 deg
        rotBottom = PuRe_MatrixF::Rotation(-1.57079633f, 3.14159265f, 0); //-90 deg
        for (int x = 0; x < (sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1; x++)
        {
            for (int y = 0; y < (sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1; y++)
            {
                PuRe_RenderInstance& instanceTop = this->m_pArrowRenderInstancesTop[y * ((sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1) + x];
                PuRe_RenderInstance& instanceBottom = this->m_pArrowRenderInstancesBottom[y * ((sba::CSpaceship::MAX_BRICK_WIDTH * 2) / this->m_ArrowSize + 1) + x];

                instanceTop.Position = PuRe_Vector3F((x * this->m_ArrowSize - sba::CSpaceship::MAX_BRICK_WIDTH) * TheBrick::CBrick::SEGMENT_WIDTH, zfightingOffset, (y * this->m_ArrowSize - sba::CSpaceship::MAX_BRICK_WIDTH) * TheBrick::CBrick::SEGMENT_WIDTH);
                instanceTop.Size = size;
                instanceTop.Color = color;
                instanceTop.Center = PuRe_Vector3F::Zero();
                instanceTop.Rotation = rotTop;

                instanceBottom.Position = PuRe_Vector3F(instanceTop.Position.X, -instanceTop.Position.Y * 2, instanceTop.Position.Z - TheBrick::CBrick::SEGMENT_WIDTH); //Position because flipped and center in edge
                instanceBottom.Size = size;
                instanceBottom.Color = color;
                instanceBottom.Center = PuRe_Vector3F::Zero();
                instanceBottom.Rotation = rotBottom;
            }
        }
    }


    // **************************************************************************
    // **************************************************************************
    CGrid::~CGrid()
    {

        SAFE_DELETE_ARRAY(this->m_pArrowRenderInstancesBottom);
        SAFE_DELETE_ARRAY(this->m_pArrowRenderInstancesTop);
        SAFE_DELETE(this->m_ArrowTexture);
        SAFE_DELETE_ARRAY(this->m_pGridRenderInstancesBottom);
        SAFE_DELETE_ARRAY(this->m_pGridRenderInstancesTop);
        SAFE_DELETE(this->m_pGridMaterial);
        SAFE_DELETE(this->m_GridTextureBottom);
        SAFE_DELETE(this->m_GridTextureTop);
        SAFE_DELETE(this->m_pGridQuad);
    }

    // **************************************************************************
    // **************************************************************************
    void CGrid::Draw(bool a_DrawArrow)
    {
        //Grid
        sba_Renderer->SetTexture(0, this->m_GridTextureTop, "tex");
        sba_Renderer->Draw(0, true, this->m_pGridQuad->GetBuffer(), 4, PuRe_Primitive::Trianglestrip, this->m_pGridMaterial, this->m_pGridRenderInstancesTop, this->m_GridRenderInstanceCount);

        sba_Renderer->SetTexture(0, this->m_GridTextureBottom, "tex");
        sba_Renderer->Draw(0, true, this->m_pGridQuad->GetBuffer(), 4, PuRe_Primitive::Trianglestrip, this->m_pGridMaterial, this->m_pGridRenderInstancesBottom, this->m_GridRenderInstanceCount);

        //Arrow
        if (a_DrawArrow)
        {
            sba_Renderer->SetTexture(0, this->m_ArrowTexture, "tex");
            sba_Renderer->Draw(0, true, this->m_pGridQuad->GetBuffer(), 4, PuRe_Primitive::Trianglestrip, this->m_pGridMaterial, this->m_pArrowRenderInstancesTop, this->m_ArrowRenderInstanceCount);

            sba_Renderer->SetTexture(0, this->m_ArrowTexture, "tex");
            sba_Renderer->Draw(0, true, this->m_pGridQuad->GetBuffer(), 4, PuRe_Primitive::Trianglestrip, this->m_pGridMaterial, this->m_pArrowRenderInstancesBottom, this->m_ArrowRenderInstanceCount);
        }
    }
  
}