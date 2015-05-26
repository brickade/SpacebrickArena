#include "include/TheBrick/Brick.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    const float CBrick::SEGMENT_WIDTH = 0.78f;
    const float CBrick::SEGMENT_HEIGHT = 0.32f;
    const int CBrick::MAX_MODEL_PATH_LENGTH = 200; //Win max path+filename length = 260

    // **************************************************************************
    // **************************************************************************
    CBrick::CBrick(PuRe_IMaterial* a_pMaterial)
    {
        this->m_ModelPath = new char[MAX_MODEL_PATH_LENGTH];
        memset(this->m_ModelPath, 0, MAX_MODEL_PATH_LENGTH);
        this->m_pMaterial = a_pMaterial;
    }

    // **************************************************************************
    // **************************************************************************
    CBrick::CBrick(PuRe_Model* a_pModel, PuRe_IMaterial* a_pMaterial)
    {
		this->m_ModelPath = new char[MAX_MODEL_PATH_LENGTH];
		memset(this->m_ModelPath, 0, MAX_MODEL_PATH_LENGTH);
        this->m_pModel = a_pModel;
        this->m_pMaterial = a_pMaterial;
    }

    // **************************************************************************
    // **************************************************************************
    CBrick::~CBrick()
    {
        SAFE_DELETE(this->m_pModel);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance* CBrick::CreateInstance(ong::World* a_pWorld)
    {
        return new CBrickInstance(this, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_Vector3F a_Position, PuRe_MatrixF a_Rotation, PuRe_Color a_Color, PuRe_Vector3F a_Scale)
    {
        this->m_pModel->Draw(a_pCamera, this->m_pMaterial, PuRe_Primitive::Triangles, a_Position, a_Scale, a_Rotation, PuRe_Vector3F(0, 0, 0), a_Color);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Deserialize(CSerializer* a_pSerializer, PuRe_IGraphics* a_pGraphics, ong::World* a_pWorld)
    {
        //m_pModel
        this->m_ModelPath = (char*)malloc(MAX_MODEL_PATH_LENGTH);
        a_pSerializer->Read(this->m_ModelPath, MAX_MODEL_PATH_LENGTH);
        this->m_pModel = new PuRe_Model(a_pGraphics, this->m_ModelPath);

        //m_BrickId
        this->m_BrickId = a_pSerializer->ReadIntUnsigned();

        //m_IsTransparent
        this->m_IsTransparent = a_pSerializer->ReadBool();

        //m_PivotOffset
        this->m_Pivotoffset = a_pSerializer->ReadVector3();

        //m_pNubs
		unsigned int numNubs = a_pSerializer->ReadIntUnsigned();
		for (unsigned int i = 0; i < numNubs; i++)
        {
            SNub nub;
            a_pSerializer->Read(&nub, sizeof(nub));
            this->m_pNubs.push_back(nub);
        }

        //m_pColliderData
        unsigned int colliderDataSize = a_pSerializer->ReadIntUnsigned();
        for (unsigned int i = 0; i < colliderDataSize; i++)
        {
            ong::ColliderData colliderData;
            //transform
            a_pSerializer->Read(&colliderData.transform, sizeof(colliderData.transform));
            //pMaterial

            //massData
            a_pSerializer->Read(&colliderData.massData, sizeof(colliderData.massData));
            //aabb
            a_pSerializer->Read(&colliderData.aabb, sizeof(colliderData.aabb));
            //shape
            ong::ShapeDescription shapeDesc;
            shapeDesc.type = a_pSerializer->ReadInt(); //m_type
            switch (shapeDesc.shapeType) //m_shape
            {
            case ong::ShapeType::SPHERE:
                a_pSerializer->Read(&shapeDesc.sphere, sizeof(shapeDesc.sphere));
                break;
            case ong::ShapeType::CAPSULE:
                a_pSerializer->Read(&shapeDesc.capsule, sizeof(shapeDesc.capsule));
                break;
            case ong::ShapeType::HULL:
                ong::Hull* hull = &shapeDesc.hull;
                hull->centroid = PuReToOng(a_pSerializer->ReadVector3());
                //Vertices
                hull->numVertices = a_pSerializer->ReadInt();
                size_t sVertices = sizeof(*hull->pVertices) * hull->numVertices;
                hull->pVertices = (ong::vec3*)malloc(sVertices);
                a_pSerializer->Read(hull->pVertices, sVertices);
                //Edges
                hull->numEdges = a_pSerializer->ReadInt();
                size_t sEdges = sizeof(*hull->pEdges) * hull->numEdges;
                hull->pEdges = (ong::HalfEdge*)malloc(sEdges);
                a_pSerializer->Read(hull->pEdges, sEdges);
                //Faces
                hull->numFaces = a_pSerializer->ReadInt();
                size_t sFaces = sizeof(*hull->pFaces) * hull->numFaces;
                hull->pFaces = (ong::Face*)malloc(sFaces);
                a_pSerializer->Read(hull->pFaces, sFaces);
                
                size_t sPlane = sizeof(*hull->pPlanes) * hull->numFaces;
                hull->pPlanes = (ong::Plane*)malloc(sPlane);
                a_pSerializer->Read(hull->pPlanes, sPlane);
                //epsilon
                hull->epsilon = a_pSerializer->ReadFloat();
                break;
            }
            colliderData.shape = a_pWorld->createShape(shapeDesc);

            ong::Material material;
            material.density = 1.0f;
            material.restitution = 0.6f;
            material.friction = 1.0f;
            ong::Material* mat = a_pWorld->createMaterial(material);

            colliderData.pMaterial = mat;

            this->m_pColliderData.push_back(colliderData);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Serialize(CSerializer* a_pSerializer)
    {
        //m_pModel
        a_pSerializer->Write(this->m_ModelPath, MAX_MODEL_PATH_LENGTH);
        //m_BrickId
        a_pSerializer->Write(this->m_BrickId);
        //m_IsTransparent
        a_pSerializer->Write(this->m_IsTransparent);
        //m_PivotOffset
        a_pSerializer->Write(this->m_Pivotoffset);
        //m_pNubs
        a_pSerializer->Write(this->m_pNubs.size());
        for (unsigned int i = 0; i < this->m_pNubs.size(); i++)
        {
            SNub nub = this->m_pNubs[i];
            a_pSerializer->Write(&nub, sizeof(nub));
        }

        //m_pColliderData
        a_pSerializer->Write(this->m_pColliderData.size());
        for (unsigned int i = 0; i < this->m_pColliderData.size(); i++)
        {
            ong::ColliderData colliderData = this->m_pColliderData[i];
            //transform
            a_pSerializer->Write(&colliderData.transform, sizeof(colliderData.transform));
            //pMaterial

            //massData
            a_pSerializer->Write(&colliderData.massData, sizeof(colliderData.massData));
            //aabb
            a_pSerializer->Write(&colliderData.aabb, sizeof(colliderData.aabb));
            //shape
            a_pSerializer->Write(colliderData.shape.getType()); //m_type
            switch (colliderData.shape.getType()) //m_shape
            {
            case ong::ShapeType::SPHERE:
                a_pSerializer->Write(colliderData.shape.toSphere(), sizeof(*colliderData.shape.toSphere()));
                break;
            case ong::ShapeType::CAPSULE:
                a_pSerializer->Write(colliderData.shape.toCapsule(), sizeof(*colliderData.shape.toCapsule()));
                break;
            case ong::ShapeType::HULL:
                ong::Hull* hull = colliderData.shape.toHull();
                a_pSerializer->Write(OngToPuRe(hull->centroid));
                //Vertices
                a_pSerializer->Write(hull->numVertices);
                a_pSerializer->Write(hull->pVertices, sizeof(*hull->pVertices) * hull->numVertices);
                //Edges
                a_pSerializer->Write(hull->numEdges);
                a_pSerializer->Write(hull->pEdges, sizeof(*hull->pEdges) * hull->numEdges);
                //Faces
                a_pSerializer->Write(hull->numFaces);
                a_pSerializer->Write(hull->pFaces, sizeof(*hull->pFaces) * hull->numFaces);
                a_pSerializer->Write(hull->pPlanes, sizeof(*hull->pPlanes) * hull->numFaces);
                //epsilon
                a_pSerializer->Write(hull->epsilon);
                break;
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    int CBrick::GetCategoryId()
    {
        return floor(this->m_BrickId / 100.0f);
    }
}