/* MIT License */

/*
Copyright (C) <2014> <Zillion Shen zillionshen@yahoo.com>

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <cstddef>
#include <string>
#include <cstdint>

using namespace std;

#define EMPTY_SLOT 0
#define DELETED_SLOT (KeyType)UINT32_MAX

template<typename KeyType, typename containerPtrType>
class ZillionSetIterator
{
	uint32_t m_index;
	containerPtrType m_set;
public:
	ZillionSetIterator(uint32_t index, containerPtrType zillionSet)
	{
		m_index = index;
		m_set = zillionSet;
	}
	ZillionSetIterator(const ZillionSetIterator& it)
	{
		m_index = it.m_index;
		m_set = it.m_set;
	}
	ZillionSetIterator& operator++()
	{
		m_index = NextOccupiedIndex(m_index);
		return*this;
	}
	ZillionSetIterator& operator++(int)
	{
		m_index = NextOccupiedIndex(m_index);
		return*this;
	}
	bool operator==(const ZillionSetIterator& it)
	{
		return (m_index == it.m_index && m_set == it.m_set);
	}
	bool operator!=(const ZillionSetIterator& it)
	{
		return (m_index != it.m_index || m_set != it.m_set);
	}
	KeyType& operator*()
	{
		return*(m_set->m_keyPtrs[m_index]);
	}
};

template<typename KeyType>
class ZillionIntSet
{
public:
	typedef ZillionIntSet* ZillionIntSetPtr;
	typedef ZillionSetIterator<KeyType, ZillionIntSetPtr> iterator;
	friend class ZillionSetIterator<KeyType, ZillionIntSetPtr>;

private:
	KeyType* m_keys;
	uint32_t m_count;
	uint32_t m_capacity;
	uint32_t m_deleted;

	KeyType& findKeyOrEmptyOrDeletedSlot(KeyType* keys, int32_t capacity, KeyType& key)
	{
		uint32_t index = key % capacity;
		while (keys[index] != EMPTY_SLOT && keys[index] != DELETED_SLOT && keys[index] != key)
			index = ++index % capacity;
		return keys[index];
	}
	KeyType& findKeyOrEmptySlot(KeyType* keys, int32_t capacity, KeyType& key)
	{
		uint32_t index = key % capacity;
		while (keys[index] != EMPTY_SLOT && keys[index] != key)
			index = ++index % capacity;
		return keys[index];
	}
	void rebuild(uint32_t newCapacity)
	{
		KeyType* newKeys = new KeyType[newCapacity];
		memset(newKeys, 0, sizeof(uint32_t)* (newCapacity));

		//relocate the existing keys
		for (uint32_t i = 0; i < m_capacity; i++)
		{
			if (m_keys[i] == EMPTY_SLOT || m_keys[i] == DELETED_SLOT)
				continue;
			else
			{
				KeyType& slot = findKeyOrEmptySlot(newKeys, newCapacity, m_keys[i]);
				slot = m_keys[i];
			}
		}

		if (m_keys != NULL) delete[] m_keys;
		m_keys = newKeys;
		m_capacity = newCapacity;
		m_deleted = 0;
	}
	void expand()
	{
		rebuild(nextPrime(m_capacity));
	}
	static void getPrimeTable(const uint32_t*& primes, uint32_t& count)
	{
		static const uint32_t primeList[] =
		{
			0ul, 3ul, 7ul, 11ul,
			23ul, 47ul, 97ul, 197ul,
			397ul, 797ul, 1597ul, 3191ul,
			6379ul, 12757ul, 25471ul, 50929ul,
			101839ul, 203669ul, 407321ul, 814643ul,
			1629281ul, 3258551ul, 6517097ul, 13034191ul,
			26068369ul, 52136729ul, 104273459ul, 208546913ul,
			312820367ul, 469230529ul, 703845773ul, 1055768627ul,
			1583652929ul, 2375479373ul, 3563219059ul, 4294967295ul
		};
		primes = primeList;
		count = sizeof(primeList) / sizeof(uint32_t);
	}
	static uint32_t nextPrime(uint32_t currentprime)
	{
		const uint32_t* primes; uint32_t count;
		getPrimeTable(primes, count);

		if (currentprime >= primes[count - 1]) return primes[count - 1];

		uint32_t l = 0, h = count - 1, mid;
		while (l <= h)
		{
			mid = (l + h) / 2;
			if (currentprime == primes[mid]) return primes[mid + 1];
			else if (currentprime > primes[mid]) l = mid + 1;
			else h = mid - 1;
		}

		return primes[l > h ? l : h];
	}
	uint32_t NextOccupiedIndex(uint32_t currentIndex)
	{
		for (currentIndex++; currentIndex < m_capacity; currentIndex++)
		{
			if (m_keyPtrs[currentIndex] != EMPTY_SLOT || currentIndex[currentIndex] == DELETED_SLOT)
				return currentIndex;
		}
		return UINT32_MAX;
	}
public:
	ZillionIntSet()
	{
		m_keys = NULL;
		m_count = 0;
		m_capacity = 0;
		m_deleted = 0;
	}
	~ZillionIntSet()
	{
		if (m_keys != NULL) delete[] m_keys;
	}
	void clear()
	{
		if (m_keys != NULL) delete[] m_keys;
		m_keys = NULL;
		m_count = 0;
		m_capacity = 0;
		m_deleted = 0;
	}
	bool exists(KeyType key)
	{
		if (m_capacity == 0) return false;
		KeyType& slot = findKeyOrEmptySlot(m_keys, m_capacity, key);
		return (slot == key);
	}
	iterator find(KeyType key)
	{
		uint32_t index = key % m_capacity;
		while (m_keys[index] != EMPTY_SLOT && m_keys[index] != key)
			index = ++index % m_capacity;
		return iterator(m_keys[index] == key ? index : UINT32_MAX, this);
	}
	void insert(ZillionIntSet& zillionSet)
	{
		for (auto it : zillionSet) this->insert(it);
	}
	void insert(KeyType key)
	{
		if (key == EMPTY_SLOT) throw exception("Can't use zero value as the key");
		if (key == DELETED_SLOT) throw exception("Can't use maximum value as the key");
		if (m_count + m_deleted >= (uint32_t)(m_capacity* 0.8))
		{
			if (m_deleted != 0 && m_deleted >= (uint32_t)(m_count* 0.8))
				rebuild(nextPrime(m_count));
			else
				expand();
		}
		KeyType& slot = findKeyOrEmptyOrDeletedSlot(m_keys, m_capacity, key);
		if (slot == key) return; //key exists
		if (slot == DELETED_SLOT) m_deleted--;
		m_count++;
		slot = key;
	}
	void erase(KeyType key)
	{
		if (m_capacity == 0) return;
		uint32_t index = key % m_capacity;
		while (m_keys[index] != EMPTY_SLOT)
		{
			if (m_keys[index] == key)
			{
				m_keys[index] = DELETED_SLOT;
				m_count--;
				m_deleted++;
				if (m_deleted > m_count) rebuild(nextPrime(m_count));
			}
			index = ++index % m_capacity;
		}
	}
	iterator begin()
	{
		for (uint32_t i = 0; i < m_capacity; i++)
		{
			if (m_keys[i] != EMPTY_SLOT && m_keys[i] != DELETED_SLOT)
				return iterator(i, this);
		}
		return end();
	}
	iterator end()
	{
		return iterator(UINT32_MAX, this);
	}
	uint32_t capacity()
	{
		return m_capacity;
	}
	uint32_t size()
	{
		return m_count;
	}
	bool operator ==(const ZillionIntSet& right)
	{
		ZillionIntSet& rr = const_cast<ZillionIntSet&>(right);

		int count = 0;
		for (iterator it = rr.begin(); it != rr.end(); ++it)
		{
			if (!this->exists(*it)) return false;
			else count++;
		}

		return count == m_count;
	}
	bool operator !=(const ZillionIntSet& right)
	{
		ZillionIntSet& rr = const_cast<ZillionIntSet&>(right);

		int count = 0;
		for (iterator it = rr.begin(); it != rr.end(); ++it)
		{
			if (!this->exists(*it)) return true;
			else count++;
		}

		return count != m_count;
	}
};
