/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <Box2D/Collision/b2PairManager.h>

// Thomas Wang's hash, see: http://www.concentric.net/~Ttwang/tech/inthash.htm
// This assumes proxyId1 and proxyId2 are 16-bit.
inline uint32 Hash(uint32 proxyId1, uint32 proxyId2)
{
	uint32 key = (proxyId2 << 16) | proxyId1;
	key = ~key + (key << 15);
	key = key ^ (key >> 12);
	key = key + (key << 2);
	key = key ^ (key >> 4);
	key = key * 2057;
	key = key ^ (key >> 16);
	return key;
}

inline bool Equals(const b2Pair& pair, int32 proxyId1, int32 proxyId2)
{
	return pair.proxyId1 == proxyId1 && pair.proxyId2 == proxyId2;
}

b2PairManager::b2PairManager()
{
	b2Assert(b2IsPowerOfTwo(b2_tableCapacity) == true);
	b2Assert(b2_tableCapacity >= b2_maxPairs);
	for (int32 i = 0; i < b2_tableCapacity; ++i)
	{
		m_hashTable[i] = b2_nullPair;
	}
	for (int32 i = 0; i < b2_maxPairs; ++i)
	{
		m_next[i] = b2_nullPair;
	}
	m_pairCount = 0;
}

b2PairManager::~b2PairManager()
{
}

b2Pair* b2PairManager::Find(int32 proxyId1, int32 proxyId2)
{
	if (proxyId1 > proxyId2) b2Swap(proxyId1, proxyId2);

	int32 hash = Hash(proxyId1, proxyId2) & b2_tableMask;

	int32 index = m_hashTable[hash];
	while (index != b2_nullPair && Equals(m_pairs[index], proxyId1, proxyId2) == false)
	{
		index = m_next[index];
	}

	if (index == b2_nullPair)
	{
		return NULL;
	}

	b2Assert(index < m_pairCount);

	return m_pairs + index;
}

inline b2Pair* b2PairManager::Find(int32 proxyId1, int32 proxyId2, uint32 hash)
{
	int32 index = m_hashTable[hash];
	
	while( index != b2_nullPair && Equals(m_pairs[index], proxyId1, proxyId2) == false)
	{
		index = m_next[index];
	}

	if ( index == b2_nullPair )
	{
		return NULL;
	}

	b2Assert(index < m_pairCount);

	return m_pairs + index;
}

b2Pair* b2PairManager::Add(int32 proxyId1, int32 proxyId2)
{
	if (proxyId1 > proxyId2) b2Swap(proxyId1, proxyId2);

	int32 hash = Hash(proxyId1, proxyId2) & b2_tableMask;

	b2Pair* pair = Find(proxyId1, proxyId2, hash);
	if (pair != NULL)
	{
		return pair;
	}

	if (m_pairCount == b2_maxPairs)
	{
		b2Assert(false);
		return NULL;
	}

	pair = m_pairs + m_pairCount;
	pair->proxyId1 = (uint16)proxyId1;
	pair->proxyId2 = (uint16)proxyId2;
	pair->status = 0;
	pair->userData = NULL;

	m_next[m_pairCount] = m_hashTable[hash];
	m_hashTable[hash] = (uint16)m_pairCount;

	++m_pairCount;

	return pair;
}

void* b2PairManager::Remove(int32 proxyId1, int32 proxyId2)
{
	if (proxyId1 > proxyId2) b2Swap(proxyId1, proxyId2);

	int32 hash = Hash(proxyId1, proxyId2) & b2_tableMask;

	const b2Pair* pair = Find(proxyId1, proxyId2, hash);
	if (pair == NULL)
	{
		return NULL;
	}

	void* userData = pair->userData;

	b2Assert(pair->proxyId1 == proxyId1);
	b2Assert(pair->proxyId2 == proxyId2);

	int32 pairIndex = int32(pair - m_pairs);
	b2Assert(pairIndex < m_pairCount);

	// Remove the pair from the hash table.
	int32 index = m_hashTable[hash];
	b2Assert(index != b2_nullPair);

	int32 previous = b2_nullPair;
	while (index != pairIndex)
	{
		previous = index;
		index = m_next[index];
	}

	if (previous != b2_nullPair)
	{
		b2Assert(m_next[previous] == pairIndex);
		m_next[previous] = m_next[pairIndex];
	}
	else
	{
		m_hashTable[hash] = m_next[pairIndex];
	}

	// We now move the last pair into spot of the
	// pair being removed. We need to fix the hash
	// table indices to support the move.

	int32 lastPairIndex = m_pairCount - 1;

	// If the removed pair is the last pair, we are done.
	if (lastPairIndex == pairIndex)
	{
		--m_pairCount;
		return userData;
	}

	// Remove the last pair from the hash table.
	const b2Pair* last = m_pairs + lastPairIndex;
	int32 lastHash = Hash(last->proxyId1, last->proxyId2) & b2_tableMask;

	index = m_hashTable[lastHash];
	b2Assert(index != b2_nullPair);

	previous = b2_nullPair;
	while (index != lastPairIndex)
	{
		previous = index;
		index = m_next[index];
	}

	if (previous != b2_nullPair)
	{
		b2Assert(m_next[previous] == lastPairIndex);
		m_next[previous] = m_next[lastPairIndex];
	}
	else
	{
		m_hashTable[lastHash] = m_next[lastPairIndex];
	}

	// Copy the last pair into the remove pair's spot.
	m_pairs[pairIndex] = m_pairs[lastPairIndex];

	// Insert the last pair into the hash table
	m_next[pairIndex] = m_hashTable[lastHash];
	m_hashTable[lastHash] = (uint16)pairIndex;

	--m_pairCount;

	return userData;
}
