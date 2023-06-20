#pragma once

#include <glm/glm.hpp>
#include <iostream>

// float vertices[] = {
// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
// 	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
// 
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
// 	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 
// 	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 
// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
// };

enum plane {
	XY,
	XZ,
	YZ
};

class TriangleMesh
{
public:
	TriangleMesh() {}

	TriangleMesh(glm::vec4* vertices, glm::vec4* colors, glm::vec4* texCoords, unsigned int* indices, unsigned int verticesN, unsigned int trianglesN) {
		m_TrianglesN = trianglesN;
		m_VerticesN = verticesN;
		m_ConnectivityData = new float[12 * verticesN];
		m_Indices = indices;

		for (unsigned int i = 0; i < verticesN; i++)
		{
			unsigned int offset = 12 * i;
			// vertices
			if (vertices)
			{
				m_ConnectivityData[offset++] = vertices[i].x;
				m_ConnectivityData[offset++] = vertices[i].y;
				m_ConnectivityData[offset++] = vertices[i].z;
				m_ConnectivityData[offset++] = vertices[i].w;
			}
			else
			{
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
			}

			// Colors
			if (colors)
			{
				m_ConnectivityData[offset++] = colors[i].x;
				m_ConnectivityData[offset++] = colors[i].y;
				m_ConnectivityData[offset++] = colors[i].z;
				m_ConnectivityData[offset++] = colors[i].w;
			}
			else
			{
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
			}

			// texture Coordinates
			if (texCoords)
			{
				m_ConnectivityData[offset++] = texCoords[i].x;
				m_ConnectivityData[offset++] = texCoords[i].y;
				m_ConnectivityData[offset++] = texCoords[i].z;
				m_ConnectivityData[offset++] = texCoords[i].w;
			}
			else
			{
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
				m_ConnectivityData[offset++] = 0.0f;
			}
		}
	}

	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 0.0f) {
		if (aaPlane > 2 || aaPlane < 0) return;
		unsigned int index = 0;

		m_VerticesN = 4;
		m_ConnectivityData = new float[12 * m_VerticesN];

		glm::vec4 posBL;
		glm::vec4 posTL;
		glm::vec4 posTR;
		glm::vec4 posBR;

		if (aaPlane == XY)
		{
			posBL = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
			posTL = glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
			posTR = glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
			posBR = glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
		}
		else if (aaPlane == XZ)
		{
			posBL = glm::vec4(-0.5f, 0.0f,  0.5f, 1.0f);
			posTL = glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);
			posTR = glm::vec4( 0.5f, 0.0f, -0.5f, 1.0f);
			posBR = glm::vec4( 0.5f, 0.0f,  0.5f, 1.0f);
		}
		else
		{
			posBL = glm::vec4(0.0f, -0.5f,  0.5f, 1.0f);
			posTL = glm::vec4(0.0f,  0.5f,  0.5f, 1.0f);
			posTR = glm::vec4(0.5f,  0.5f, -0.5f, 1.0f);
			posBR = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
		}

		glm::vec4 colorBL = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		glm::vec4 colorTL = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		glm::vec4 colorTR = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		glm::vec4 colorBR = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green

		glm::vec4 texCoordBL = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) * texCoordScaleFactor;
		glm::vec4 texCoordTL = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) * texCoordScaleFactor;
		glm::vec4 texCoordTR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) * texCoordScaleFactor;
		glm::vec4 texCoordBR = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * texCoordScaleFactor;

		// Bottom Left
		// -----------
		// Vertices
		m_ConnectivityData[index++] = posBL.x;
		m_ConnectivityData[index++] = posBL.y;
		m_ConnectivityData[index++] = posBL.z;
		m_ConnectivityData[index++] = posBL.w;

		// Colors
		m_ConnectivityData[index++] = colorBL.x;
		m_ConnectivityData[index++] = colorBL.y;
		m_ConnectivityData[index++] = colorBL.z;
		m_ConnectivityData[index++] = colorBL.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBL.x;
		m_ConnectivityData[index++] = texCoordBL.y;
		m_ConnectivityData[index++] = texCoordBL.z;
		m_ConnectivityData[index++] = texCoordBL.w;

		// Top Left
		// --------
		// Vertices
		m_ConnectivityData[index++] = posTL.x;
		m_ConnectivityData[index++] = posTL.y;
		m_ConnectivityData[index++] = posTL.z;
		m_ConnectivityData[index++] = posTL.w;

		// Colors
		m_ConnectivityData[index++] = colorTL.x;
		m_ConnectivityData[index++] = colorTL.y;
		m_ConnectivityData[index++] = colorTL.z;
		m_ConnectivityData[index++] = colorTL.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTL.x;
		m_ConnectivityData[index++] = texCoordTL.y;
		m_ConnectivityData[index++] = texCoordTL.z;
		m_ConnectivityData[index++] = texCoordTL.w;

		// Top Right
		// ---------
		// Vertices
		m_ConnectivityData[index++] = posTR.x;
		m_ConnectivityData[index++] = posTR.y;
		m_ConnectivityData[index++] = posTR.z;
		m_ConnectivityData[index++] = posTR.w;

		// Colors
		m_ConnectivityData[index++] = colorTR.x;
		m_ConnectivityData[index++] = colorTR.y;
		m_ConnectivityData[index++] = colorTR.z;
		m_ConnectivityData[index++] = colorTR.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTR.x;
		m_ConnectivityData[index++] = texCoordTR.y;
		m_ConnectivityData[index++] = texCoordTR.z;
		m_ConnectivityData[index++] = texCoordTR.w;

		// Bottom Right
		// ------------
		// Vertices
		m_ConnectivityData[index++] = posBR.x;
		m_ConnectivityData[index++] = posBR.y;
		m_ConnectivityData[index++] = posBR.z;
		m_ConnectivityData[index++] = posBR.w;

		// Colors
		m_ConnectivityData[index++] = colorBR.x;
		m_ConnectivityData[index++] = colorBR.y;
		m_ConnectivityData[index++] = colorBR.z;
		m_ConnectivityData[index++] = colorBR.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBR.x;
		m_ConnectivityData[index++] = texCoordBR.y;
		m_ConnectivityData[index++] = texCoordBR.z;
		m_ConnectivityData[index++] = texCoordBR.w;

		// Indexing
		// --------
		m_TrianglesN = 2;
		m_Indices = new unsigned int[m_TrianglesN * 3];

		m_Indices[0] = 0;
		m_Indices[1] = 1;
		m_Indices[2] = 2;

		m_Indices[3] = 0;
		m_Indices[4] = 2;
		m_Indices[5] = 3;

	}
	
	void SetAsAACube() {
		unsigned int index = 0;

		m_VerticesN = 8;
		m_ConnectivityData = new float[12 * m_VerticesN];

		glm::vec4 posBLF = glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f);
		glm::vec4 posTLF = glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f);
		glm::vec4 posTRF = glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f);
		glm::vec4 posBRF = glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f);
		glm::vec4 posBLB = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
		glm::vec4 posTLB = glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f);
		glm::vec4 posTRB = glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f);
		glm::vec4 posBRB = glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f);

		glm::vec4 colorBLF = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		glm::vec4 colorTLF = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		glm::vec4 colorTRF = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		glm::vec4 colorBRF = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
		glm::vec4 colorBLB = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		glm::vec4 colorTLB = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		glm::vec4 colorTRB = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		glm::vec4 colorBRB = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green

		glm::vec4 texCoordBLF = glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f);
		glm::vec4 texCoordTLF = glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f);
		glm::vec4 texCoordTRF = glm::vec4( 1.0f,  1.0f,  1.0f, 1.0f);
		glm::vec4 texCoordBRF = glm::vec4( 1.0f, -1.0f,  1.0f, 1.0f);
		glm::vec4 texCoordBLB = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
		glm::vec4 texCoordTLB = glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f);
		glm::vec4 texCoordTRB = glm::vec4( 1.0f,  1.0f, -1.0f, 1.0f);
		glm::vec4 texCoordBRB = glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f);

		// Front
		// -----
		// Bottom Left
		// -----------
		// Vertices
		m_ConnectivityData[index++] = posBLF.x;
		m_ConnectivityData[index++] = posBLF.y;
		m_ConnectivityData[index++] = posBLF.z;
		m_ConnectivityData[index++] = posBLF.w;

		// Colors
		m_ConnectivityData[index++] = colorBLF.x;
		m_ConnectivityData[index++] = colorBLF.y;
		m_ConnectivityData[index++] = colorBLF.z;
		m_ConnectivityData[index++] = colorBLF.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBLF.x;
		m_ConnectivityData[index++] = texCoordBLF.y;
		m_ConnectivityData[index++] = texCoordBLF.z;
		m_ConnectivityData[index++] = texCoordBLF.w;

		// Top Left
		// --------
		// Vertices
		m_ConnectivityData[index++] = posTLF.x;
		m_ConnectivityData[index++] = posTLF.y;
		m_ConnectivityData[index++] = posTLF.z;
		m_ConnectivityData[index++] = posTLF.w;

		// Colors
		m_ConnectivityData[index++] = colorTLF.x;
		m_ConnectivityData[index++] = colorTLF.y;
		m_ConnectivityData[index++] = colorTLF.z;
		m_ConnectivityData[index++] = colorTLF.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTLF.x;
		m_ConnectivityData[index++] = texCoordTLF.y;
		m_ConnectivityData[index++] = texCoordTLF.z;
		m_ConnectivityData[index++] = texCoordTLF.w;

		// Top Right
		// ---------
		// Vertices
		m_ConnectivityData[index++] = posTRF.x;
		m_ConnectivityData[index++] = posTRF.y;
		m_ConnectivityData[index++] = posTRF.z;
		m_ConnectivityData[index++] = posTRF.w;

		// Colors
		m_ConnectivityData[index++] = colorTRF.x;
		m_ConnectivityData[index++] = colorTRF.y;
		m_ConnectivityData[index++] = colorTRF.z;
		m_ConnectivityData[index++] = colorTRF.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTRF.x;
		m_ConnectivityData[index++] = texCoordTRF.y;
		m_ConnectivityData[index++] = texCoordTRF.z;
		m_ConnectivityData[index++] = texCoordTRF.w;

		// Bottom Right
		// ------------
		// Vertices
		m_ConnectivityData[index++] = posBRF.x;
		m_ConnectivityData[index++] = posBRF.y;
		m_ConnectivityData[index++] = posBRF.z;
		m_ConnectivityData[index++] = posBRF.w;

		// Colors
		m_ConnectivityData[index++] = colorBRF.x;
		m_ConnectivityData[index++] = colorBRF.y;
		m_ConnectivityData[index++] = colorBRF.z;
		m_ConnectivityData[index++] = colorBRF.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBRF.x;
		m_ConnectivityData[index++] = texCoordBRF.y;
		m_ConnectivityData[index++] = texCoordBRF.z;
		m_ConnectivityData[index++] = texCoordBRF.w;

		// Back
		// ----
		// Bottom Left
		// -----------
		// Vertices
		m_ConnectivityData[index++] = posBLB.x;
		m_ConnectivityData[index++] = posBLB.y;
		m_ConnectivityData[index++] = posBLB.z;
		m_ConnectivityData[index++] = posBLB.w;

		// Colors
		m_ConnectivityData[index++] = colorBLB.x;
		m_ConnectivityData[index++] = colorBLB.y;
		m_ConnectivityData[index++] = colorBLB.z;
		m_ConnectivityData[index++] = colorBLB.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBLB.x;
		m_ConnectivityData[index++] = texCoordBLB.y;
		m_ConnectivityData[index++] = texCoordBLB.z;
		m_ConnectivityData[index++] = texCoordBLB.w;

		// Top Left
		// --------
		// Vertices
		m_ConnectivityData[index++] = posTLB.x;
		m_ConnectivityData[index++] = posTLB.y;
		m_ConnectivityData[index++] = posTLB.z;
		m_ConnectivityData[index++] = posTLB.w;

		// Colors
		m_ConnectivityData[index++] = colorTLB.x;
		m_ConnectivityData[index++] = colorTLB.y;
		m_ConnectivityData[index++] = colorTLB.z;
		m_ConnectivityData[index++] = colorTLB.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTLB.x;
		m_ConnectivityData[index++] = texCoordTLB.y;
		m_ConnectivityData[index++] = texCoordTLB.z;
		m_ConnectivityData[index++] = texCoordTLB.w;

		// Top Right
		// ---------
		// Vertices
		m_ConnectivityData[index++] = posTRB.x;
		m_ConnectivityData[index++] = posTRB.y;
		m_ConnectivityData[index++] = posTRB.z;
		m_ConnectivityData[index++] = posTRB.w;

		// Colors
		m_ConnectivityData[index++] = colorTRB.x;
		m_ConnectivityData[index++] = colorTRB.y;
		m_ConnectivityData[index++] = colorTRB.z;
		m_ConnectivityData[index++] = colorTRB.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordTRB.x;
		m_ConnectivityData[index++] = texCoordTRB.y;
		m_ConnectivityData[index++] = texCoordTRB.z;
		m_ConnectivityData[index++] = texCoordTRB.w;

		// Bottom Right
		// ------------
		// Vertices
		m_ConnectivityData[index++] = posBRB.x;
		m_ConnectivityData[index++] = posBRB.y;
		m_ConnectivityData[index++] = posBRB.z;
		m_ConnectivityData[index++] = posBRB.w;

		// Colors
		m_ConnectivityData[index++] = colorBRB.x;
		m_ConnectivityData[index++] = colorBRB.y;
		m_ConnectivityData[index++] = colorBRB.z;
		m_ConnectivityData[index++] = colorBRB.w;

		// texture Coordinates
		m_ConnectivityData[index++] = texCoordBRB.x;
		m_ConnectivityData[index++] = texCoordBRB.y;
		m_ConnectivityData[index++] = texCoordBRB.z;
		m_ConnectivityData[index++] = texCoordBRB.w;

		// Indexing
		// --------
#if 0
		    5-----6
		  / |   / |
		1---+-2   |
		|   4-+---7
        | /   | /
		0-----3
#endif
		enum corners { BLF, TLF, TRF, BRF, BLB, TLB, TRB, BRB };
		index = 0;
		m_TrianglesN = 12;
		m_Indices = new unsigned int[m_TrianglesN * 3];

		// Front
		m_Indices[index++] = BLF;
		m_Indices[index++] = TLF;
		m_Indices[index++] = TRF;

		m_Indices[index++] = BLF;
		m_Indices[index++] = TRF;
		m_Indices[index++] = BRF;

		// Right
		m_Indices[index++] = BRF;
		m_Indices[index++] = TRF;
		m_Indices[index++] = TRB;

		m_Indices[index++] = BRF;
		m_Indices[index++] = TRB;
		m_Indices[index++] = BRB;

		// Back
		m_Indices[index++] = BRB;
		m_Indices[index++] = BLB;
		m_Indices[index++] = TLB;

		m_Indices[index++] = BRB;
		m_Indices[index++] = TLB;
		m_Indices[index++] = TRB;

		// Left
		m_Indices[index++] = BLB;
		m_Indices[index++] = BLF;
		m_Indices[index++] = TLF;

		m_Indices[index++] = BLB;
		m_Indices[index++] = TLF;
		m_Indices[index++] = TLB;

		// Bottom
		m_Indices[index++] = BRF;
		m_Indices[index++] = BLF;
		m_Indices[index++] = BLB;

		m_Indices[index++] = BRF;
		m_Indices[index++] = BLB;
		m_Indices[index++] = BRB;

		// Top
		m_Indices[index++] = TRF;
		m_Indices[index++] = TLF;
		m_Indices[index++] = TLB;

		m_Indices[index++] = TRF;
		m_Indices[index++] = TLB;
		m_Indices[index++] = TRB;

	}

	void print(unsigned int start = 0, unsigned int end = 0) {
		std::cout << "Number of Triangles = " << m_TrianglesN << std::endl;

		for (unsigned int i = start; i < 27 * m_TrianglesN && i <= 27 * end; i++)
		{
			if (i == start)
			{
				std::cout << "Triangle: " << start << std::endl << '<';
			}
			else if (i % 27 == 0)
			{
				std::cout << '>' << std::endl << "Triangle: " << i / 27 << std::endl << '<';
			}
			else if (i % 9 == 0)
			{
				std::cout << '>' << std::endl << '<';
			}
			else if (i % 9 == 3 || i % 9 == 7)
			{
				std::cout << ">, <";
			}
			else
			{
				std::cout << ", ";
			}

			std::cout << m_ConnectivityData[i];
		}

		std::cout << '>' << std::endl;
	}

	~TriangleMesh()
	{
		if (m_ConnectivityData) delete m_ConnectivityData;
	}

public:
	float* m_ConnectivityData;
	unsigned int* m_Indices;
	unsigned int m_TrianglesN;
	unsigned int m_VerticesN;
};
