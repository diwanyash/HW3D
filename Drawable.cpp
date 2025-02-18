#include "Drawable.h"
#include "IndexBuffer.h"

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("Use IndexBuffer to Bind Index Buffer" && typeid(*bind) != typeid(IndexBuffer) );
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{
	assert("Can't Add IndexBuffer Again ( Second Time )" && pIndexBuffer == nullptr );
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
