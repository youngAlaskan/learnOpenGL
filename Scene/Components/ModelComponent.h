#pragma once

class Model;

struct ModelComponent
{
	std::weak_ptr<Model> ParentModel;

	ModelComponent() = default;
	explicit ModelComponent(const std::shared_ptr<Model>& model)
		: ParentModel(std::weak_ptr<Model>(model)) {}
	explicit ModelComponent(Model* model)
		: ParentModel(std::weak_ptr<Model>(std::shared_ptr<Model>(model))) {}
	ModelComponent(ModelComponent&) = default;
	~ModelComponent() = default;
};