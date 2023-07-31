#pragma once

#include "TransformComponent.h"

enum class DrawingMode {
	ISOLATED,
	LIT_OBJECT,
	MIRROR,
	REFRACTOR,
	LINES,
	SKYBOX,
	SCREEN
};

enum class DrawableType
{
	LINE,
	TRIANGLE_MESH,
	CUBE_MAP,
	SCREEN
};

class Drawable
{
public:
	Drawable();
	bool operator==(const Drawable& other) const;
	virtual ~Drawable();

public:
	unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;
	TransformComponent m_TransformComponent{};
	DrawableType m_Type;
};
