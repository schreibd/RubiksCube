#pragma once
#include <stdio.h>
#include <vectormath.h>
#include <gxm.h>
#include "Logic.h"


using namespace sce::Vectormath::Simd::Aos;

typedef struct BasicVertex
{
	float position[3];  // Easier to index.
	float normal[3];
	uint32_t color; // Data gets expanded to float 4 in vertex shader.
} BasicVertex; 

class Cube
{
public:
	Cube(void);
	~Cube(void);
	void DrawCube(SceGxmContext* context, const Matrix4& view, const Matrix4& projection, const Matrix4& globalRotation, const SceGxmProgramParameter* wvpParam, const SceGxmProgramParameter* wParam);
	void InitializeBuffer();
	void CreateCubeSide(BasicVertex* field, int type, int direction);
	void setColor(uint32_t color, Logic::SIDE_INDEX side);

public:
	Vector4 position;
	Matrix4 modelTransformation;
	Matrix4 animationTransformation;
	BasicVertex *m_basicVertices;
	uint16_t	*m_basicIndices;
	int32_t m_basicVerticesUId;
	int32_t	m_basicIndiceUId; 

	int idx;
	static int currIdx;
};

