/*!
* \class PolyMesh
*
* \brief
*
* \author Shenyao Ke
* \date December 2016
*/
#pragma once
#include "Geometry/Mesh.h"
#include "Geometry/PrimitiveAttribute.h"

namespace Kaguya
{

enum class PolyMeshType
{
	TRIANGLE,
	QUAD
};

// Used for Embree
struct TessTrait
{
	size_t      byteOffset = 0;
	size_t      byteStride = 0;
	const void* data = nullptr;
};

struct TessBuffer
{
	std::vector<TessTrait> vertTraits;
	TessTrait              indexTrait;
	size_t                 nPrimtives;
	size_t                 nVertices;
	size_t                 nTimeStep;
	uint32_t               nGeomId;
};

class PolyMesh : public Mesh
{
public:
	PolyMesh() {}
	PolyMesh(std::vector<Point3f>             &vertexBuffer,
			 std::vector<uint32_t>            &indexBuffer,
			 size_t                            vertexCount,
			 size_t                            faceCount,
			 std::shared_ptr<TextureAttribute> texAttri,
			 std::shared_ptr<NormalAttribute>  normAttri);
	~PolyMesh();

	GeometryType primitiveType() const override
	{
		return GeometryType::POLYGONAL_MESH;
	}

	virtual PolyMeshType polyMeshType() const = 0;

	virtual void getTessellated(TessBuffer &trait) const = 0;

	void getRenderBuffer(RenderBufferTrait* trait) const override;

	static size_t tessellatedCount(std::vector<uint32_t> &faceSizeBuffer, size_t faceSize);

	static std::shared_ptr<PolyMesh> createPolyMesh(std::vector<Point3f>             &vertexBuffer,
													std::vector<uint32_t>            &indexBuffer,
													std::vector<uint32_t>            &faceSizeBuffer,
													std::shared_ptr<TextureAttribute> texAttri,
													std::shared_ptr<NormalAttribute>  normAttri);

	static std::shared_ptr<TriangleMesh> createTriMesh(std::vector<Point3f>             &vertexBuffer,
													   std::vector<uint32_t>            &indexBuffer,
													   std::vector<uint32_t>            &faceSizeBuffer,
													   std::shared_ptr<TextureAttribute> texAttri,
													   std::shared_ptr<NormalAttribute>  normAttri);

protected:
	virtual void tessellate(std::vector<uint32_t> &indexBuffer,
							std::vector<uint32_t> &faceSizeBuffer,
							size_t                 tessellatedCount) = 0;

protected:
	std::vector<Point3f>              mVertexBuffer;
	std::vector<uint32_t>             mIndexBuffer;
	size_t                            mVertexCount;
	size_t                            mFaceCount;

	std::shared_ptr<TextureAttribute> mTextureAttribute;
	std::shared_ptr<NormalAttribute>  mNormalAttibute;
};

}
