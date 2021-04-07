#include "circular_buffer.h"             // Include buffer header

// Initialize buffer
int cb_init(circular_buffer *cb, size_t capacity, size_t data_size) {
  const int incremented_capacity = capacity + 1; // Add extra element to evaluate count
  cb->buffer = malloc(incremented_capacity * data_size);
  if (cb->buffer == NULL)
    return CB_MEMORY_ERROR;
  cb->buffer_end = (char *)cb->buffer + incremented_capacity * data_size;
  cb->data_size = data_size;
  cb->head = cb->buffer;
  cb->tail = cb->buffer;
  return CB_SUCCESS;
}
/* Clean buffer */
int cb_clean(circular_buffer *cb) {
  free(cb->buffer);
  return CB_SUCCESS;
}

/* Insert data into the buffer */
int cb_push(circular_buffer *cb, const void *item) {
	// Get it lenght. It is used len + 1 to be able to control when it is full
	const int buffer_length = _cb_length(cb);
	const int capacity_length = buffer_length - cb->data_size;	

	// Checks if buffer is full
	// In order for the buffer to be full, the head pointer should be 1 element
    // behind the tail pointer. Thus, the space needed to cover in order to jump 
    // from the head location to the tail location should be equal to 1 * data_size.
	if ((char *)cb->tail - (char *)cb->head == cb->data_size ||
		(char *)cb->head - (char *)cb->tail == capacity_length)
		return CB_OVERFLOW_ERROR;
	// Copy item into the buffer
	memcpy(cb->head, item, cb->data_size);

	// Change head pointer
	cb->head = (char*)cb->head + cb->data_size;
	if(cb->head == cb->buffer_end)
		cb->head = cb->buffer;
	return CB_SUCCESS;
}
/* Checks if buffer is empty */
int cb_is_empty(circular_buffer *cb){
	return (cb->head == cb->tail) ? 1 : 0;
}

/* Get the oldest data from buffer (FIFO) */
int cb_pop(circular_buffer *cb, void *item) {
	// Checks if buffer is empty
	if (cb_is_empty(cb))
		return CB_EMPTY_ERROR;
	// Get data from the buffer (oldest one)
	memcpy(item, cb->tail, cb->data_size);
	// Change tail pointer value
	cb->tail = (char*)cb->tail + cb->data_size;
	if(cb->tail == cb->buffer_end)
		cb->tail = cb->buffer;
	return CB_SUCCESS;
}

// Realiza a leitura de multiplos valores do buffer circular
int cb_get_values (circular_buffer *cb, void *vector, uint16_t *request_size) {
	uint16_t _size = *request_size;
	// Checks if buffer is empty
	if (cb_is_empty(cb))
		return CB_EMPTY_ERROR;

	// Validate size
	uint16_t available_itens = ((cb->buffer_end - cb->tail) / cb->data_size) - 1;
	

	if (available_itens < _size) _size = available_itens;

	// Get data from the buffer (oldest one)
	memcpy(vector, cb->tail, cb->data_size * _size);

	// Change tail pointer value
	cb->tail = (char*)cb->tail + cb->data_size* _size;
	if(cb->tail == cb->buffer_end)
		cb->tail = cb->buffer;

	// Rewrite request size
	*request_size = _size;

	return CB_SUCCESS;
}

/* Get buffer size */
int _cb_length(circular_buffer *cb) {
  return (char *)cb->buffer_end - (char *)cb->buffer;
}