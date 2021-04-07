// Inclusion guard, to prevent multiple includes of the same header
#ifndef CB_H
#define CB_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* 
 * @brief Define Circular Buffer returns
*/ 
#define CB_SUCCESS          0   // Operation was successful;
#define CB_MEMORY_ERROR     1   // Failed to allocate memory;
#define CB_OVERFLOW_ERROR   2   // CB is full. Cannot push more items;
#define CB_EMPTY_ERROR      3   // CB is empty. Cannot pop more items.

/* 
 * @brief Define Circular Buffer structure
 */ 
typedef struct circular_buffer {	
  void *buffer;		// Holds buffer init memory address
  void *buffer_end;	// Holds buffer end memory address (size + 1)
  size_t data_size;		// Buffer data unit of storage. (e.g 16 bits)
  void *head;		// Buffer head position
  void *tail;		// Buffer tail position
} circular_buffer;

/* 
 * Functions
 */ 

/* 
 * @brief Used to initialize Circular Buffer
 * @param cb Circular Buffer data structure pointer.
 * @param capacity Total capacity of the CB.
 * @param data_size Size of data that will be stored. (eg. uint16_t)
 * 
 * @return 
 *      - CB_SUCCESS When buffer was initialized successfuly
 *      - CB_MEMORY_ERROR When error occurred while trying to init the cb.
 */ 
int cb_init(circular_buffer *cb, size_t capacity, size_t data_size);

/* 
 * @brief Used to clean and kill the circular buffer.
 * @param cb Circular Buffer data structure pointer.
 * 
 * @return CB_SUCCESS
 */ 
int cb_clean(circular_buffer *cb);

/* 
 * @brief Used to push data into the circular buffer
 * @param cb Circular Buffer data structure pointer.
 * @param item Item that will be store into the circular buffer.
 * 
 * @return
 *      - CB_SUCCESS When success;
 *      - CB_OVERFLOW_ERROR When buffer is full.
 */ 
int cb_push(circular_buffer *cb, const void *item);

/* 
 * @brief Used to get the oldest data from the buffer (FIFO)
 * @param cb Circular Buffer data structure pointer.
 * @param item Variable pointer where data will be returned
 * 
 * @return
 *      - CB_SUCCESS When success;
 *      - CB_EMPTY_ERROR When buffer is empty.
 */ 
int cb_pop(circular_buffer *cb, void *item);

/* 
 * @brief Indicates if buffer is empty
 * @param cb Circular Buffer data structure pointer.
 * 
 * @return 1 when buffer is empty
 */
int cb_is_empty(circular_buffer *cb);

/* 
 * @brief Indicates buffer total size.
 * @param cb Circular Buffer data structure pointer.
 * 
 * @return Buffer total size
 */ 
int _cb_length(circular_buffer *cb);

/*  
 * @brief Used to get multiple values from buffer.
 * @param cb Circular Buffer data structure pointer.
 * @param vector Variable pointer where data will be returned
 * @param request_size Variable pointer with total of items that will be returned.
 * If buffer size is less than requested size, this varible will be rewrited with
 * total values read from the buffer
 * 
 * @return
 *      - CB_SUCCESS When success;
 *      - CB_EMPTY_ERROR When buffer is empty.
 */ 
int cb_get_values (circular_buffer *cb, void *vector, uint16_t *request_size);


// End of the inclusion guard
#endif