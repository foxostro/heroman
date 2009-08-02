#ifndef _TILE_H_
#define _TILE_H_

#include "Material.h"
#include "BoundingBox.h"

/** Represents the data necessary to construct a Tile in the game map. */
class Tile
{
private:
	/** Describes the material that is to be used to texture the Tile */
	Material wallMat;

	/** Describes the material to use for floor/top on the Tile */
	Material floorMat;

	/** Bounding box for the node */
	BoundingBox boundingBox;

public:
	/** Default Constructor */
	Tile();

	/** Copy Constructor */
	Tile(const Tile &tile)
	: wallMat(tile.wallMat),
	  floorMat(tile.floorMat),
	  boundingBox(tile.boundingBox)
	{}

	/**
	Draws the Tile.
	@param position The world space position of the tile
	*/
	void draw(const vec2 &position) const;

	/**
	Gets the tile height, specified in meters
	@return tile height, specified in meters
	*/
	float getTileHeight() const
	{
		return boundingBox.m_Max.z;
	}

	/**
	Gets a pointer to the tile height for the explicit purpose of allowing it to modified arbitrarily
	@return Pointer to the tile height
	*/
	float* getModifiableTileHeight()
	{
		return &boundingBox.m_Max.y;
	}

	/** Sets up the tile */
	void create(float width,
	            float height,
			    const Material &floorMat,
				const Material &wallMat);

private:
	/**
	Initializes the bounding box for the node
	@param width Node width
	@param height Node height
	*/
	void setupBoundingBox(float width, float height);

	/** Draws the walls of a block-type tile */
	void drawWalls(const Material &wall) const;

	/** Draws the floor of a block-type tile */
	void drawFloor(const Material &floor) const;

	static void texCoord(float u, float v);
};

#endif
