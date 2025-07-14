#include "Drawable.h"
#include "IndexBuffer.h"
#include "ConditionalNoexcept.h"
#include <cassert>
#include "Bindable.h"

using namespace bind;

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<bind::Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(*bind) == typeid(bind::IndexBuffer))
	{
		assert("can't Bind IndexBuffer to Index Buffer Twice" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<bind::IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
}
