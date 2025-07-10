#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include <string>
#include <optional>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindablePtr);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(int id, const std::string& name ,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
private:
	void ShowTree(Node*& pSelectedNode ) const noexcept;
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
	int GetId() const noexcept;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, std::string filename) noexcept(!IS_DEBUG);
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(int& nextID, const aiNode& node) noxnd;
	void Draw(Graphics& gfx) const noxnd;
	void ShowWindow( const char* WindowName ) noexcept;
	void Reset() noexcept;
	~Model() noexcept;
private:
	static constexpr float scaler = 1.0f;
	std::unique_ptr<class ModelWindow> pWindow;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> MeshPtrs;
};