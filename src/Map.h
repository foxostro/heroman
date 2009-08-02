#ifndef _MAP_H_
#define _MAP_H_

#include "PropertyBag.h"
#include "Tile.h"

class PhysicsEngine;

/**
Manages a map made out of Tile objects.
Provides tools to edit the map graphically.
*/
class Map
{
private:
	/** Data associated with a tile character in the map */
	class TileData
	{
	public:
		float height;
		Material floorMat;
		Material wallMat;

		TileData() : height(0.0f) {}

		TileData(float _height, const Material &_floor, const Material &_wall)
		{
			height = _height;
			floorMat = _floor;
			wallMat = _wall;
		}
	};

	/** Number of tiles across the width of the map */
	int mapWidth;

	/** Number of tiles across the height of the map */
	int mapHeight;

	/** The width of a tile, specified in meters */
	float tileMetersX;

	/**  Two dimensional array of tiles in the map */
	vector<Tile> tiles;

	/** Map Character -> tile data */
	map<char, TileData> tileLegend;

	/** Indicates that the map has already been loaded */
	bool loaded;

	/** Space where map geometry is added to the physics engine */
	dSpaceID space;

	/** Display list for map geometry */
	GLuint displayList;

public:
	/**
	Indicates whether the physics trimesh geometry should be drawn instead of
	the heightmap model itself.
	*/
	bool displayDebugData;

public:
	~Map();

	Map();

	/** Resets a map that has already been cleanly destroyed */
	void clear();

	/** Cleanly destroys a map and then resets it */
	void destroy();

	/**
	Creates a map from data
	@param data terrain data source
	@param textureFactory texture factory tracks loaded textures
	@param physicsEngine Physics engine
	*/
	void create(const PropertyBag &data,
		        TextureFactory &textureFactory,
		        shared_ptr<PhysicsEngine> physicsEngine);

	/** Draws the map */
	void draw() const;

	/** Transforms the world-space coords into tile-map space */
	inline ivec2 toTileSpace(const vec2 &p) const
	{
		return ivec2((int)floorf(p.x / tileMetersX),
		             (int)floorf(p.y / tileMetersX));
	}

	/**
	Dtermines whether or not a position is within the tile map
	@param p Tile coordinates
	@return true if the positionis within the tile map
	*/
	inline bool onATile(const ivec2 &p) const
	{
		return(p.x >= 0 && p.x < mapWidth && p.y >= 0 && p.y < mapHeight);
	}

	/**
	Gets a Tile from the map given its coordinates in tile-space
	@param p Tile coordinates
	@return a reference to the Tile
	*/
	inline Tile& getTile(const ivec2 &p)
	{
		ASSERT(onATile(p), "position is not on a tile");
		return tiles[(p.y*mapWidth) + p.x];
	}

	/**
	Gets a Tile from the map given its coordinates in tile-space
	@param p Tile coordinates
	@return a reference to the Tile
	*/
	inline const Tile& getTile(const ivec2 &p) const
	{
		ASSERT(onATile(p), "position is not on a tile");
		return tiles[(p.y*mapWidth) + p.x];
	}

	/**
	Get the meters on each side of a Tile
	@return meters per Tile
	*/
	inline float getTileMetersX() const
	{
		return tileMetersX;
	}

	inline int getWidth() const
	{
		return mapWidth;
	}

	inline int getHeight() const
	{
		return mapHeight;
	}
};

#endif
