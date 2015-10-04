#pragma once

// CStringPtrArray

class CStringPtrArray : public CObject
{
public:
// Construction and destruction
	CStringPtrArray();
	~CStringPtrArray();

// Attributes
	INT_PTR GetSize() const;
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;
	INT_PTR GetUpperBound() const;
	void SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	const CString& GetAt(INT_PTR nIndex) const;
	CString& GetAt(INT_PTR nIndex);
	void SetAt(INT_PTR nIndex,const CString& newElement);
	const CString& ElementAt(INT_PTR nIndex) const;
	CString& ElementAt(INT_PTR nIndex);

	// Direct Access to the element data (may return NULL)
	const CString** GetData() const;
	CString** GetData();

	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex,const CString& newElement);
	INT_PTR Add(const CString& newElement);

	// Overloaded operator helpers
	const CString& operator[](INT_PTR nIndex) const;
	CString& operator[](INT_PTR nIndex);

	// Operations that move elements around
	void InsertAt(INT_PTR nIndex, const CString& newElement, INT_PTR nCount = 1);
	void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);

// Implementation
protected:
	CString** m_pData;   // the actual array of data
	INT_PTR m_nSize;     // # of elements (upperBound - 1)
	INT_PTR m_nMaxSize;  // max allocated
	INT_PTR m_nGrowBy;   // grow amount

public:
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};

AFX_INLINE INT_PTR CStringPtrArray::GetSize() const
	{ return m_nSize; }
AFX_INLINE INT_PTR CStringPtrArray::GetCount() const
	{ return m_nSize; }
AFX_INLINE BOOL CStringPtrArray::IsEmpty() const
	{ return m_nSize == 0; }
AFX_INLINE INT_PTR CStringPtrArray::GetUpperBound() const
	{ return m_nSize-1; }
AFX_INLINE void CStringPtrArray::RemoveAll()
	{ SetSize(0, -1); }
AFX_INLINE CString& CStringPtrArray::GetAt(INT_PTR nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return *m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
AFX_INLINE const CString& CStringPtrArray::GetAt(INT_PTR nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return *m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
AFX_INLINE void CStringPtrArray::SetAt(INT_PTR nIndex, const CString& newElement)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		*m_pData[nIndex] = newElement; 
	else
		AfxThrowInvalidArgException();		
}
AFX_INLINE const CString& CStringPtrArray::ElementAt(INT_PTR nIndex) const
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return *m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
AFX_INLINE CString& CStringPtrArray::ElementAt(INT_PTR nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	if(nIndex >= 0 && nIndex < m_nSize)
		return *m_pData[nIndex]; 
	AfxThrowInvalidArgException();		
}
AFX_INLINE const CString** CStringPtrArray::GetData() const
	{ return (const CString**)m_pData; }
AFX_INLINE CString** CStringPtrArray::GetData()
	{ return (CString**)m_pData; }
AFX_INLINE INT_PTR CStringPtrArray::Add(const CString& newElement)
	{ INT_PTR nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
AFX_INLINE const CString& CStringPtrArray::operator[](INT_PTR nIndex) const
	{ return GetAt(nIndex); }
AFX_INLINE CString& CStringPtrArray::operator[](INT_PTR nIndex)
	{ return ElementAt(nIndex); }