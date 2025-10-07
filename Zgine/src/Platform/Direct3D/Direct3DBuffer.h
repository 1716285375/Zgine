#pragma once

#include "Zgine/Renderer/Buffer.h"

namespace Zgine {

	class Direct3DBuffer : public VertexBuffer
	{
	public:
		
		virtual void Bind() const;
		virtual void Unbind() const;
	private:

	};
}