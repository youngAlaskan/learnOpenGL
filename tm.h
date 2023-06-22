#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "line.h"

enum plane {
	XY,
	XZ,
	YZ
};

class TriangleMesh
{
public:
	TriangleMesh() {}

	TriangleMesh(glm::vec3* vertices, glm::vec3* colors, glm::vec3* normals, glm::vec3* texCoords, unsigned int* indices, unsigned int verticesN, unsigned int trianglesN) {
		m_TrianglesN = trianglesN;
		m_VerticesN = verticesN;

		for (unsigned int i = 0; i < verticesN; i++)
		{
			unsigned int offset = 12 * i;
			// vertices
			if (vertices)
			{
				m_ConnectivityData.push_back(vertices[i].x);
				m_ConnectivityData.push_back(vertices[i].y);
				m_ConnectivityData.push_back(vertices[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// Colors
			if (colors)
			{
				m_ConnectivityData.push_back(colors[i].x);
				m_ConnectivityData.push_back(colors[i].y);
				m_ConnectivityData.push_back(colors[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// Normals
			if (normals)
			{
				m_ConnectivityData.push_back(normals[i].x);
				m_ConnectivityData.push_back(normals[i].y);
				m_ConnectivityData.push_back(normals[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}

			// texture Coordinates
			if (texCoords)
			{
				m_ConnectivityData.push_back(texCoords[i].x);
				m_ConnectivityData.push_back(texCoords[i].y);
				m_ConnectivityData.push_back(texCoords[i].z);
			}
			else
			{
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
				m_ConnectivityData.push_back(0.0f);
			}
		}
	}

	// TODO: Implement normals
	void SetAsAARectangle(int aaPlane, float texCoordScaleFactor = 0.0f) {
		if (aaPlane > 2 || aaPlane < 0) return;
		int index = 0;

		m_VerticesN = 4;
		m_TrianglesN = 2;
		m_Indices.reserve(m_TrianglesN * 3);
		m_ConnectivityData.reserve(12 * m_VerticesN);

		glm::vec3 posBL;
		glm::vec3 posTL;
		glm::vec3 posTR;
		glm::vec3 posBR;

		if (aaPlane == XY)
		{
			posBL = glm::vec3(-0.5f, -0.5f, 0.0f);
			posTL = glm::vec3(-0.5f,  0.5f, 0.0f);
			posTR = glm::vec3( 0.5f,  0.5f, 0.0f);
			posBR = glm::vec3( 0.5f, -0.5f, 0.0f);
		}
		else if (aaPlane == XZ)
		{
			posBL = glm::vec3(-0.5f, 0.0f,  0.5f);
			posTL = glm::vec3(-0.5f, 0.0f, -0.5f);
			posTR = glm::vec3( 0.5f, 0.0f, -0.5f);
			posBR = glm::vec3( 0.5f, 0.0f,  0.5f);
		}
		else
		{
			posBL = glm::vec3(0.0f, -0.5f,  0.5f);
			posTL = glm::vec3(0.0f,  0.5f,  0.5f);
			posTR = glm::vec3(0.5f,  0.5f, -0.5f);
			posBR = glm::vec3(0.5f, -0.5f, -0.5f);
		}

		glm::vec3 colorBL = glm::vec3(1.0f, 0.0f, 0.0f); // Red
		glm::vec3 colorTL = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
		glm::vec3 colorTR = glm::vec3(0.0f, 0.0f, 1.0f); // Blue
		glm::vec3 colorBR = glm::vec3(0.0f, 1.0f, 0.0f); // Green

		glm::vec3 texCoordBL = glm::vec3(0.0f, 0.0f, 0.0f) * texCoordScaleFactor;
		glm::vec3 texCoordTL = glm::vec3(0.0f, 1.0f, 0.0f) * texCoordScaleFactor;
		glm::vec3 texCoordTR = glm::vec3(1.0f, 1.0f, 0.0f) * texCoordScaleFactor;
		glm::vec3 texCoordBR = glm::vec3(1.0f, 0.0f, 0.0f) * texCoordScaleFactor;

		// Bottom Left
		// -----------
		connectivityPush3f(posBL);
		connectivityPush3f(colorBL);
		connectivityPush3f(texCoordBL);

		// Top Left
		// --------
		connectivityPush3f(posTL);
		connectivityPush3f(colorTL);
		connectivityPush3f(texCoordTL);

		// Top Right
		// ---------
		connectivityPush3f(posTR);
		connectivityPush3f(colorTR);
		connectivityPush3f(texCoordTR);

		// Bottom Right
		// ------------
		connectivityPush3f(posBR);
		connectivityPush3f(colorBR);
		connectivityPush3f(texCoordBR);

		// Indexing
		// --------
		indicesPush3i(0, 1, 2);
		indicesPush3i(0, 2, 3);

	}
	
	virtual void SetAsAACube() {
#if 0
		5-----6
	  / |   / |
	1---+-2   |
	|   4-+---7
	| /   | /
	0-----3
#endif
		unsigned int BLF = 0, TLF = 1, TRF = 2, BRF = 3, BLB = 4, TLB = 5, TRB = 6, BRB = 7;
		unsigned int index = 0;
		m_TrianglesN = 12;
		m_VerticesN = 3 * m_TrianglesN;
		m_ConnectivityData.reserve(12 * m_VerticesN);
		m_Indices.reserve(m_VerticesN);

		// Front
		indicesPush3i(BLF, TLF, TRF);
		indicesPush3i(BLF, TRF, BRF);

		// Right
		indicesPush3i(BRF, TRF, TRB);
		indicesPush3i(BRF, TRB, BRB);

		// Back
		indicesPush3i(BRB, TRB, TLB);
		indicesPush3i(BRB, TLB, BLB);

		// Left
		indicesPush3i(BLB, TLB, TLF);
		indicesPush3i(BLB, TLF, BLF);

		// Bottom
		indicesPush3i(BRF, BRB, BLB);
		indicesPush3i(BRF, BLB, BLF);

		// Top
		indicesPush3i(TRF, TRB, TLB);
		indicesPush3i(TRF, TLB, TLF);

		std::vector<glm::vec3> positions;
		positions.push_back(glm::vec3(-0.5f, -0.5f,  0.5f)); // BLF
		positions.push_back(glm::vec3(-0.5f,  0.5f,  0.5f)); // TLF
		positions.push_back(glm::vec3( 0.5f,  0.5f,  0.5f)); // TRF
		positions.push_back(glm::vec3( 0.5f, -0.5f,  0.5f)); // BRF
		positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // BLB
		positions.push_back(glm::vec3(-0.5f,  0.5f, -0.5f)); // TLB
		positions.push_back(glm::vec3( 0.5f,  0.5f, -0.5f)); // TRB
		positions.push_back(glm::vec3( 0.5f, -0.5f, -0.5f));  // BRB

		std::vector<glm::vec3> colors;
		colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLF - Red
		colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLF - Yellow
		colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRF - Blue
		colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRF - Green
		colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLB - Red
		colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLB - Yellow
		colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRB - Blue
		colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRB - Green

		std::vector<glm::vec3> normals;
		normals.push_back(glm::vec3( 0.0f,  0.0f,  1.0f)); // POS Z
		normals.push_back(glm::vec3( 1.0f,  0.0f,  0.0f)); // POS X
		normals.push_back(glm::vec3( 0.0f,  0.0f, -1.0f)); // NEG Z
		normals.push_back(glm::vec3(-1.0f,  0.0f,  0.0f)); // NEG X
		normals.push_back(glm::vec3( 0.0f, -1.0f,  0.0f)); // NEG Y
		normals.push_back(glm::vec3( 0.0f,  1.0f,  0.0f)); // POS Y

		std::vector<glm::vec3> texCoords;
		texCoords.push_back(glm::vec3(-1.0f, -1.0f,  1.0f)); // BLF
		texCoords.push_back(glm::vec3(-1.0f,  1.0f,  1.0f)); // TLF
		texCoords.push_back(glm::vec3( 1.0f,  1.0f,  1.0f)); // TRF
		texCoords.push_back(glm::vec3( 1.0f, -1.0f,  1.0f)); // BRF
		texCoords.push_back(glm::vec3(-1.0f, -1.0f, -1.0f)); // BLB
		texCoords.push_back(glm::vec3(-1.0f,  1.0f, -1.0f)); // TLB
		texCoords.push_back(glm::vec3( 1.0f,  1.0f, -1.0f)); // TRB
		texCoords.push_back(glm::vec3( 1.0f, -1.0f, -1.0f)); // BRB

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			unsigned int corner = m_Indices[i];
			connectivityPush3f(positions[corner]);
			connectivityPush3f(colors[corner]);
			connectivityPush3f(normals[i / 6]);
			connectivityPush3f(texCoords[corner]);

			glm::vec3 vertex = positions[corner];

			glm::vec3 dir = normals[i / 6];

			m_Normals.push_back(Line(vertex, vertex + dir));
		}

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			m_Indices[i] = i;
		}

		m_ShaderProgram = Shader("positionColorNormalTex.vert", "litObject.frag");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		int stride = (int) m_ConnectivityData.size() / (int) m_Indices.size() * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void setModel(glm::mat4 model)
	{
		m_Model = model;
	}

	void setView(glm::mat4 view)
	{
		m_View = view;
	}

	void setProj(glm::mat4 proj)
	{
		m_Proj = proj;
	}

	void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 proj)
	{
		m_Model = model;
		m_View = view;
		m_Proj = proj;
	}

	virtual void draw()
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void draw(glm::vec3 lightPos, glm::vec3 cameraPos)
	{
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		m_ShaderProgram.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		m_ShaderProgram.setVec3("lightPos", lightPos);
		m_ShaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ShaderProgram.setVec3("viewPos", cameraPos);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// TODO: Fix normals bug. Normals only showing up in one place
	void drawNormals(glm::vec3 color)
	{
		for (int i = 0; i < m_Normals.size(); i++) {
			m_Normals.at(i).draw(m_Model, m_View, m_Proj, color);
		}
	}

	void print(unsigned int start = 0, unsigned int end = 0) {
		std::cout << "Number of Triangles = " << m_TrianglesN << std::endl;

		for (unsigned int i = start; i < 36 * m_TrianglesN && i < 36 * end; i++)
		{
			if (i == start)
			{
				std::cout << "Triangle: " << start << std::endl << '<';
			}
			else if (i % 36 == 0)
			{
				std::cout << '>' << std::endl << "Triangle: " << i / 36 << std::endl << '<';
			}
			else if (i % 12 == 0)
			{
				std::cout << '>' << std::endl << '<';
			}
			else if (i % 12 == 3 || i % 12 == 6 || i % 12 == 9)
			{
				std::cout << ">, <";
			}
			else
			{
				std::cout << ", ";
			}

			if (m_ConnectivityData[i] >= 0.0f)
				std::cout << ' ';
			std::cout << m_ConnectivityData[i];
		}

		std::cout << '>' << std::endl;
	}

	~TriangleMesh()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteProgram(m_ShaderProgram.ID);
	}

public:
	std::vector<float> m_ConnectivityData;
	std::vector<unsigned int> m_Indices;
	std::vector<Line> m_Normals;
	unsigned int m_TrianglesN, m_VerticesN;
	Shader m_ShaderProgram;
	unsigned int VAO, VBO, EBO;
	glm::mat4 m_Model = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
	glm::mat4 m_Proj = glm::mat4(1.0f);

protected:
	void connectivityPush3f(glm::vec3 v)
	{
		m_ConnectivityData.push_back(v.x);
		m_ConnectivityData.push_back(v.y);
		m_ConnectivityData.push_back(v.z);
	}

	void indicesPush3i(unsigned int x, unsigned int y, unsigned int z)
	{
		m_Indices.push_back(x);
		m_Indices.push_back(y);
		m_Indices.push_back(z);
	}
};
