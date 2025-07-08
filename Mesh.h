#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include <string>
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
public:
	Node(const std::string& name ,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void RenderTree() noexcept;
private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
private:
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model(Graphics& gfx, std::string filename) noexcept(!IS_DEBUG);
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node) noxnd;
	void Draw(Graphics& gfx) const noxnd;
	void ShowWindow( const char* WindowName ) noexcept;
	void Reset() noexcept;
private:
	struct
	{
		float roll =	0.0f;
		float pitch =	0.0f;
		float yaw =		0.0f;
		float x =		0.0f;
		float y =		0.0f;
		float z =		0.0f;
	}pos;
	std::unique_ptr<Node> pRoot;
	std::vector< std::unique_ptr<Mesh>> MeshPtrs;
};