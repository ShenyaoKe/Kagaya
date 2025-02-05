#pragma once
#include "Geometry/Mesh.h"

namespace Kaguya
{

class SubdMesh : public Mesh
{
public:
	SubdMesh();
	~SubdMesh();

	GeometryType primitiveType() const override
	{
		return GeometryType::SUBDIVISION_MESH;
	}
};

}
