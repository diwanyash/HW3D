#include "Mesh.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindablePtr)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindablePtr)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>(pi));
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}
	AddBind(std::make_unique<TransformCBuff>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

Node::Node(const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	:
	name( name ),
	meshPtrs( std::move(meshPtrs) )
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built = DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
	for (auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::ShowTree() const noexcept
{
	if ( ImGui::TreeNode( name.c_str() ) )
	{
		for (const auto& pchild : childPtrs)
		{
			pchild->ShowTree();
		}
		ImGui::TreePop();
	}
}

//Window
class ModelWindow // Pointer to IMPLementation IDIOM ( PIMPL IDIOM )
{
public:
	void Show(const char* WindowName, const Node& root) noexcept
	{
		WindowName = WindowName ? WindowName : "Model";

		if (ImGui::Begin(WindowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree();

			ImGui::NextColumn();
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

			ImGui::Text("Position");
			ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);

			if (ImGui::Button("Reset"))
			{
				pos.roll = 0.0f;
				pos.pitch = 0.0f;
				pos.yaw = 0.0f;
				pos.x = 0.0f;
				pos.y = 0.0f;
				pos.z = 0.0f;
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
			DirectX::XMMatrixTranslation(pos.x, pos.y - 10.0f, pos.z + 6.0f);
	}
	void Reset() noexcept
	{
		pos.roll = 0.0f;
		pos.pitch = 0.0f;
		pos.yaw = 0.0f;
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 0.0f;
	}
private:
	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	}pos;
};

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

Model::Model(Graphics& gfx, std::string filename) noexcept(!IS_DEBUG)
	:
	pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		MeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}
	pRoot = ParseNode(*pScene->mRootNode);

}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);

	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector< std::unique_ptr<Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));
	bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"Shaderm\\PhongPS.cso"));
	bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 0.0f, 1.0f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} colorConst;

	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node) noxnd
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto MeshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(MeshPtrs.at(MeshIdx).get());
	}

	auto pNode = std::make_unique<Node>( node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}

void Model::ShowWindow(const char* WindowName) noexcept
{
	pWindow->Show(WindowName, *pRoot);
}

void Model::Reset() noexcept
{
	pWindow->Reset();
}

Model::~Model() noexcept
{}

void Model::Draw(Graphics& gfx) const noxnd
{
	pRoot->Node::Draw(gfx, pWindow->GetTransform());
}
























