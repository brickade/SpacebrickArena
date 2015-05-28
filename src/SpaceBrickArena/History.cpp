#include "include/History.h"

namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CHistory::CHistory(int a_size, int a_performanceBuffer)
    {
        this->m_PerformanceBuffer = a_performanceBuffer;
        this->m_StorageEnd = a_size + a_performanceBuffer;
        this->m_Storage = new HistoryStep[this->m_StorageEnd-1];
        this->m_CurrentPos = 0;
        this->m_RedoEndPos = 0;
    }


    // **************************************************************************
    // **************************************************************************
    CHistory::~CHistory()
    {
        delete[] this->m_Storage;
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::AddStep(HistoryStep& step)
    {
        //Add to History
        this->m_CurrentPos++;
        this->m_RedoEndPos++;
        if (this->m_CurrentPos == this->m_StorageEnd - this->m_PerformanceBuffer)
        {
            for (int i = 0; i <= this->m_StorageEnd - this->m_PerformanceBuffer; i++)
            {
                this->m_Storage[i] = this->m_Storage[i + this->m_PerformanceBuffer];
            }
            this->m_CurrentPos -= this->m_PerformanceBuffer;
            this->m_RedoEndPos -= this->m_PerformanceBuffer;
        }
        this->m_Storage[this->m_CurrentPos] = step;
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::CutRedos()
    {
        this->m_RedoEndPos = this->m_CurrentPos;
    }

    // **************************************************************************
    // **************************************************************************
    HistoryStep* CHistory::Undo()
    {
        this->m_CurrentPos--;
        if (this->m_CurrentPos < 0)
        {
            this->m_CurrentPos = 0;
            return nullptr;
        }
        return &this->m_Storage[this->m_CurrentPos+1];
    }

    // **************************************************************************
    // **************************************************************************
    HistoryStep* CHistory::Redo()
    {
        this->m_CurrentPos++;
        if (this->m_CurrentPos > this->m_RedoEndPos)
        {
            this->m_CurrentPos = this->m_RedoEndPos;
            return nullptr;
        }
        return &this->m_Storage[this->m_CurrentPos];
    }


}