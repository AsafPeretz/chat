#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>  /* size_t */

typedef struct Queue Queue;

typedef enum Queue_Result {
	Queue_Success,
	Queue_Uninitialized_Error,
	Queue_InvalidArgument_Error,
	Queue_Underflow_Error,
	Queue_Overflow_Error,
	Queue_Unknown_Error
} Queue_Result;

/**
 * @brief Dynamically create a new queue object of given capacity
 * @param[in] _maxSize - maximal number of elements that can be stored in the queue
  * @return Queue * - on success / NULL on fail
 *
 * @warning if _maxSize is zero function will return NULL.
 */
Queue* Queue_Create(size_t _maxSize);

/**
 * @brief Dynamically deallocate a previously allocated queue
 * @details Average time complexity O(n) if _elementDestroy existed and O(1) if not.
 * @param[in] _queue - Queue to be deallocated. Set to NULL following cleanup
 * @params[in] _elementDestroy : A function pointer to be used to destroy all
 * 					 remaining elements in the queue or a null if no such destroy is required
 * @return void
 */
void Queue_Destroy(Queue* *_queue, void (*_elementDestroy)(void* _item));

/**
 * @brief Enqueue an Item to the tail of the Queue.
 * @param[in] _queue - Queue to operate on.
 * @param[in] _item - Item to enqueue.
 * @return success or error code
 * @retval QUEUE_SUCCESS on success
 * @retval QUEUE_UNINITIALIZED_ERROR if input queue is not initialized
 * @retval QUEUE_OVERFLOW_ERROR if queue is full.
 */
Queue_Result Queue_Enqueue(Queue* _queue, void* _item);

/**
 * @brief Dequeue (Remove and obtain) item at head of queue or return an error if queue is empty.
 * @param[in] _queue - Queue to operate on.
 * @param[out] _pValue - pointer to variable that will receive dequeued item
 * @return success or error code
 * @retval QUEUE_SUCCESS on success
 * @retval QUEUE_UNINITIALIZED_ERROR if input queue is not initialized
 * @return QUEUE_UNDERFLOW_ERROR if queue is empty
 */
Queue_Result Queue_Dequeue(Queue* _queue, void* *_pValue);

/**
 * @brief Return true if the queue is empty.
 * @param[in] _queue - Queue to check.
 * @return  1 if empty 0 otherwise
 */
int Queue_IsEmpty(const Queue* _queue);
int Queue_IsFull (const Queue* _queue);

#ifdef DEBUG
/**
 * The following are convenience function(s) for debugging purposes
 */
typedef int	(*QueueElementAction)(void* _element, void* _context);
/**
 * @brief Iterate over all elements in the queue.
 * @details The user provided _action function will be called for each element
 *          if _action returns a zero for an element the iteration will stop.
 * @param[in] _queue  - queue to iterate over.
 * @param[in] _action - User provided function pointer to be invoked for each element
 * @param[in] _context - User provided context, will be sent to _action
 * @returns number of times the user function was invoked
 *
 * Example use: Queue_ForEach(queue, PrintElement, NULL)
 */
size_t Queue_ForEach(const Queue* _queue, QueueElementAction _action, void* _context);
#endif

#endif /* QUEUE_H */
