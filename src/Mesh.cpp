#include "stdafx.h"
#include "Effect.h"
#include "Mesh.h"

Mesh::~Mesh()
{}

Mesh::Mesh()
: vertexArray(new ResourceBufferVertices()),
  normalArray(new ResourceBufferVertices()),
  texCoordArray(new ResourceBufferTexCoords()),
  indexArray(new ResourceBufferIndices()),
  polygonWinding(GL_CW)
{}

Mesh::Mesh(const Mesh *obj)
: vertexArray(new ResourceBufferVertices()),
  normalArray(new ResourceBufferVertices()),
  texCoordArray(new ResourceBufferTexCoords()),
  indexArray(new ResourceBufferIndices()),
  polygonWinding(GL_CW)
{
	copy(*obj);
}

Mesh::Mesh(const Mesh &obj)
{
	copy(obj);
}

Mesh::Mesh(const vector<vec3> &verticesArray,
           const vector<vec3> &normalsArray,
           const vector<vec2> &texcoordsArray,
           const vector<Face> &facesArray)
: vertexArray(new ResourceBufferVertices()),
  normalArray(new ResourceBufferVertices()),
  texCoordArray(new ResourceBufferTexCoords()),
  indexArray(new ResourceBufferIndices()),
  polygonWinding(GL_CW)
{
	ASSERT(verticesArray.size()>0,  "Too few vertices");
	ASSERT(normalsArray.size()>0,   "Too few normals");
	ASSERT(texcoordsArray.size()>0, "Too few tex-coords");
	ASSERT(facesArray.size()>0,     "Too few faces");

    const int numOfIndices = (int)facesArray.size()*3;
	unsigned int el = 0;

    unsigned int *indices = new unsigned int[numOfIndices];
    vec3 *vertices = new vec3[numOfIndices];
    vec2 *texCoords = new vec2[numOfIndices];
    vec3 *normals = new vec3[numOfIndices];

    for(vector<Face>::const_iterator i=facesArray.begin();
        i!=facesArray.end();
        ++i)
    {
        const Face &face = *i;

        for(int j=0; j<3; ++j)
        {
			indices[el] = el;

			ASSERT(face.vertIndex[j] >= 0   && (size_t)face.vertIndex[j] < verticesArray.size(),   "Invalid vertex index: " + itos(face.vertIndex[j]));
			ASSERT(face.coordIndex[j] >= 0  && (size_t)face.coordIndex[j] < texcoordsArray.size(), "Invalid tex-coord index: " + itos(face.coordIndex[j]));
			ASSERT(face.normalIndex[j] >= 0 && (size_t)face.normalIndex[j] < normalsArray.size(),  "Invalid normal index: " + itos(face.normalIndex[j]));

            vertices[el] = verticesArray[face.vertIndex[j]];
            texCoords[el] = texcoordsArray[face.coordIndex[j]];
            normals[el] = normalsArray[face.normalIndex[j]];

            el++;
        }
    }

    vertexArray->recreate(numOfIndices, vertices);
    normalArray->recreate(numOfIndices, normals);
    texCoordArray->recreate(numOfIndices, texCoords);
    indexArray->recreate(numOfIndices, indices);

    delete [] indices;
    delete [] vertices;
    delete [] texCoords;
    delete [] normals;
}

Mesh& Mesh::operator=(const Mesh &obj)
{
	copy(obj);
	return(*this);
}

void Mesh::copy(const Mesh &mesh)
{
	material = mesh.material;
    vertexArray = mesh.vertexArray->clone();
    normalArray = mesh.normalArray->clone();
    texCoordArray = mesh.texCoordArray->clone();
	indexArray = mesh.indexArray->clone();
	polygonWinding = mesh.polygonWinding;
}

void Mesh::draw(bool useMaterial) const
{
    ASSERT(indexArray,    "Member \'indexArray\' is null");
    ASSERT(vertexArray,   "Member \'vertexArray\' is null");
    ASSERT(normalArray,   "Member \'normalArray\' is null");
    ASSERT(texCoordArray, "Member \'texCoordArray\' is null");

    CHECK_GL_ERROR();

    if(useMaterial)
	{
		material.bind();
	}

	glFrontFace(polygonWinding);

    //glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoordArray->getPointer());
    glNormalPointer(GL_FLOAT, 0, normalArray->getPointer());
    glVertexPointer(3, GL_FLOAT, 0, vertexArray->getPointer());

#if 0
	/* Draw using ranged vertex arrays */
	// glDrawRangeElements is only in OpenGL 1.2 or greater
    glDrawRangeElements(GL_TRIANGLES,
                        0,
                        vertexArray->getNumber(),
                        indexArray->getNumber(),
                        GL_UNSIGNED_INT,
                        indexArray->getPointer());
#elif 1
	/* Draw using vertex arrays */
	GLsizei count = indexArray->getNumber();
	const GLvoid *indices = (const GLvoid *)indexArray->getPointer();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
#else
	/* Draw with immediate mode */
	GLsizei count = indexArray->getNumber();
	unsigned int *indices = (unsigned int *)indexArray->getPointer();
	vec3 *normals = (vec3 *)normalArray->getPointer();
	vec3 *vertices = (vec3 *)vertexArray->getPointer();
	vec2 *texcoords = (vec2 *)texCoordArray->getPointer();

	glBegin(GL_TRIANGLES);
	for(GLsizei i=0; i<count; ++i)
	{
		unsigned int a = indices[i];
		glTexCoord2fv(texcoords[a]);
		glNormal3fv(normals[a]);
		glVertex3fv(vertices[a]);
	}
	glEnd();
#endif

    FLUSH_GL_ERROR();
}

float Mesh::calculateRadius(RadiusCalculator radius) const
{
    const size_t numOfVerts = vertexArray->getNumber();
    const vec3 *vertices = (const vec3*)vertexArray->getPointer();

    vec3 a(0,0,0);
    for(size_t i=0; i<numOfVerts; ++i)
    {
        vec3 b = vertices[i];
        a = (radius(a) > radius(b)) ? a : b;
    }

    return radius(a);
}

float Mesh::calculateHeight() const
{
	const size_t numOfVerts = vertexArray->getNumber();
	const vec3 *vertices = (const vec3*)vertexArray->getPointer();

	float maxZ=0, minZ=0;

	for(size_t i=0; i<numOfVerts; ++i)
	{
		const float &z = vertices[i].z;
		maxZ = max(maxZ, z);
		minZ = min(minZ, z);
	}

	return maxZ - minZ;
}

float Mesh::calculateHeightBelowGround() const
{
	const size_t numOfVerts = vertexArray->getNumber();
	const vec3 *vertices = (const vec3*)vertexArray->getPointer();

	float minZ=0;

	for(size_t i=0; i<numOfVerts; ++i)
	{
		minZ = min(minZ, vertices[i].z);
	}

	return min(minZ, 0.0f); // Clamp pos values to 0, pass through neg values
}

void Mesh::interpolate(float bias, const Mesh &a, const Mesh &b)
{
    const size_t numOfElements = a.indexArray->getNumber();

	const unsigned int *elA = (const unsigned int *)a.indexArray->getPointer();
	const unsigned int *elB = (const unsigned int *)b.indexArray->getPointer();
	const vec3 *vertsA = (const vec3 *)a.vertexArray->getPointer();
	const vec3 *vertsB = (const vec3 *)b.vertexArray->getPointer();
	const vec3 *normsA = (const vec3 *)a.normalArray->getPointer();
	const vec3 *normsB = (const vec3 *)b.normalArray->getPointer();

	unsigned int *elements = (unsigned int *)indexArray->lock();
    vec3 *vertices = (vec3 *)vertexArray->lock();
    vec3 *normals = (vec3 *)normalArray->lock();

	for(size_t i=0; i<numOfElements; ++i)
	{
		const unsigned int &a = elA[i];
		const unsigned int &b = elB[i];

		// Make sure conectivity is the same as one of the keyframes
		const unsigned int &x = elements[i] = a;

		const vec3 &vertexA = vertsA[a];
		const vec3 &normalA = normsA[a];

		const vec3 &vertexB = vertsB[b];
		const vec3 &normalB = normsB[b];

		vertices[x] = vertexA + (vertexB - vertexA)*bias;
		normals[x] = normalA + (normalB - normalA)*bias;
	}

    indexArray->unlock();
    vertexArray->unlock();
    normalArray->unlock();
}

tuple<dGeomID,dTriMeshDataID> Mesh::createGeom(dSpaceID physicsSpace)const
{
    dVector3 *triVert = new dVector3[vertexArray->getNumber()];

    {
        const vec3 *vertices = (const vec3 *)vertexArray->lock();
        for(int i=0; i<vertexArray->getNumber(); ++i)
        {
            triVert[i][0] = vertices[i].x;
            triVert[i][1] = vertices[i].y;
            triVert[i][2] = vertices[i].z;
        }
        vertexArray->unlock();
    }

    int *indexes = new int[indexArray->getNumber()];

    {
        const unsigned int *indices = (const unsigned int*)indexArray->lock();
        for(int i=0; i<indexArray->getNumber(); ++i)
        {
            indexes[i] = (int)indices[i];
        }
        indexArray->unlock();
    }

    return createGeom(physicsSpace,
                      (const dVector3*)triVert,
                      vertexArray->getNumber(),
                      (const int*)indexes,
                      indexArray->getNumber());
}

tuple<dGeomID,dTriMeshDataID> Mesh::createGeom(dSpaceID physicsSpace,
                                               const dVector3 *vertices,
                                               int numVertices,
                                               const int *indices,
                                               int numIndices)
{
	dTriMeshDataID meshID = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSimple(meshID,
	                            (const dReal*)vertices,
	                            numVertices,
	                            indices,
	                            numIndices);
	dGeomID geom = dCreateTriMesh(physicsSpace, meshID, 0, 0, 0);
	dGeomSetPosition(geom, 0.0, 0.0, 0.0);
	return make_tuple(geom, meshID);
}
