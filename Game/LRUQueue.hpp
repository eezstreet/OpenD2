#pragma once
#include "../Shared/D2Shared.hpp"

///////////////////////////////////////////
//
//	LRU (least-recently used) queues are used on the renderer for DCCs.
//

class LRUQueueItem
{
public:
	LRUQueueItem(handle i, int d) { itemHandle = i; nDirection = d; }

	handle GetHandle() { return itemHandle; }
	int GetDirection() { return nDirection; }

	LRUQueueItem* pNext;
	LRUQueueItem* pPrev;

protected:
	handle itemHandle;
	int	nDirection;
};

template<typename T>
class LRUQueue
{
private:
	T* pHead;
	T* pTail;

	DWORD dwHitCount;
	DWORD dwMissCount;
	DWORD dwQueryCount;
	DWORD dwLRUSize;
	DWORD dwInUseCount;

	void MoveToFront(T* pItem)
	{
		// If this thing is the front, we don't need to do anything
		if (pItem == pHead)
		{
			return;
		}

		// Move this item out of its current queue slot (if it exists)
		if (pItem->pPrev)
		{
			pItem->pPrev->pNext = pItem->pNext;
		}
		if (pItem->pNext)
		{
			pItem->pNext->pPrev = pItem->pPrev;
		}

		// If this thing was the tail, we need to set it to the previous thing
		if (pItem == pTail)
		{
			pTail = (T*)pItem->pPrev;
		}

		// Fix the pointers on the item so it points in the correct directions
		pItem->pPrev = nullptr;
		pItem->pNext = pHead;

		// Make the tail fall off if we exceed the size
		if (dwInUseCount > dwLRUSize)
		{
			T* pOldTail = pTail;
			pTail = (T*)pTail->pPrev;
			delete pOldTail;
			dwInUseCount = dwLRUSize;
		}

		// Insert this thing as the head
		if (pHead)
		{
			pHead->pPrev = pItem;
		}
		pHead = pItem;

		// ...and possibly as the tail too if there isn't one
		if (pTail == nullptr)
		{
			pTail = pHead;
		}
	}

public:
	LRUQueue(DWORD dwInitialQueueSize)
	{
		dwHitCount = dwMissCount = dwQueryCount = dwInUseCount = 0;
		dwLRUSize = dwInitialQueueSize;
		pHead = pTail = nullptr;
	}

	~LRUQueue()
	{
		T* pCurrent = pHead;
		T* pPrev = nullptr;
		while (pCurrent != nullptr)
		{
			pPrev = pCurrent;
			pCurrent = (T*)pCurrent->pNext;
			delete pPrev;
		}
	}

	T* QueryItem(handle itemHandle, int nDirection)
	{
		// see if it's in the LRU first
		T* pCurrent = pHead;

		dwQueryCount++;

		while (pCurrent != nullptr)
		{
			if (pCurrent->GetHandle() == itemHandle && pCurrent->GetDirection() == nDirection)
			{
				// Mark this as a hit...
				dwHitCount++;
				// ...move it to the front...
				MoveToFront(pCurrent);
				// ... and return it
				return pCurrent;
			}
			pCurrent = (T*)pCurrent->pNext;
		}

		// MISS!
		// we need to make a new LRU item and push it to the front
		dwMissCount++;

		pCurrent = new T(itemHandle, nDirection);
		dwInUseCount++;
		MoveToFront(pCurrent);
		return pCurrent;
	}
};