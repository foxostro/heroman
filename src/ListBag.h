#ifndef LIST_BAG_H
#define LIST_BAG_H

#include "PropertyBag.h"

/** List of elements contained within a PropertyBag data source */
template<typename ElementType>
class ListBag : public vector<ElementType>
{
public:
	ListBag() { /* Do Nothing */ }

	const_reference getRandom() const
	{
		if(size()==1)
		{
			return at(0);
		}
		else
		{
			size_t idx = IRAND_RANGE(0, size());
			return at(idx);
		}
	}
};

#endif
