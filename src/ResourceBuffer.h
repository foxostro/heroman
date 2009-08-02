#ifndef _BUFFER_RESOURCE_H_
#define _BUFFER_RESOURCE_H_

#include "vec2.h"
#include "vec3.h"
#include "color.h"

/**
Contains a buffer of graphically related data such as an index array or a
vertex array. This data may be stored in memory on the graphics device after
being submitted. (but the current implementation doesn't)
*/
template<typename ELEMENT>
class ResourceBuffer
{
public:
	/** Destructor */
	virtual ~ResourceBuffer();

	/** Default Constructor */
	ResourceBuffer();

	/**
	Constructor
	@param numElements Number of elements in the buffer
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	*/
	ResourceBuffer(int numElements, const ELEMENT *buffer);

    /**
    Copy constructor. Copies the contents of a given buffer.
    @param copyMe Buffer to clone
    */
    ResourceBuffer(const ResourceBuffer &copyMe);

    /**
    Creates a copy of this buffer
    @return New object with copy of this buffer
    */
    shared_ptr< ResourceBuffer<ELEMENT> > clone() const;

	/**
	Reallocates memory for the buffer
	@param numElements Number of elements in the buffer.  Must be greater
	than zero unless both numElements=0 and buffer=0
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	*/
	void recreate(int numElements, const ELEMENT *buffer);

	/**
	Gets the number of elements in the buffer
	@return Number of elements in the buffer
	*/
	int getNumber() const;

	/**
	Gets a pointer to the first coordinate of the
	first element in the array. (Read-only access)
	@return elements array
	*/
	virtual const void* getPointer() const;

	/**
	Locks the buffer to allow read-write access by the client.
	@return elements array
	*/
	virtual void* lock();

	/** Unlocks the buffer if it was previously locked. */
	virtual void unlock() const;

private:
	/** Indicates that the buffer is currently locked */
	mutable bool locked;

	/** Number of elements in the buffer */
	int numElements;

	/** Buffer, stored on the client-side */
	ELEMENT *buffer;
};

/* Buffers for various purposes */
typedef ResourceBuffer<vec3> ResourceBufferVertices;
typedef ResourceBuffer<color> ResourceBufferColors;
typedef ResourceBuffer<vec2> ResourceBufferTexCoords;
typedef ResourceBuffer<unsigned int> ResourceBufferIndices;

/* Various shared buffers */
typedef shared_ptr<ResourceBufferVertices> ResourceBufferVerticesPtr;
typedef shared_ptr<ResourceBufferColors> ResourceBufferColorsPtr;
typedef shared_ptr<ResourceBufferTexCoords> ResourceBufferTexCoordsPtr;
typedef shared_ptr<ResourceBufferIndices> ResourceBufferIndicesPtr;

#endif
