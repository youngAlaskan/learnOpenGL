#pragma once

#include <string>

struct TagComponent
{
	std::string Tag = std::string();

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	explicit TagComponent(std::string tag)
		: Tag(std::move(tag)) {}
	~TagComponent() = default;

	operator std::string& () { return Tag; }
	operator const std::string& () const { return Tag; }
};
