#ifndef LIST_H
#define LIST_H

#include <stddef.h>  /* size_t */

#define TRUE 1
#define FALSE 0

typedef struct List List;

typedef enum List_Result {
	List_Success = 0,
	List_UUninitialized_Error,
	List_Allocation_Error,
	List_Underflow_Error,
	List_Unknown_Error
} List_Result;

/**
 * @brief Create a list
 *
 * @returns a pointer to the created list.
 * @retval NULL on failure due to allocation failure
 */
List* List_Create(void);


/**
 * @brief Destroy list
 * @details Destroys the list completely
 *          optionally destroys elements using user provided function
 *
 * @params[in] _pList : A pointer to previously created List returned via ListCreate
 *					   on completion *_pList will be null
 * @params[in] _elementDestroy : A function pointer to be used to destroy elements inserted into the list
 *             or a null if no such destroy is required
 */
void List_Destroy(List** _pList, void (*_elementDestroy)(void* _item));


/**
 * @brief Add element to head of list
 * @details time complexity: O(1).
 *
 * @params _list A previously created List ADT returned via ListCreate
 * @params _item An item to add to the list
 * @returns error code
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if the list is not initialized
 * @retval LIST_ALLOCATION_ERROR on memory allocation failure
 */
List_Result List_PushHead(List* _list, const void* _item);


/**
 * @brief Add element to the list's tail
 * @details time complexity O(1).
 *
 * @params _list
 * @params _item to add to the list
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if the list is not initialized
 * @retval LIST_ALLOCATION_ERROR on memory allocation failure
 */
List_Result List_PushTail(List* _list, const void* _item);


/** @brief Remove element from list's head
 *  @details if successful, stores a pointer to the removed item in _pItem
 *  time complexity O(1).
 *
 * @params _list : A previously created List ADT returned via ListCreate
 * @params _pItem: A pointer to receive the removed item. if NULL, the item will be removed and no returned.
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if the list is not initialized
 * @retval LIST_UNDERFLOW_ERROR if the list is empty
 */
List_Result List_PopHead(List* _list, void* *_pItem);


/** @brief Remove element from list's tail
 *  @details time complexity O(1).
 * *
 * @params _list : A previously created List ADT returned via ListCreate
 * @params _pItem: A pointer to receive the removed item. if NULL, the item will be removed and no returned.
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if the list is not initialized
 * @retval LIST_UNDERFLOW_ERROR if the list is empty
 */
List_Result List_PopTail(List* _list, void* *_pItem);


/** @brief Get number of elements in the list
 * Average time complexity O(n).
 *
 * @params _list : A previously created List ADT returned via ListCreate
 * @retval number of elements on success
 * @retval 0 if the list is empty or not initialized
 */
size_t List_Size(const List* _list);


/** @brief Check if list is empty
 *
 * @params _list : A previously created List ADT returned via ListCreate
 * @retval true if list is empty
 */
int List_IsEmpty(const List* _list);


#endif /* LIST_H */
