#include "stdafx.h"
#include "Material.h"
#include "FileFuncs.h"
#include "Mesh.h"
#include "PhysicsEngine.h"
#include "Terrain.h"

#include <SDL/SDL_image.h>

dReal heightfieldCallback(void *_heightmapData, int y, int x)
{
	ASSERT(_heightmapData, "Parameter \"_heightmapData\" was null");

	const HeightMapData &heightmapData = *reinterpret_cast<HeightMapData*>(_heightmapData);

	const int w = heightmapData.w;
	const float *heightMap = heightmapData.map;

	ASSERT(heightMap, "\"heightMap\" was null");

	return heightMap[(y*w)+x];
}

Terrain::~Terrain()
{
	delete [] heightmap.map;
}

Terrain::Terrain()
: geom(0),
  displayDebugData(false),
  scaleXY(1.0f),
  scaleZ(1.0f)
{}

void Terrain::clear()
{
    if(geom)
	{
		dGeomDestroy(geom);
		geom=0;
	}

    geometry.reset();

	loaded = false;
}

HeightMapData Terrain::loadHeightMap(const FileName &fileName)
{
	// Load the font image with DevIL
	unsigned int imageName = devil_loadImage(fileName);
	unsigned char *pixels = ilGetData();
	int width		      = ilGetInteger(IL_IMAGE_WIDTH);
	int height		      = ilGetInteger(IL_IMAGE_HEIGHT);
	int bytesPerPixel	  = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	ASSERT(width == height, "Heightmap is not square!");

	const size_t rowSize = width * bytesPerPixel;
	const size_t imageSize = height * rowSize;

	float *heightmap = new float[width * height];

	for(size_t y = 0; y < (size_t)height; ++y)
	{
	    unsigned char *row = (unsigned char*)(pixels) + (y*rowSize);

	    for(size_t x = 0; x < rowSize; x += (bytesPerPixel / 8))
	    {
	        // only care about the first component
	        unsigned char pixel = row[x];
			heightmap[y*width + x] = pixel / 255.0f;
	    }
	}

	// return
	HeightMapData data;
	data.map = heightmap;
	data.w = width;
	data.h = height;
	return data;
}

void Terrain::generateVertices(vector<vec3> &verticesArray,
						   vector<vec3> &normalsArray,
						   vector<vec2> &texcoordsArray,
						   const int heightmapSize,
						   const float *heightmap,
						   const float scaleXY,
						   const float scaleZ)
{
	const int numVertices = heightmapSize * heightmapSize;

	verticesArray.resize(numVertices);
	normalsArray.resize(numVertices);
	texcoordsArray.resize(numVertices);

	for(int y=0; y<heightmapSize; ++y)
	{
		for(int x=0; x<heightmapSize; ++x)
		{
			const size_t idx = y*heightmapSize + x;

			verticesArray[idx] = vec3((float)x*scaleXY,
			                          (float)y*scaleXY,
									  heightmap[idx]*scaleZ);

			normalsArray[idx] = vec3(0,0,1); // fake

			texcoordsArray[idx] = vec2((float)x, (float)y);
		}
	}
}

vector<Face> Terrain::generateIndices(const int heightmapSize)
{
	const int numFaces = (heightmapSize-1) * (heightmapSize-1) * 2;
	vector<Face> facesArray;
	facesArray.resize(numFaces);
	int idx=0;
	for(int y=0; y<heightmapSize-1; ++y)
	{
		for(int x=0; x<heightmapSize-1; ++x)
		{
			// Face 'a' of the heightmap tile
			{
				Face a;
				a.vertIndex[0] = a.coordIndex[0] = a.normalIndex[0] = (y+0)*heightmapSize + (x+0);
				a.vertIndex[1] = a.coordIndex[1] = a.normalIndex[1] = (y+0)*heightmapSize + (x+1);
				a.vertIndex[2] = a.coordIndex[2] = a.normalIndex[2] = (y+1)*heightmapSize + (x+0);
				facesArray[idx++] = a;
			}

			// Face 'b' of the heightmap tile
			{
				Face b;
				b.vertIndex[0] = b.coordIndex[0] = b.normalIndex[0] = (y+0)*heightmapSize + (x+1);
				b.vertIndex[1] = b.coordIndex[1] = b.normalIndex[1] = (y+1)*heightmapSize + (x+1);
				b.vertIndex[2] = b.coordIndex[2] = b.normalIndex[2] = (y+1)*heightmapSize + (x+0);
				facesArray[idx++] = b;
			}
		}
	}

	return facesArray;
}

shared_ptr<Mesh> Terrain::generateGeometry(const int heightmapSize,
	                                   const float *heightmap,
						               TextureFactory &textureFactory,
	                                   const FileName heightmapTex,
	                                   const float scaleXY,
						               const float scaleZ)
{
	vector<vec3> verticesArray;
	vector<vec3> normalsArray;
	vector<vec2> texcoordsArray;

	// Generate geometry
	generateVertices(verticesArray,
	                 normalsArray,
					 texcoordsArray,
					 heightmapSize,
					 heightmap,
					 scaleXY,
					 scaleZ);

	vector<Face> facesArray = generateIndices(heightmapSize);

	// Put map geometry into a mesh object
	shared_ptr<Mesh> geometry(new Mesh(verticesArray,
	                                   normalsArray,
									   texcoordsArray,
									   facesArray));

	// Set heightmap material
	geometry->material = Material(textureFactory.load(heightmapTex));

	return geometry;
}

void Terrain::create(const PropertyBag &data,
	                 TextureFactory &textureFactory,
	                 shared_ptr<PhysicsEngine> physicsEngine)
{
	ASSERT(physicsEngine, "Physics engine was null");
	
	scaleXY = data.getFloat("scaleXY");
	scaleZ = data.getFloat("scaleZ");

	const FileName heightmapFile = data.getFileName("heightmap");
	const FileName heightmapTex = data.getFileName("material");

	// Loads heightmap
	this->heightmap = loadHeightMap(heightmapFile);

	// Generate heightmap geometry
	geometry = generateGeometry(heightmap.w,
	                            heightmap.map,
					            textureFactory,
								heightmapTex,
					            scaleXY, scaleZ);

	// Send heightmap to physics engine
	tuple<dGeomID,dTriMeshDataID> n = geometry->createGeom(physicsEngine->getSpace());
	geom = n.get<0>();
	dGeomSetPosition(geom, 0.0f, 0.0f, 0.0f);

	loaded = true;
}

void Terrain::draw() const
{
	if(!loaded) return;

	ASSERT(geometry, "Member \"geometry\" was null!");

	CHECK_GL_ERROR();

	// Draw heightmap geometry
	geometry->draw(true);

	if(displayDebugData)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		PhysicsEngine::drawGeom(geom);
		glPopAttrib();
	}

	CHECK_GL_ERROR();
}
