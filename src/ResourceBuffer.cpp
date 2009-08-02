#include "stdafx.h"
#include "ResourceBuffer.h"

template<typename ELEMENT>
ResourceBuffer<ELEMENT>::~ResourceBuffer()
{
    delete [] buffer;
}

template<typename ELEMENT>
ResourceBuffer<ELEMENT>::ResourceBuffer()
: locked(false),
  numElements(0),
  buffer(0)
{}

template<typename ELEMENT>
ResourceBuffer<ELEMENT>::ResourceBuffer(int numElements,
                                        const ELEMENT *buffer)
: locked(false),
  numElements(0),
  buffer(0)
{
    recreate(numElements, buffer);
}

template<typename ELEMENT>
ResourceBuffer<ELEMENT>::ResourceBuffer(const ResourceBuffer &copyMe)
: locked(false),
  numElements(0),
  buffer(0)
{
    recreate(copyMe.numElements, copyMe.buffer);
}

template<typename ELEMENT>
shared_ptr< ResourceBuffer<ELEMENT> > ResourceBuffer<ELEMENT>::clone() const
{
	ResourceBuffer<ELEMENT> *ptr = new ResourceBuffer<ELEMENT>(*this);
    shared_ptr< ResourceBuffer<ELEMENT> > buffer(ptr);
    return buffer;
}

template<typename ELEMENT>
void ResourceBuffer<ELEMENT>::recreate(int numElements, const ELEMENT *buffer)
{
    ASSERT(!locked, "Cannot realloc a locked buffer!");
    ASSERT(numElements>=0, "Parameter \'numElements\' < 0");

    delete [] (this->buffer);
    this->buffer = 0;
    this->numElements = numElements;

    if(numElements>0)
    {
        this->buffer = new ELEMENT[numElements];
        this->numElements = numElements;

        if(buffer != 0)
        {
            memcpy(this->buffer, buffer, sizeof(ELEMENT)*numElements);
        }
        else
        {
            memset(this->buffer, 0, sizeof(ELEMENT)*numElements);
        }
    }
}

template<typename ELEMENT>
int ResourceBuffer<ELEMENT>::getNumber() const
{
    return numElements;
}

template<typename ELEMENT>
const void* ResourceBuffer<ELEMENT>::getPointer() const
{
    ASSERT(!locked, "Cannot get a locked buffer!");
    return buffer;
}

template<typename ELEMENT>
void* ResourceBuffer<ELEMENT>::lock()
{
    ASSERT(!locked, "Cannot lock a buffer that is already locked!");
    locked=true;
    return buffer;
}

template<typename ELEMENT>
void ResourceBuffer<ELEMENT>::unlock() const
{
    ASSERT(locked, "Cannot unlock a buffer that is not locked!");
    locked=false;
}

// template class instantiations
// (see http://www.codeproject.com/cpp/templatesourceorg.asp)
template class ResourceBuffer<vec3>;
template class ResourceBuffer<color>;
template class ResourceBuffer<vec2>;
template class ResourceBuffer<unsigned int>;
