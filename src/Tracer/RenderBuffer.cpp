#include "Tracer/RenderBuffer.h"

/************************************************************************/
/* Render Buffer Function Definition                                    */
/************************************************************************/
namespace Kaguya
{

RenderBuffer::RenderBuffer(uint32_t w, uint32_t h)
	: width(w), height(h), size(w * h)
	, beauty(size << 2)
	, diff(size << 2), spec(size << 2)
	//, fl(size * 3), fr(size * 3), sss(size * 3)
	, p(size * 3), n(size * 3)
	, dpdu(size * 3), dpdv(size * 3)
	, dndu(size * 3), dndv(size * 3)
	, uv(size << 1), z(size), id(size)
{
}

void RenderBuffer::resize(uint32_t w, uint32_t h)
{
	clear();

	width = w;
	height = h;

	size = w * h;
	size_t size2 = size << 1;
	size_t size3 = size * 3;
	size_t size4 = size << 2;

	beauty.resize(size4);
	diff.resize(size4);
	spec.resize(size4);


	p.resize(size3);
	n.resize(size3);

	uv.resize(size2);

	dpdu.resize(size3);
	dpdv.resize(size3);
	dndu.resize(size3);
	dndv.resize(size3);

	z.resize(size);
	id.resize(size);
}

void RenderBuffer::clear()
{
	p.clear();
	n.clear();

	uv.clear();

	dpdu.clear();
	dpdv.clear();
	dndu.clear();
	dndv.clear();

	z.clear();
	id.clear();
}

bool RenderBuffer::empty() const
{
	return p.size() == 0;
}
void RenderBuffer::setBuffer(uint32_t x, uint32_t y,
							 const Intersection &isec, Float zdepth)
{
	size_t index = y * width + x;
	if (index >= size) return;

	size_t id3 = index * 3, id2 = index << 1;

	Vec3ToFloats(isec.P, p, id3);
	Vec3ToFloats(isec.Ng, n, id3);
	Vec3ToFloats(isec.dPdu, dpdu, id3);
	Vec3ToFloats(isec.dPdv, dpdv, id3);
	Vec3ToFloats(isec.dNdu, dndu, id3);
	Vec3ToFloats(isec.dNdv, dndv, id3);

	Vec2ToFloats(isec.UV, uv, id2);
	z[index] = static_cast<float>(zdepth);
	id[index] = isec.shape->getShapeID();
}

}
