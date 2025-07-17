#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include "Sampler.h"
#include <memory>

namespace dx = DirectX;
 
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<bind::Bindable>> bindablePtr)
{
	AddBind(std::make_shared<bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindablePtr)
	{
		AddBind(std::move(pb));
	}
	AddBind(std::make_shared<bind::TransformCBuff>(gfx, *this));
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

Node::Node(int id,const std::string& name,std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept(!IS_DEBUG)
	:
	id( id ),
	name( name ),
	meshPtrs( std::move(meshPtrs) )
{
	dx::XMStoreFloat4x4(&transform, transform_in);
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built = 
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;
	for (auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4( &appliedTransform, transform );
}

void Node::ShowTree( Node*& pSelectedNode) const noexcept
{
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();

	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0);

	const auto extended = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetId(), node_flags, name.c_str());

	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	if (extended)
	{
		for (const auto& pchild : childPtrs)
		{
			pchild->ShowTree( pSelectedNode);
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

		int nodeIndexTracked = 0;
		if (ImGui::Begin(WindowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree( pSelectedNode );

			ImGui::NextColumn(); 
			if( pSelectedNode != nullptr )
			{
				auto& transform = transforms[ pSelectedNode->GetId() ];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

				if (ImGui::Button("Reset"))
				{
					transform.roll = 0.0f;
					transform.pitch = 0.0f;
					transform.yaw = 0.0f;
					transform.x = 0.0f;
					transform.y = 0.0f;
					transform.z = 0.0f;
				}
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		assert( pSelectedNode != nullptr );
		const auto& transform = transforms.at( pSelectedNode->GetId() );

		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	void Reset() noexcept
	{
		auto& transform = transforms.at( pSelectedNode->GetId() );
		transform.roll = 0.0f;
		transform.pitch = 0.0f;
		transform.yaw = 0.0f;
		transform.x = 0.0f;
		transform.y = 0.0f;
		transform.z = 0.0f;
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

int Node::GetId() const noexcept
{
	return id;
}

Model::Model(Graphics& gfx,const std::string filename) noexcept(!IS_DEBUG)
	:
	pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		MeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials ));
	}
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);

}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using Dvtx::VertexLayout;
	using namespace bind;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
			mesh.mVertices[i].x = mesh.mVertices[i].x * scaler;
			mesh.mVertices[i].y = mesh.mVertices[i].y * scaler;
			mesh.mVertices[i].z = mesh.mVertices[i].z * scaler;

		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

	std::vector< std::shared_ptr<bind::Bindable>> bindablePtrs;
	bool hasSpecularMap = false;

	

	using namespace std::string_literals;
	const auto base = "E:/chilli game dev/HW3D/model/nano_textured/"s;

	float shininess = 35.0f;
	if ( mesh.mMaterialIndex >= 0 )
	{
		auto& material = *pMaterials[ mesh.mMaterialIndex ];
		aiString texFileName;
		material.GetTexture( aiTextureType_DIFFUSE, 0, &texFileName );
		bindablePtrs.push_back( Texture::Resolve( gfx, base + texFileName.C_Str()));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back( Texture::Resolve(gfx,base + texFileName.C_Str(), 1));
			hasSpecularMap = true;
		}
		else
		{
			material.Get( AI_MATKEY_SHININESS, shininess );
		}
		bindablePtrs.push_back( Sampler::Resolve( gfx ) );
	}

	auto meshTag = base + "%" + mesh.mName.C_Str();

	bindablePtrs.push_back( VertexBuffer::Resolve(gfx, meshTag ,vbuf));
	bindablePtrs.push_back( IndexBuffer::Resolve(gfx, meshTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "Shaderm\\TexturedPhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back( InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	if (hasSpecularMap)
	{
		bindablePtrs.push_back( PixelShader::Resolve(gfx, "Shaderm\\TexturedSpecPhongPS.cso"));
	}
	else
	{
		bindablePtrs.push_back( PixelShader::Resolve(gfx, "Shaderm\\TexturedPhongPS.cso") );

		struct PSMaterialConstant
		{
			float specularIntensity = 1.6f;
			float specularPower;
			float padding[2];
		} colorConst;
		colorConst.specularPower = shininess;
	
		bindablePtrs.push_back( PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, colorConst, 1u));
	}


	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noxnd
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

	auto pNode = std::make_unique<Node>( nextId++,node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode( nextId, *node.mChildren[i]));
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
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform( pWindow->GetTransform() );
	}
	pRoot->Node::Draw(gfx, dx::XMMatrixIdentity() );
}
























