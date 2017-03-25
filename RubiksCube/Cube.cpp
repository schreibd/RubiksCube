#include "Cube.h"
#include <stdlib.h>
#include <iostream>
#include "RenderAssistant.h"

using namespace sce::Vectormath::Simd::Aos;


Cube::Cube(void) : m_basicVertices(NULL), m_basicIndices(NULL), m_basicVerticesUId(-1), m_basicIndiceUId(-1), modelTransformation(Matrix4::identity()), animationTransformation(Matrix4::identity())
{
 InitializeBuffer();

}


Cube::~Cube(void)
{
}

//Allokiert Speicher für die Vertices und die Indizes eines Würfels 
//Aktualisiert Indizes 
void Cube::InitializeBuffer(){
	/* create shaded triangle vertex/index data  */
	m_basicVertices = (BasicVertex *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 4 * 6 * sizeof(BasicVertex), 4, SCE_GXM_MEMORY_ATTRIB_READ, &m_basicVerticesUId );
	m_basicIndices = (uint16_t *)graphicsAlloc( SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, 6 * 6 * sizeof(uint16_t), 2, SCE_GXM_MEMORY_ATTRIB_READ, &m_basicIndiceUId );

	int count = 0;
    for(int side = 0; side < 6; ++side)
    {
        int baseIndex = side * 4;
        m_basicIndices[count++] = baseIndex;
        m_basicIndices[count++] = baseIndex + 1;
        m_basicIndices[count++] = baseIndex + 2;
		
        m_basicIndices[count++] = baseIndex;
        m_basicIndices[count++] = baseIndex + 3;
        m_basicIndices[count++] = baseIndex + 2;
       
    }

	count = 0;
			
	for(int type = 0; type < 3; ++type)
	{
		 for(int dir = -1; dir < 2; dir += 2)
		{
			BasicVertex* writePtr = &m_basicVertices[count];
			this->CreateCubeSide(writePtr, type, dir);
			count += 4;
		}
	}
} 

// Helper function to create a cube.
// type 0: x, 1 :y, 2: z.
// direction: 1 : positive, -1 : negative
void Cube::CreateCubeSide(BasicVertex* field, int type, int direction)
{
    for(int i = 0; i < 4; ++i) {
        field[i].position[type] = direction * 0.5f;
		field[i].normal[i%3] = 0.0f;
	}

    int localXDim = (type + 1) % 3;
    int localYDim = (type + 2) % 3;

	

   field[0].position[localXDim] = -0.5f;
   field[1].position[localXDim] = -0.5f;
   field[2].position[localXDim] = 0.5f; 
   field[3].position[localXDim] = 0.5f; 
   field[0].position[localYDim] = 0.5f; 
   field[1].position[localYDim] = -0.5f;
   field[2].position[localYDim] = - 0.5f;
   field[3].position[localYDim] = 0.5f;
   //Speichere Normalen ab
   field[0].normal[type] = 1.0f * direction;
   field[1].normal[type] = 1.0f * direction;
   field[2].normal[type] = 1.0f * direction;
   field[3].normal[type] = 1.0f * direction;
   
}

void Cube::DrawCube(SceGxmContext* context, const Matrix4& view, const Matrix4& projection, const Matrix4& globalRotation, const SceGxmProgramParameter* wvpParam, const SceGxmProgramParameter* wParam)
{
	Matrix4 world = globalRotation * animationTransformation * modelTransformation;
	Matrix4 completeTransform = projection * view * world;

	/* set the vertex program constants */
	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer( context, &vertexDefaultBuffer );
	sceGxmSetUniformDataF( vertexDefaultBuffer, wvpParam, 0, 16, (float*)&completeTransform );
	sceGxmSetUniformDataF( vertexDefaultBuffer, wParam, 0, 16, (float*)&world);

	/* draw the spinning triangle */
	sceGxmSetVertexStream( context, 0, m_basicVertices );
	//Methodenaufruf zum Zeichnen der Würfel mit Schleife
	sceGxmDraw( context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_basicIndices, 6 * 6 );
}

void Cube::setColor(uint32_t color, Logic::SIDE_INDEX side){
	int vertexStart = 0;
	int vertexEnd = 0;

	switch (side)
	{
		case Logic::SIDE_LEFT:
			vertexStart = 0;
			vertexEnd = 3;
		break;
		case Logic::SIDE_RIGHT:
			vertexStart = 4;
			vertexEnd = 7;
		break;
		case Logic::SIDE_TOP:
			vertexStart = 8;
			vertexEnd = 11;
		break;
		case Logic::SIDE_BOTTOM:
			vertexStart = 12;
			vertexEnd = 15;
		break;
		case Logic::SIDE_FRONT:
			vertexStart = 16;
			vertexEnd = 19;
		break;
		case Logic::SIDE_BACK:
			vertexStart = 20;
			vertexEnd = 23;	
	}

	for (int i=vertexStart; i <= vertexEnd; ++i)
	{
		m_basicVertices[i].color = color;
	}
}