#include "include/TheBrick/Serializer.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CSerializer::CSerializer()
    {

    }

    // **************************************************************************
    // **************************************************************************
    CSerializer::~CSerializer()
    {

    }

    // **************************************************************************
    // **************************************************************************
    bool CSerializer::OpenRead(const char* a_pFile)
    {
#ifdef _DEBUG
        errno_t err = fopen_s(&this->m_pFile, a_pFile, "rb");
        //assert(err == 0);
#else
        this->m_pFile = fopen(a_pFile, "rb");
#endif
        if (this->m_pFile == NULL)
        {
            return false;
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    bool CSerializer::OpenWrite(const char* a_pFile)
    {
#ifdef _DEBUG
        errno_t err = fopen_s(&this->m_pFile, a_pFile, "wb");
        //assert(err == 0);
#else
        this->m_pFile = fopen(a_pFile, "wb");
#endif
        if (this->m_pFile == NULL)
        {
            return false;
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Close()
    {
        fclose(this->m_pFile);
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Read(void* a_pBuffer, size_t a_Size)
    {
        fread(a_pBuffer, 1, a_Size, this->m_pFile);
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(const void* a_pData, size_t a_Size)
    {
        fwrite(a_pData, 1, a_Size, this->m_pFile);
    }

    // **************************************************************************
    // **************************************************************************
    bool CSerializer::ReadBool()
    {
        bool result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    char CSerializer::ReadChar()
    {
        char result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    short CSerializer::ReadShort()
    {
        short result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    int CSerializer::ReadInt()
    {
        int result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    unsigned char CSerializer::ReadCharUnsigned()
    {
        unsigned char result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    unsigned short CSerializer::ReadShortUnsigned()
    {
        unsigned short result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    unsigned int CSerializer::ReadIntUnsigned()
    {
        unsigned int result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    float CSerializer::ReadFloat()
    {
        float result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector2F CSerializer::ReadVector2()
    {
        PuRe_Vector2F result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CSerializer::ReadVector3()
    {
        PuRe_Vector3F result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector4F CSerializer::ReadVector4()
    {
        PuRe_Vector4F result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_QuaternionF CSerializer::ReadQuaternion()
    {
        PuRe_QuaternionF result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_MatrixF CSerializer::ReadMatrix()
    {
        PuRe_MatrixF result;
        this->Read(&result, sizeof(result));
        return result;
    }

    // **************************************************************************
    // **************************************************************************

    void CSerializer::Write(bool a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(char a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(short a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(int a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(unsigned char a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(unsigned short a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(unsigned int a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(float a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(PuRe_Vector2F a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(PuRe_Vector3F a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(PuRe_Vector4F a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(PuRe_QuaternionF a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }

    // **************************************************************************
    // **************************************************************************
    void CSerializer::Write(PuRe_MatrixF a_Data)
    {
        this->Write(&a_Data, sizeof(a_Data));
    }
}