#pragma once

#include "tm.h"

class Light : public TriangleMesh
{
public:
	Light() {}

	void SetAsAACube() override {
#if 0
		5 ----- 6
	  / |     / |
	1 --+-- 2   |
	|   4 --+-- 7
	| /     | /
	0 ----- 3
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

		m_Positions.push_back(glm::vec3(-0.5f, -0.5f, 0.5f)); // BLF
		m_Positions.push_back(glm::vec3(-0.5f, 0.5f, 0.5f)); // TLF
		m_Positions.push_back(glm::vec3(0.5f, 0.5f, 0.5f)); // TRF
		m_Positions.push_back(glm::vec3(0.5f, -0.5f, 0.5f)); // BRF
		m_Positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f)); // BLB
		m_Positions.push_back(glm::vec3(-0.5f, 0.5f, -0.5f)); // TLB
		m_Positions.push_back(glm::vec3(0.5f, 0.5f, -0.5f)); // TRB
		m_Positions.push_back(glm::vec3(0.5f, -0.5f, -0.5f));  // BRB

		m_Colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLF - Red
		m_Colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLF - Yellow
		m_Colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRF - Blue
		m_Colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRF - Green
		m_Colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // BLB - Red
		m_Colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // TLB - Yellow
		m_Colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // TRB - Blue
		m_Colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // BRB - Green

		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // POS Z
		m_Normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // POS X
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f)); // NEG Z
		m_Normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f)); // NEG X
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f)); // NEG Y
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // POS Y

		m_TexCoords.push_back(glm::vec3(-1.0f, -1.0f, 1.0f)); // BLF
		m_TexCoords.push_back(glm::vec3(-1.0f, 1.0f, 1.0f)); // TLF
		m_TexCoords.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // TRF
		m_TexCoords.push_back(glm::vec3(1.0f, -1.0f, 1.0f)); // BRF
		m_TexCoords.push_back(glm::vec3(-1.0f, -1.0f, -1.0f)); // BLB
		m_TexCoords.push_back(glm::vec3(-1.0f, 1.0f, -1.0f)); // TLB
		m_TexCoords.push_back(glm::vec3(1.0f, 1.0f, -1.0f)); // TRB
		m_TexCoords.push_back(glm::vec3(1.0f, -1.0f, -1.0f)); // BRB

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			unsigned int corner = m_Indices[i];
			connectivityPush3f(m_Positions[corner]);
			connectivityPush3f(m_Colors[corner]);
			connectivityPush3f(m_Normals[i / 6]);
			connectivityPush3f(m_TexCoords[corner]);

			normalPush3f(m_Positions[corner]);
			normalPush3f(m_Positions[corner] + m_Normals[i / 6]);
		}

		for (unsigned int i = 0; i < m_VerticesN; i++) {
			m_Indices[i] = i;
		}

		m_ShaderProgram = Shader("position.vert", "uniformColor.frag");
		m_ShaderProgramNormals = Shader("position.vert", "uniformColor.frag");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_ConnectivityData.size() * sizeof(float), m_ConnectivityData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

		int stride = (int)m_ConnectivityData.size() / (int)m_Indices.size() * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		// Unbinds
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glGenVertexArrays(1, &nVAO);
		glGenBuffers(1, &nVBO);
		glBindVertexArray(nVAO);

		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NormalData.size() * sizeof(float), m_NormalData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw() override {
		m_ShaderProgram.use();
		m_ShaderProgram.setMat4("model", m_Model);
		m_ShaderProgram.setMat4("view", m_View);
		m_ShaderProgram.setMat4("proj", m_Proj);

		m_ShaderProgram.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_VerticesN, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};
