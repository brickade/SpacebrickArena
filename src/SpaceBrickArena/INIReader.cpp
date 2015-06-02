#include "include/INIReader.h"

namespace Game
{
    CIniReader* CIniReader::g_pInstance = 0;

    // **************************************************************************
    // **************************************************************************
    CIniReader* CIniReader::Instance()
    {
        if (CIniReader::g_pInstance == nullptr)
        {
            assert(CIniReader::g_pInstance == nullptr);
        }
        return CIniReader::g_pInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void CIniReader::SetPath(const char* a_pFile)
    {
        if (CIniReader::g_pInstance == nullptr)
        {
            CIniReader::g_pInstance = new CIniReader(a_pFile);
        }
    }

    // **************************************************************************
    // **************************************************************************
    CIniReader::CIniReader(const char* a_pFile)
    {
        FILE* pFile;
        long size = 0;
        char* buffer = nullptr;
        bool readKey = true;
        std::string Key;
        std::string Value;
        //open file
        pFile = fopen(a_pFile, "r");
        if (pFile != NULL)
        {
            //Get file size
            fseek(pFile, 0, SEEK_END);
            size = ftell(pFile);
            rewind(pFile);

            //allocate buffer and copy file data into it
            buffer = (char*)malloc(sizeof(char)*size);
            fread(buffer, 1, size, pFile);
            //close file
            fclose(pFile);

            //handle buffer
            for (int i = 0; i < size; i++)
            {
                if (buffer[i] == '=')
                    readKey = false;
                else if (buffer[i] == '\r')
                {
                    i++;
                    //insert
                    if (Key != ""&&Value != "")
                        this->m_Table[Key] = Value;
                    //reset strings
                    Key.clear();
                    Value.clear();

                    readKey = true;
                }
                else
                {
                    if (readKey)
                        Key += buffer[i];
                    else
                        Value += buffer[i];
                }
            }
            //deallocate buffer
            free(buffer);
        }
        else
        {
            auto error = fopen_s(&pFile, a_pFile, "w"); //always writes at the end, created if it does not exist
            //Ini File doesn't exist, create new
            if (pFile != NULL)
            {
                this->m_Table["ResolutionWidth"] = "1920";
                this->m_Table["ResolutionHeight"] = "1080";
                this->m_Table["WindowWidth"] = "1024";
                this->m_Table["WindowHeight"] = "768";
                this->m_Table["DisplayMode"] = "Windowed";
                this->m_Table["SSAO"] = "On";
                std::string write;
                for (auto it = this->m_Table.begin(); it != this->m_Table.end(); ++it)
                {
                    write = it->first + "=" + it->second +"\r\n";
                    fwrite(write.c_str(), sizeof(char), write.length(), pFile);
                }
                fclose(pFile);
            }

        }
    }

    // **************************************************************************
    // **************************************************************************
    CIniReader::~CIniReader()
    {
    }

    // **************************************************************************
    // **************************************************************************
    std::string CIniReader::GetValue(std::string a_Key)
    {
        return this->m_Table[a_Key];
    }

}