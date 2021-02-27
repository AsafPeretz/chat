#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>  /* size_t */

typedef struct HashMap HashMap;

typedef enum Map_Result {
	Map_Success = 0,
	Map_Uninitialized_Error,
	Map_KeyNull_ERROR,
	Map_KeyDuplicate_Error,
	Map_KeyNotFound_Error,
	Map_Overflow_Error,
	Map_Allocation_Error,
	Map_Unknown_Error
} Map_Result;


typedef size_t (*HashFunction)(const void* _key);
typedef int (*KeyEqualityFunction)(const void* _firstKey, const void* _secondKey);
typedef int	(*KeyValueActionFunction)(const void* _key, void* _value, void* _context);


/**
 * @brief Create a new hash map with given capcity and key characteristics.
 * @param[in] _capacity - Expected max capacity
 * 						  actuall capacity will be equal to nearest larger prime number.
 * @param[in] _hashFunc - hashing function for keys
 * @param[in] _keysEqualFunc - equality check function for keys.
 * @return newly created map or null on failure(capacity = 0,_hashFunc = NULL,
 * _keysEqualFunc = NULL or if the allocation failed)
 */
HashMap* HashMap_Create(size_t _capacity, HashFunction _hashFunc, KeyEqualityFunction _keysEqualFunc);


/**
 * @brief destroy hash map and set *_map to null
 * @param[in] _map : map to be destroyed
 * @param[optional] _keyDestroy : pointer to function to destroy keys
 * @param[optional] _valDestroy : pointer to function to destroy values
 * @details optionally destroy all keys and values using user provided functions
 */
void HashMap_Destroy(HashMap** _map, void (*_keyDestroy)(void* _key), void (*_valDestroy)(void* _value));


/**
 * @brief Adjust map capacity and rehash all key/value pairs
 * @param[in] _map - exisiting map
 * @param[in] _newCapacity - new capacity shall be rounded to nearest larger prime number.
 * @return MAP_SUCCESS or MAP_ALLOCATION_ERROR
 */
Map_Result HashMap_Rehash(HashMap *_map, size_t _newCapacity);


/**
 * @brief Insert a key-value pair into the hash map.
 * complexty(Avearge - O(1),Worst - O(n)),if the _hashFunc that the HashMap_Create got would be good it would be O(1).
 * @param[in] _map - Hash map to insert to, must be initialized
 * @param[in] _key - key to serve as index
 * @param[in] _value - the value to associate with the key
 * @return Success indicator
 * @retval  MAP_SUCCESS	on success
 * @retval  MAP_KEY_DUPLICATE_ERROR	if key alread present in the map
 * @retval  MAP_KEY_NULL_ERROR
 * @retval  MAP_ALLOCATION_ERROR on failure to allocate key-value pair or failure of list allocate.
 * @retval  MAP_OVERFLOW_ERROR if no more space is available
 * @retval  MAP_UNINITIALIZED_ERROR
 *
 * @warning key must be unique and destinct
 */
Map_Result HashMap_Insert(HashMap* _map, const void* _key, const void* _value);


/**
 * @brief Remove a key-value pair from the hash map.
 * complexty(Avearge - O(1),Worst - O(n)),if the _hashFunc that the HashMap_Create got would be good it would be O(1).
 * @param[in] _map - Hash map to remove pair from, must be initialized
 * @param[in] _searchKey - key to to search for in the map
 * @param[out] _pKey - pointer to variable that will get the key stored in the map equaling _searchKey
 * @param[out] _pValue - pointer to variable that will get the value stored in the map corresponding to foind key
 * @return Success indicator
 * @retval  MAP_SUCCESS	on success
 * @retval  MAP_KEY_NULL_ERROR
 * @retval  MAP_KEY_NOT_FOUND_ERROR if key not found
 * @retval  MAP_UNINITIALIZED_ERROR
 *
 * @warning key must be unique and destinct
 */
Map_Result HashMap_Remove(HashMap* _map, const void* _searchKey, void** _pKey, void** _pValue);


/**
 * @brief Find a value by key
  * complexty(Avearge - O(1),Worst - O(n)),if the _hashFunc that the HashMap_Create got would be good it would be O(1).
 * @param[in] _map - Hash map to use, must be initialized
 * @param[in] _searchKey - key to serve as index for search
 * @param[out] _pValue - pointer to variable that will get the value assoiciated with the search key.
 * @return Success indicator
 * @retval  MAP_SUCCESS	on success
 * @retval  MAP_KEY_NULL_ERROR
 * @retval  MAP_KEY_NOT_FOUND_ERROR if key not found
 * @retval  MAP_UNINITIALIZED_ERROR
 *
 * @warning key must be unique and destinct
 */
Map_Result HashMap_Find(const HashMap* _map, const void* _searchKey, void** _pValue);


/**
 * @brief Get number of key-value pairs inserted into the hash map
 * @warning complexity is O(1)
 */
size_t HashMap_Size(const HashMap* _map);
size_t HashMap_Capacity(const HashMap* _map);
size_t HashMap_HashSize(const HashMap* _map);


/**
 * @brief Iterate over all key-value pairs in the map and call a function for each pair
 * The user provided KeyValueActionFunction will be called for each element.
 * Iteration will stop if the called function returns a zero for a given pair
 *
 * @param[in] _map - Hash map to iterate over.
 * @param[in] _action - User provided function pointer to be invoked for each element
 * @param[in] _context - User provided function pointer to be invoked for each element
 * @returns number of times the user functions was invoked
 */
size_t HashMap_ForEach(const HashMap* _map, KeyValueActionFunction _action, void* _context);




typedef struct Map_Stats {
	size_t m_pairs;				/* number of pairs stored */
	size_t m_collisions;			/* total number of collisions encountered */
	size_t m_buckets;				/* total number of buckets */
	size_t m_chains;			/* non empty buckets (having non zero length list) */
	size_t m_maxChainLength;		/* length of longest chain */
	float m_averageChainLength;	/* average length of non-empty chains */
} Map_Stats;
/**
 * @brief return the struct Map_Stats that include all the statistics of the hash
 *
 * @param[in] _map - Hash map to iterate over.
 * @returns the struct Map_Stats.
 */
Map_Stats HashMap_GetMapStatistics(const HashMap* _map);
/**
 * @brief return the number of collisions and average chain length of the hash.
 *
 * @param[in] _map - Hash map to iterate over.
 * @param[out] _pCollisions - pointer to variable that will get the number of collisions.
 * @param[out] _pValue - pointer to variable that will get the average chain length.
 * @return Success indicator
 * @retval  MAP_SUCCESS	on success
 * @retval  MAP_UNINITIALIZED_ERROR
 */
Map_Result HashMap_GetStatistics(const HashMap* _map, size_t *_pCollisions, float *_pAverageChain);

#endif /* HASHMAP_H */
