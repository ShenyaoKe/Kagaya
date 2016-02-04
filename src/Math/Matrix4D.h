//
//  Matrix4D.h
//
//  Created by Shenyao Ke on 1/21/15.
//  Copyright (c) 2015 AKIKA. All rights reserved.
//
#pragma once
#ifndef __Matrix4D__
#define __Matrix4D__

#ifndef KAGUYA_DOUBLE_AS_FLOAT
#define KAGUYA_DOUBLE_AS_FLOAT
#endif // !KAGUYA_DOUBLE_AS_FLOAT

#include "Core/Kaguya.h"
#include "Math/CGVector.h"
#include "Math/Matrix3D.h"

class Matrix4D
{
public:
	Matrix4D() : mtx{}
	{
		//Determinant();
	}
	Matrix4D(const Float mat[4][4])
	{
		memcpy(mtx, mat, sizeof(mtx));
	}
	Matrix4D(const Float mat[16])
	{
		memcpy(mtx, mat, sizeof(mtx));
	}
	Matrix4D(const Matrix4D &mat)
	{
		memcpy(mtx, mat.mtx, sizeof(mtx));
	}
	Matrix4D(Float t00, Float t01, Float t02, Float t03,
		Float t10, Float t11, Float t12, Float t13,
		Float t20, Float t21, Float t22, Float t23,
		Float t30, Float t31, Float t32, Float t33)
	{
		mtx[0][0] = t00; mtx[0][1] = t01; mtx[0][2] = t02; mtx[0][3] = t03;
		mtx[1][0] = t10; mtx[1][1] = t11; mtx[1][2] = t12; mtx[1][3] = t13;
		mtx[2][0] = t20; mtx[2][1] = t21; mtx[2][2] = t22; mtx[2][3] = t23;
		mtx[3][0] = t30; mtx[3][1] = t31; mtx[3][2] = t32; mtx[3][3] = t33;
	}

	Matrix4D(Vector4D& col0, Vector4D& col1, Vector4D& col2, Vector4D& col3)
	{
		// Column Major
		mtx[0][0] = col0.x;	mtx[0][1] = col0.y;	mtx[0][2] = col0.z;	mtx[0][3] = col0.w;
		mtx[1][0] = col1.x;	mtx[1][1] = col1.y;	mtx[1][2] = col1.z;	mtx[1][3] = col1.w;
		mtx[2][0] = col2.x;	mtx[2][1] = col2.y;	mtx[2][2] = col2.z;	mtx[2][3] = col2.w;
		mtx[3][0] = col3.x;	mtx[3][1] = col3.y;	mtx[3][2] = col3.z;	mtx[3][3] = col3.w;
		//Determinant();
	}
	Matrix4D(Float val)
	{
		memset(mtx, val, sizeof(mtx));
		//Determinant();
	}
	~Matrix4D()
	{
	}
	
	// Operators
	Float* operator [] (int i);
	const Float* operator [] (int i) const;
	Vector3D operator ()(const Vector3D& v, Float w = 1.0) const;
	const Matrix4D &operator = (const Matrix4D &mat);
	friend Matrix4D operator + (const Matrix4D &m1, const Matrix4D &m2);
	friend Matrix4D operator - (const Matrix4D &m1, const Matrix4D &m2);
	friend Matrix4D operator * (const Matrix4D &m1, const Matrix4D &m2);
	friend Vector4D operator * (const Matrix4D &m, const Vector4D& p);
	friend bool operator == (const Matrix4D &m1, const Matrix4D &m2);
	friend bool operator != (const Matrix4D &m1, const Matrix4D &m2);
	//Vector3D operator * (Vector3D& p) const;
	//Matrix4D operator / (const Matrix4D &) const;

	void zero();
	void setIdentity();
	void printInfo();
	Float Determinant() const;
	Float Minor(int x, int y) const;
	Float Cofactor(int x, int y) const;
	Matrix4D Transpose() const;
	Matrix4D Adjoint() const;
	Matrix4D Inverse() const;

	//Set transformation matrix
	static Matrix4D Identity();
	static Matrix4D Translate(Float tx, Float ty, Float tz);
	static Matrix4D Translate(const Vector3D& vec);
	static Matrix4D RotateX(Float theta);
	static Matrix4D RotateY(Float theta);
	static Matrix4D RotateZ(Float theta);
	static Matrix4D Rotate(Float alpha, Float beta, Float gamma);// in degree
	static Matrix4D Rotate(const Vector3D &axis, Float theta,
		bool isNormalized = false);
	static Matrix4D Scale(Float sx, Float sy, Float sz);
	static Matrix4D Scale(Float scale);
	static Matrix4D Shear(const Vector3D& vec);
	static Matrix4D Reflect(const Vector3D& vec);
	static Matrix4D LookAt(const Point3D &pos = Point3D(0, 0, 0),
		const Point3D &target = Point3D(0, 0, 1), const Vector3D &up = Point3D(0, 1, 0));
	static Matrix4D Perspective(Float verticalAngle = 90, Float aspectRatio = 1.6,
		Float nearPlane = 0.001, Float farPlane = 100);
	static Matrix4D Orthography(Float lf = -1, Float rt = 1,
		Float bt = -1, Float tp = 1, Float nr = -1, Float fr = 1);

	/*template <typename vbo_t>
	friend void exportVBO(const Matrix4D &mat, vbo_t *vtx_array);*/
	template <typename vbo_t>
	void exportVBO(vbo_t *vtx_array) const;

public:		
	// Column Major
	Float mtx[4][4];
	//Float det = 0;
};
inline Float* Matrix4D::operator[](int i)
{
	return mtx[i];
}
inline const Float* Matrix4D::operator[](int i) const
{
	return mtx[i];
}
inline Vector3D Matrix4D::operator ()(const Vector3D& v, Float w) const
{
	Float x = v.x, y = v.y, z = v.z;
	Float xp = mtx[0][0] * x + mtx[1][0] * y + mtx[2][0] * z;
	Float yp = mtx[0][1] * x + mtx[1][1] * y + mtx[2][1] * z;
	Float zp = mtx[0][2] * x + mtx[1][2] * y + mtx[2][2] * z;
	if (w == 1.0)
	{
		xp += mtx[3][0];
		yp += mtx[3][1];
		zp += mtx[3][2];
		w = mtx[0][3] * x + mtx[1][3] * y + mtx[2][3] * z + mtx[3][3];
		if (w != 1.0)
		{
			xp /= w;
			yp /= w;
			zp /= w;
		}
	}
	return Vector3D(xp, yp, zp);
}
inline Matrix4D operator+(const Matrix4D& m1, const Matrix4D& m2)
{
	return Matrix4D(
			m1.mtx[0][0] + m2.mtx[0][0],
			m1.mtx[0][1] + m2.mtx[0][1],
			m1.mtx[0][2] + m2.mtx[0][2],
			m1.mtx[0][3] + m2.mtx[0][3],

			m1.mtx[1][0] + m2.mtx[1][0],
			m1.mtx[1][1] + m2.mtx[1][1],
			m1.mtx[1][2] + m2.mtx[1][2],
			m1.mtx[1][3] + m2.mtx[1][3],

			m1.mtx[2][0] + m2.mtx[2][0],
			m1.mtx[2][1] + m2.mtx[2][1],
			m1.mtx[2][2] + m2.mtx[2][2],
			m1.mtx[2][3] + m2.mtx[2][3],

			m1.mtx[3][0] + m2.mtx[3][0],
			m1.mtx[3][1] + m2.mtx[3][1],
			m1.mtx[3][2] + m2.mtx[3][2],
			m1.mtx[3][3] + m2.mtx[3][3]
			);
}

inline Matrix4D operator-(const Matrix4D& m1, const Matrix4D& m2)
{
	return Matrix4D(
		m1.mtx[0][0] - m2.mtx[0][0],
		m1.mtx[0][1] - m2.mtx[0][1],
		m1.mtx[0][2] - m2.mtx[0][2],
		m1.mtx[0][3] - m2.mtx[0][3],

		m1.mtx[1][0] - m2.mtx[1][0],
		m1.mtx[1][1] - m2.mtx[1][1],
		m1.mtx[1][2] - m2.mtx[1][2],
		m1.mtx[1][3] - m2.mtx[1][3],

		m1.mtx[2][0] - m2.mtx[2][0],
		m1.mtx[2][1] - m2.mtx[2][1],
		m1.mtx[2][2] - m2.mtx[2][2],
		m1.mtx[2][3] - m2.mtx[2][3],

		m1.mtx[3][0] - m2.mtx[3][0],
		m1.mtx[3][1] - m2.mtx[3][1],
		m1.mtx[3][2] - m2.mtx[3][2],
		m1.mtx[3][3] - m2.mtx[3][3]
		);
}

inline Matrix4D operator*(const Matrix4D& m1, const Matrix4D& m2)
{
	Matrix4D ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				ret.mtx[i][j] += m1.mtx[i][k] * m2.mtx[k][j];
			}
		}
	}
	return ret;
}

inline Vector4D operator*(const Matrix4D& m, const Vector4D& p)
{
	return Vector4D(
		p.x * m.mtx[0][0] + p.y * m.mtx[1][0] + p.z * m.mtx[2][0] + p.w * m.mtx[3][0],
		p.x * m.mtx[0][1] + p.y * m.mtx[1][1] + p.z * m.mtx[2][1] + p.w * m.mtx[3][1],
		p.x * m.mtx[0][2] + p.y * m.mtx[1][2] + p.z * m.mtx[2][2] + p.w * m.mtx[3][2],
		p.x * m.mtx[0][3] + p.y * m.mtx[1][3] + p.z * m.mtx[2][3] + p.w * m.mtx[3][3]
		);
}

inline bool operator==(const Matrix4D& m1, const Matrix4D& m2)
{
	return m1.mtx[0][0] == m2.mtx[0][0]
		&& m1.mtx[0][1] == m2.mtx[0][1]
		&& m1.mtx[0][2] == m2.mtx[0][2]
		&& m1.mtx[0][3] == m2.mtx[0][3]
		&& m1.mtx[1][0] == m2.mtx[1][0]
		&& m1.mtx[1][1] == m2.mtx[1][1]
		&& m1.mtx[1][2] == m2.mtx[1][2]
		&& m1.mtx[1][3] == m2.mtx[1][3]
		&& m1.mtx[2][0] == m2.mtx[2][0]
		&& m1.mtx[2][1] == m2.mtx[2][1]
		&& m1.mtx[2][2] == m2.mtx[2][2]
		&& m1.mtx[2][3] == m2.mtx[2][3]
		&& m1.mtx[3][0] == m2.mtx[3][0]
		&& m1.mtx[3][1] == m2.mtx[3][1]
		&& m1.mtx[3][2] == m2.mtx[3][2]
		&& m1.mtx[3][3] == m2.mtx[3][3];
}

inline bool operator!=(const Matrix4D& m1, const Matrix4D& m2)
{
	return m1.mtx[0][0] != m2.mtx[0][0]
		|| m1.mtx[0][1] != m2.mtx[0][1]
		|| m1.mtx[0][2] != m2.mtx[0][2]
		|| m1.mtx[0][3] != m2.mtx[0][3]
		|| m1.mtx[1][0] != m2.mtx[1][0]
		|| m1.mtx[1][1] != m2.mtx[1][1]
		|| m1.mtx[1][2] != m2.mtx[1][2]
		|| m1.mtx[1][3] != m2.mtx[1][3]
		|| m1.mtx[2][0] != m2.mtx[2][0]
		|| m1.mtx[2][1] != m2.mtx[2][1]
		|| m1.mtx[2][2] != m2.mtx[2][2]
		|| m1.mtx[2][3] != m2.mtx[2][3]
		|| m1.mtx[3][0] != m2.mtx[3][0]
		|| m1.mtx[3][1] != m2.mtx[3][1]
		|| m1.mtx[3][2] != m2.mtx[3][2]
		|| m1.mtx[3][3] != m2.mtx[3][3];
}

inline const Matrix4D& Matrix4D::operator = (const Matrix4D &mat)
{
	memcpy(mtx, mat.mtx, sizeof(mtx));
	return *this;
}

inline void Matrix4D::zero()
{
	memset(mtx, 0, sizeof(mtx));
}

inline void Matrix4D::setIdentity()
{
	mtx[0][0] = mtx[1][1] = mtx[2][2] = mtx[3][3] = 1.0;

	mtx[0][1] = mtx[0][2] = mtx[0][3] =
	mtx[1][0] = mtx[1][2] = mtx[1][3] =
	mtx[2][0] = mtx[2][1] = mtx[2][3] =
	mtx[3][0] = mtx[3][1] = mtx[3][2] = 0.0;

	/**this = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};*/
}

inline void Matrix4D::printInfo()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << mtx[i][j] << "\t";
		}
		cout << endl;
	}
}
inline Float Matrix4D::Determinant() const
{
	Float det = 0;
	for (int i = 0; i < 4; i++)
	{
		det += mtx[0][i] * mtx[1][(i + 1) % 4] * mtx[2][(i + 2) % 4] * mtx[3][(i + 3) % 4];
		det -= mtx[3][i] * mtx[2][(i + 1) % 4] * mtx[1][(i + 2) % 4] * mtx[0][(i + 3) % 4];
	}
	return det;
}
inline Float Matrix4D::Minor(int x, int y) const
{
	Matrix3D tmpM;
	for (int i = 1; i < 4; i++)
	{
		for (int j = 1; j < 4; j++)
		{
			tmpM.mtx[i - 1][j - 1] = mtx[(x + i) % 4][(y + j) % 4];
		}
	}
	//tmpM.Determinant();
	return tmpM.determinant();
}
inline Float Matrix4D::Cofactor(int x, int y) const
{
	if ((x + y) % 2 == 0)
	{
		return Minor(x, y);
	}
	else
	{
		return -Minor(x, y);
	}
}

inline Matrix4D Matrix4D::Identity()
{
	return Matrix4D(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

inline Matrix4D Matrix4D::Transpose() const
{
	return Matrix4D(mtx[0][0], mtx[1][0], mtx[2][0], mtx[3][0],
		mtx[0][1], mtx[1][1], mtx[2][1], mtx[3][1],
		mtx[0][2], mtx[1][2], mtx[2][2], mtx[3][2],
		mtx[0][3], mtx[1][3], mtx[2][3], mtx[3][3]);
	Matrix4D buffer;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			buffer.mtx[i][j] = mtx[j][i];
		}
	}
	return buffer;
}

inline Matrix4D Matrix4D::Adjoint() const
{
	Matrix4D ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.mtx[i][j] = Cofactor(j, i);
		}
	}
	//buffer.det = 1.0 / det;
	return ret;
}

inline Matrix4D Matrix4D::Inverse() const
{
	Matrix4D ret, adjM;
	Float det = this->Determinant();
	if (det == 0)
	{
		cout << "The matrix is non-inversable!" << endl;
		return Matrix4D();
	}
	adjM = Adjoint();

	Float invDet = 1.0 / det;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.mtx[i][j] = adjM.mtx[i][j] * invDet;
		}
	}

	return ret;
}

inline Matrix4D Matrix4D::LookAt(const Point3D& pos, const Point3D& target, const Vector3D& up)
{
	//Camera to World
	Vector3D nz = target - pos;
	// distance between target and camera position is too small
	if (isFuzzyNull(nz.x) && isFuzzyNull(nz.y) && isFuzzyNull(nz.z))
	{
		return Matrix4D();
	}

	nz.normalize();
#ifdef RIGHT_HAND_ORDER // OpenGL style
	Vector3D nx = Normalize(Cross(up, nz));
	Vector3D ny = Cross(nz, nx);
	return Matrix4D(
		nx.x, nx.y, nx.z, 0.0,
		ny.x, ny.y, ny.z, 0.0,
		-nz.x, -nz.y, -nz.z, 0.0,
		pos.x, pos.y, pos.z, 1.0
	);
#else // DirectX style
	nz = -nz;
	Vector3D nx = Normalize(Cross(nz, up));//left dir
	Vector3D ny = Cross(nx, nz);
	return Matrix4D(
		nx.x, nx.y, nx.z, 0.0,
		ny.x, ny.y, ny.z, 0.0,
		nz.x, nz.y, nz.z, 0.0,
		pos.x, pos.y, pos.z, 1.0
	);
#endif
}

inline Matrix4D Matrix4D::Perspective(Float verticalAngle, Float aspectRatio, Float nearPlane, Float farPlane)
{

	Float radAngle = DegreeToRadian(verticalAngle * 0.5);
	Float sy = std::sin(DegreeToRadian(verticalAngle * 0.5));
	if (sy == 0)
	{
		return Matrix4D();
	}
	sy = std::cos(radAngle) / sy;
//	Float sx = -sy / aspectRatio;
//	Float sz = (farPlane + nearPlane) / (farPlane - nearPlane);
//	Float pz = 2.0 * farPlane * nearPlane / (nearPlane - farPlane);
	
	Float sx = sy / aspectRatio;
	Float clip = nearPlane - farPlane;
#ifdef RIGHT_HAND_ORDER // OpenGL style
	Float sz = (farPlane + nearPlane) / clip;
	Float pz = 2.0 * farPlane * nearPlane / clip;
	Float w = -1.0;
#else // DirectX style
	Float sz = -(farPlane + nearPlane) / clip;
	Float pz = 2.0 * farPlane * nearPlane / clip;
	Float w = 1.0;
#endif

	return Matrix4D(
		sx, 0.0, 0.0, 0.0,
		0.0, sy, 0.0, 0.0,
		0.0, 0.0, sz, w,
		0.0, 0.0, pz, 0.0
		);

}

inline Matrix4D Matrix4D::Orthography(Float lf, Float rt, Float bt, Float tp, Float nr, Float fr)
{
	return Matrix4D(
		2.0 / (lf - rt), 0.0, 0.0, 0.0,
		0.0, 2.0 / (tp - bt), 0.0, 0.0,
		0.0, 0.0, 2.0 / (fr - nr), 0.0,
		(rt + lf) / (rt - lf), (tp + bt) / (tp - bt), (nr + fr) / (fr - nr), 1.0
		);
}

/*
template <typename vbo_t>
void exportVBO(const Matrix4D &mat, vbo_t *vtx_array)*/
template <typename vbo_t>
void Matrix4D::exportVBO(vbo_t *vtx_array) const
{
	if (sizeof(Float) == sizeof(vbo_t))
	{
		memcpy(vtx_array, this->mtx[0], sizeof(this->mtx));
	}
	else
	{
		for (int i = 0; i < 16; i++)
		{
			vtx_array[i] = static_cast<vbo_t>(this->mtx[0][i]);
		}
	}
}

inline Matrix4D Matrix4D::Translate(const Vector3D& vec)
{
	return Translate(vec.x, vec.y, vec.z);
}

inline Matrix4D Matrix4D::Translate(Float tx, Float ty, Float tz)
{
	return Matrix4D(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		tx, ty, tz, 1.0
		);
}

inline Matrix4D Matrix4D::RotateX(Float theta)
{
	Matrix4D ret;

	theta = DegreeToRadian(theta);
	Float costh = cos(theta);
	Float sinth = sin(theta);

	ret.mtx[0][0] = ret.mtx[3][3] = 1.0;
	ret.mtx[1][1] = ret.mtx[2][2] = costh;
	ret.mtx[1][2] = sinth;
	ret.mtx[2][1] = -sinth;

	return ret;
}

inline Matrix4D Matrix4D::RotateY(Float theta)
{
	Matrix4D ret;

	theta = DegreeToRadian(theta);
	Float costh = cos(theta);
	Float sinth = sin(theta);

	ret.mtx[1][1] = ret.mtx[3][3] = 1.0;
	ret.mtx[0][0] = ret.mtx[2][2] = costh;
	ret.mtx[2][0] = sinth;
	ret.mtx[0][2] = -sinth;

	return ret;
}

inline Matrix4D Matrix4D::RotateZ(Float theta)
{
	Matrix4D ret;

	theta = DegreeToRadian(theta);
	Float costh = cos(theta);
	Float sinth = sin(theta);

	ret.mtx[2][2] = ret.mtx[3][3] = 1.0;
	ret.mtx[0][0] = ret.mtx[1][1] = costh;
	ret.mtx[0][1] = sinth;
	ret.mtx[1][0] = -sinth;

	return ret;
}

inline Matrix4D Matrix4D::Rotate(Float alpha, Float beta, Float gamma)
{
	Matrix4D ret;
	alpha = DegreeToRadian(alpha);
	beta = DegreeToRadian(beta);
	gamma = DegreeToRadian(gamma);
	Float cosA = cos(alpha), sinA = sin(alpha);
	Float cosB = cos(beta), sinB = sin(beta);
	Float cosG = cos(gamma), sinG = sin(gamma);

	ret.mtx[0][0] = cosA * cosG - cosB * sinA * sinG;
	ret.mtx[0][1] = -cosB * cosG * sinA - cosA * sinG;
	ret.mtx[0][2] = sinA * sinB;
	ret.mtx[1][0] = cosG * sinA + cosA * cosB * sinG;
	ret.mtx[1][1] = cosA * cosB * cosG - sinA * sinG;
	ret.mtx[1][2] = -cosA * sinB;
	ret.mtx[2][0] = sinB * sinG;
	ret.mtx[2][1] = sinB * cosG;
	ret.mtx[2][2] = cosB;
	ret.mtx[3][3] = 1.0;

	return ret;
}

inline Matrix4D Matrix4D::Rotate(const Vector3D &axis, Float theta, bool isNormalized)
{
	Vector3D u = isNormalized ? axis : Normalize(axis);
	Float rad = DegreeToRadian(theta);
	Float c = cos(rad);
	Float s = sin(rad);
	Float t = 1 - c;

	Matrix4D ret;

	ret.mtx[0][0] = t * sqr(u.x) + c;
	ret.mtx[0][1] = t * u.x * u.y - s * u.x;
	ret.mtx[0][2] = t * u.x * u.z + s * u.y;
	ret.mtx[0][3] = 0.0;

	ret.mtx[1][0] = t * u.x * u.y + s * u.z;
	ret.mtx[1][1] = t * sqr(u.y) + c;
	ret.mtx[1][2] = t * u.y * u.z - s * u.x;
	ret.mtx[1][3] = 0.0;

	ret.mtx[2][0] = t * u.x * u.z - s * u.y;
	ret.mtx[2][1] = t * u.y * u.z + s * u.x;
	ret.mtx[2][2] = t * sqr(u.z) + c;
	ret.mtx[2][3] = 0.0;

	ret.mtx[3][0] = 0.0;
	ret.mtx[3][1] = 0.0;
	ret.mtx[3][2] = 0.0;
	ret.mtx[3][3] = 1.0;
	return ret;
}

inline Matrix4D Matrix4D::Scale(Float sx, Float sy, Float sz)
{
	return Matrix4D(
		sx, 0.0, 0.0, 0.0,
		0.0, sy, 0.0, 0.0,
		0.0, 0.0, sz, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}

inline Matrix4D Matrix4D::Scale(Float scale)
{
	return Matrix4D(
		scale, 0.0, 0.0, 0.0,
		0.0, scale, 0.0, 0.0,
		0.0, 0.0, scale, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
}
/*
inline void Matrix4D::Shear(Vector3D& vec)
{
 	setIdentify();
 	mtx[0][1] = vec.x;
 	mtx[1][0] = vec.y;
 	Determinant();
}
inline void Matrix4D::Reflect(Vector3D& vec)
{
 	// vec is a vector in the direction of the line
 	mtx[0][0] = vec.x * vec.x - vec.y * vec.y;	mtx[0][1] = 2 * vec.x * vec.y;
 	mtx[1][0] = 2 * vec.x * vec.y;	mtx[1][1] = vec.y * vec.y - vec.x * vec.x;
 	mtx[2][2] = 1;
 	Determinant();
}
inline void Matrix4D::Perspective(Vector3D& vPnt)
{
 	setIdentify();
 	mtx[2][0] = 1.0 / vPnt.x; mtx[2][1] = 1.0 / vPnt.y;
}
*/
template void Matrix4D::exportVBO<float>(float *) const;
template void Matrix4D::exportVBO<double>(double *) const;
#endif
