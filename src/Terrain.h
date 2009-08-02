#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "PropertyBag.h"
#include "Material.h"
#include "Mesh.h"

class PhysicsEngine;

struct HeightMapData
{
	float *map;
	int w, h;

	HeightMapData()
		: map(0), w(0), h(0)
	{}
};

class Terrain
{
private:
	shared_ptr<Mesh> geometry;
	dGeomID geom;
	HeightMapData heightmap;
	float scaleXY, scaleZ;
	bool loaded;

public:
	/**
	Indicates whether the physics trimesh geometry should be drawn instead of
	the heightmap model itself.
	*/
	bool displayDebugData;

public:
	~Terrain();

	Terrain();

	void clear();

	/**
	Creates the terrain from data
	@param data terrain data source
	@param textureFactory texture factory tracks loaded textures
	@param physicsEngine Physics engine
	*/
	void create(const PropertyBag &data,
	            TextureFactory &textureFactory,
	            shared_ptr<PhysicsEngine> physicsEngine);

	/** Draws the map */
	void draw() const;

private:
	/** 
	Loads a square heightmap from an image file
	@param fileName File name of the heightmap image
	@return tuple containing the heightmap buffer and the size of the square
	heightmap on a side. The length of the buffer is thus this size squared.
	*/
	HeightMapData loadHeightMap(const FileName &fileName);

	/** Generates geometry from heightmap data */
	static shared_ptr<Mesh> generateGeometry(const int heightmapSize,
	                                         const float *heightmap,
						                     TextureFactory &textureFactory,
	                                         const FileName heightmapTex,
	                                         const float scaleXY,
						                     const float scaleZ);

	/** Generate vertices from heightmap data */
	static void generateVertices(vector<vec3> &verticesArray,
	                             vector<vec3> &normalsArray,
						         vector<vec2> &texcoordsArray,
						         const int heightmapSize,
						         const float *heightmap,
						         const float scaleXY,
						         const float scaleZ);

	/** Generate indices for heightmap geometry */
	static vector<Face> generateIndices(const int heightmapSize);
};

#endif
