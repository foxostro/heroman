#include "stdafx.h"
#include "Tile.h"
#include "Material.h"
#include "searchfile.h"
#include "FileFuncs.h"
#include "PhysicsEngine.h"
#include "Map.h"

Map::Map()
{
	clear();
}

Map::~Map()
{
	destroy();
}

void Map::clear()
{
	loaded = false;
	displayDebugData = false;
	tileLegend.clear();
	tiles.clear();
	space = 0;
	mapWidth = 0;
	mapHeight = 0;
	displayList = 0;
}

void Map::destroy()
{
	if(space)
	{
		dSpaceDestroy(space);
	}

	if(displayList)
	{
		glDeleteLists(displayList, 1);
	}

	clear();
}

void Map::create(const PropertyBag &data,
				 TextureFactory &textureFactory,
				 shared_ptr<PhysicsEngine> physicsEngine)
{
	destroy();

	// Set up the space to which tile geoms will add themselves
	space = dHashSpaceCreate(physicsEngine->getSpace());

	tileMetersX = data.getFloat("tileMetersX"); // xz scaling

	// Load the tile legend
	tileLegend.clear();
	const PropertyBag legendBag = data.getBag("legend");

	for(size_t i=0, numberOfEntries=legendBag.getNumInstances("entry");
		i < numberOfEntries; ++i)
	{
		const PropertyBag entry = legendBag.getBag("entry", i);

		const string key = entry.getString("key");
		const float height = entry.getFloat("height");
		const FileName floorImage = entry.getFileName("floor");
		const FileName wallImage = entry.getFileName("wall");

		TileData tileData(height, textureFactory.load(floorImage, false),
			                      textureFactory.load(wallImage, false));

		tileLegend.insert(make_pair(key[0], tileData));
	}

	// Load tile map
	const string tileMap = data.getString("data");

	vector<string> lines;

	tokenizer< char_separator<char> >
		tokens(tileMap, char_separator<char>(" \t\n"));

	for(tokenizer< char_separator<char> >::const_iterator i = tokens.begin();
		i != tokens.end();
		++i)
	{
		lines.push_back(*i);
	}

	// Allocate memory for the map tiles
	mapWidth = (int)lines.size();
	mapHeight = (int)lines[0].size();
	tiles.resize(mapHeight * mapWidth);

	// Set up tile data
	for(int x=0; x<mapWidth; ++x)
	{
		ASSERT(x < (int)lines.size(), "Invalid x");
		const string &line = lines[x];

		for(int y=0; y<mapHeight; ++y)
		{
			ASSERT(y < (int)line.size(), "Invalid y");

			const char c = line[y];

			const TileData &tileData = tileLegend[c];

			//Tile &tile = getTile(ivec2(mapWidth-x-1,y));
			Tile &tile = getTile(ivec2(x,y));

			tile.create(tileMetersX,
			            tileData.height,
						tileData.floorMat,
						tileData.wallMat);
		}
	}

	// Create a physics objects for each tile
	for(int y=0; y<mapHeight; ++y)
	{
		for(int _x=0; _x<mapWidth; ++_x)
		{
			int x = mapWidth-_x-1;

			Tile &tile = getTile(ivec2(x,y));

			const float height = tile.getTileHeight();

			if(height>0.01f)
			{
				dGeomID geom = dCreateBox(space,
										  tileMetersX,
										  tileMetersX,
										  height);

				dGeomSetPosition(geom,
								 x*tileMetersX + (tileMetersX/2),
								 y*tileMetersX + (tileMetersX/2),
								 height/2);
			}
		}
	}

	// Create the display list
	{
		displayList = glGenLists(1);
		glNewList(displayList, GL_COMPILE);

		for(int y=0; y<mapHeight; ++y)
		{
			for(int x=0; x<mapWidth; ++x)
			{
				getTile(ivec2(x,y)).draw(vec2(x,y) * tileMetersX);
			}
		}

		glEndList();
	}

	loaded = true;
}

void Map::draw() const
{
	if(!loaded) return;
	glCallList(displayList);
}
