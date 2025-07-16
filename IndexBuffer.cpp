#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace bind
{
	IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
		:
		IndexBuffer( gfx, "?", indices)
	{}
	IndexBuffer::IndexBuffer(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices)
		:
		tag( tag ),
		count(UINT(indices.size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bid = {};
		bid.Usage = D3D11_USAGE_DEFAULT;
		bid.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bid.CPUAccessFlags = 0u;
		bid.MiscFlags = 0u;
		bid.ByteWidth = UINT(count * sizeof(unsigned short));
		bid.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA sid = {};
		sid.pSysMem = indices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bid, &sid, &pIndexBuffer));
	}

	void IndexBuffer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	}

	std::shared_ptr<Bindable> IndexBuffer::Resolve(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices)
	{
		return Codex::Resolve<IndexBuffer>( gfx, tag, indices );
	}

	std::string IndexBuffer::GetUID() const noexcept
	{
		return GenerateUID_(tag);
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}
	std::string IndexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(IndexBuffer).name() + "#"s + tag;
	}
}