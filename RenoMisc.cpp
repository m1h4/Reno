#include "RenoGlobals.h"
#include "RenoMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStringPtrArray::CStringPtrArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CStringPtrArray::~CStringPtrArray()
{
	ASSERT_VALID(this);

	if (m_pData != NULL)
	{
		for( int i = 0; i < m_nSize; i++ )
			delete *(m_pData + i);

		delete[] (BYTE*)m_pData;
	}
}

void CStringPtrArray::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if(nNewSize < 0 )
		AfxThrowInvalidArgException();

	if (nGrowBy >= 0)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			for( int i = 0; i < m_nSize; i++ )
				delete *(m_pData + i);
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create buffer big enough to hold number of requested elements or
		// m_nGrowBy elements, whichever is larger.
#ifdef SIZE_T_MAX
		ASSERT(nNewSize <= SIZE_T_MAX/sizeof(CString*));    // no overflow
#endif
		size_t nAllocSize = __max(nNewSize, m_nGrowBy);
		m_pData = (CString**) new BYTE[(size_t)nAllocSize * sizeof(CString*)];
		memset((void*)m_pData, 0, (size_t)nAllocSize * sizeof(CString*));
		for( int i = 0; i < nNewSize; i++ )
			*( m_pData + i ) = new CString();
		m_nSize = nNewSize;
		m_nMaxSize = nAllocSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			memset((void*)(m_pData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(CString*));
			for( int i = 0; i < nNewSize-m_nSize; i++ )
				*( m_pData + m_nSize + i ) = new CString();
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			for( int i = 0; i < m_nSize-nNewSize; i++ )
				delete *(m_pData + nNewSize + i);
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		INT_PTR nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
		
		if(nNewMax  < m_nMaxSize)
			AfxThrowInvalidArgException();

#ifdef SIZE_T_MAX
		ASSERT(nNewMax <= SIZE_T_MAX/sizeof(CString*)); // no overflow
#endif
		CString** pNewData = (CString**) new BYTE[(size_t)nNewMax * sizeof(CString*)];

		// copy new data from old
		::ATL::Checked::memcpy_s(pNewData, (size_t)nNewMax * sizeof(CString*),
			m_pData, (size_t)m_nSize * sizeof(CString*));

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);
		memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize-m_nSize) * sizeof(CString*));
		for( int i = 0; i < nNewSize-m_nSize; i++ )
			*( pNewData + m_nSize + i ) = new CString();

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

void CStringPtrArray::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(CString*)); // no overflow
#endif
		CString** pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (CString**) new BYTE[m_nSize * sizeof(CString*)];
			// copy new data from old
			::ATL::Checked::memcpy_s(pNewData, m_nSize * sizeof(CString*),
				m_pData, m_nSize * sizeof(CString*));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

void CStringPtrArray::SetAtGrow(INT_PTR nIndex, const CString& newElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	
	if(nIndex < 0)
		AfxThrowInvalidArgException();

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	*m_pData[nIndex] = newElement;
}

void CStringPtrArray::InsertAt(INT_PTR nIndex, const CString& newElement, INT_PTR nCount /*=1*/)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if(nIndex < 0 || nCount <= 0)
		AfxThrowInvalidArgException();

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		INT_PTR nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		for( int i = 0; i < nCount; i++ )
			delete *(m_pData + nOldSize + i);
		// shift old data up to fill gap
		::ATL::Checked::memmove_s(m_pData + nIndex + nCount, (nOldSize-nIndex) * sizeof(CString*),
			m_pData + nIndex, (nOldSize-nIndex) * sizeof(CString*));

		// re-init slots we copied from
		memset((void*)(m_pData + nIndex), 0, (size_t)nCount * sizeof(CString*));
		for( int i = 0; i < nCount; i++ )
			*( m_pData + nIndex + i ) = new CString();
	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		*m_pData[nIndex++] = newElement;
}

void CStringPtrArray::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	INT_PTR nUpperBound = nIndex + nCount;
	ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

	if(nIndex < 0 || nCount < 0 || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
		AfxThrowInvalidArgException();

	// just remove a range
	INT_PTR nMoveCount = m_nSize - (nUpperBound);
	for( int i = 0; i < nCount; i++ )
		delete *(m_pData + nIndex + i);
	if (nMoveCount)
	{
		::ATL::Checked::memmove_s(m_pData + nIndex, (size_t)nMoveCount * sizeof(CString*),
			m_pData + nUpperBound, (size_t)nMoveCount * sizeof(CString*));
	}
	m_nSize -= nCount;
}

#ifdef _DEBUG
void CStringPtrArray::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with " << m_nSize << " elements" << "\n";

	for(LONG i = 0; i < m_nSize; ++i)
		dc << "  " << ElementAt(i) << "\n";
}

void CStringPtrArray::AssertValid() const
{
	CObject::AssertValid();

	if (m_pData == NULL)
	{
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(CString*)));
	}
}
#endif //_DEBUG