/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_GFX3D_VERTEX_H__
# define __YUNI_GFX3D_VERTEX_H__

# include <yuni/yuni.h>
# include <yuni/core/smartptr.h>
# include <yuni/core/point3D.h>
# include <yuni/core/static/assert.h>
# include "vertexpolicies.h"
# include "vertexpadding.h"

namespace Yuni
{
namespace Gfx3D
{


	/*!
	** \brief Generic vertex for vertex buffer usage
	**
	** A vertex has to contain its position, and then may
	** optionally contain normal, color and texture information.
	*/
	template<
		template<class> class NorP = Policy::Normal::With,
		template<class> class ColP = Policy::Color::None,
		template<class> class TexP = Policy::TexCoord::UV,
		class UserP = Policy::User::None
	>
	class Vertex:
		public Yuni::Point3D<float>,
		public NorP<Vertex<NorP, ColP, TexP, UserP> >,
		public ColP<Vertex<NorP, ColP, TexP, UserP> >,
		public TexP<Vertex<NorP, ColP, TexP, UserP> >,
		public UserP,
		public VertexPadding<
			sizeof(Yuni::Point3D<float>) +
			NorP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			ColP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			TexP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			UserP::Size>
	{
	public:
		//! Full type
		typedef Vertex<NorP, ColP, TexP, UserP>  VertexType;
		//! Normal policy
		typedef NorP<VertexType>  NormalPolicy;
		//! Color policy
		typedef ColP<VertexType>  ColorPolicy;
		//! Texture policy
		typedef TexP<VertexType>  TexCoordPolicy;
		//! User data policy
		typedef UserP  UserDataPolicy;
		//! Padding type
		typedef VertexPadding<
			sizeof(Yuni::Point3D<float>) +
			NorP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			ColP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			TexP<Vertex<NorP, ColP, TexP, UserP> >::Size +
			UserP::Size>  PaddingT;
		//! Smart pointer
		typedef SmartPtr<VertexType>  Ptr;

	public:
		enum // anonymous
		{
			//! Size in bytes of data without padding (not particularly useful except for debugging)
			DataSize = (sizeof(Yuni::Point3D<float>) +
				NorP<Vertex<NorP, ColP, TexP, UserP> >::Size +
				ColP<Vertex<NorP, ColP, TexP, UserP> >::Size +
				TexP<Vertex<NorP, ColP, TexP, UserP> >::Size +
				UserP::Size),

			// Features
			//! Does the vertex contain normals data ?
			HasNormal = NormalPolicy::HasNormal,
			//! Does the vertex contain color data ?
			HasColor = ColorPolicy::HasColor,
			//! Does the vertex contain texture data ?
			HasTexture = TexCoordPolicy::HasTexture,

			// Offsets in bytes
			//! Offset of the coordinates data in bytes
			PointOffset = 0,
			//! Offset of the normals data in bytes
			NormalOffset = PointOffset + sizeof(Yuni::Point3D<float>),
			//! Offset of the color data in bytes
			ColorOffset = NormalOffset + NormalPolicy::Size,
			//! Offset of the texture coordinates data in bytes
			TexCoordOffset = ColorOffset + ColorPolicy::Size,
			//! Offset of the user data in bytes
			UserOffset = TexCoordOffset + TexCoordPolicy::Size
		};

		//! For use in ShaderProgram::bindArgument()
		enum Attribute
		{
			//! Coordinates of the vertex in space
			vaPosition = 0,

			//! Vertex normal
			vaNormal = 1,

			//! Vertex color
			vaColor = 2,

			//! Texture coordinates
			vaTextureCoord = 3,

			//! Multi-texturing texture 0
			vaTexture0 = 0,

			//! Multi-texturing texture 1
			vaTexture1 = 1,

			//! Multi-texturing texture 2
			vaTexture2 = 2,

			//! Multi-texturing texture 3
			vaTexture3 = 3,

			//! Multi-texturing texture 4
			vaTexture4 = 4,

			//! Multi-texturing texture 5
			vaTexture5 = 5,

			//! Multi-texturing texture 6
			vaTexture6 = 6,

			//! Multi-texturing texture 7
			vaTexture7 = 7,

			//! User index, all following indices are free, vaUser + 1, vaUser + 2, ...
			vaUser = 12

		}; // enum Attribute


	public:
		//! Empty constructor, default values
		Vertex();

		//! Constructor with coordinates
		template<class U, class V, class W>
		Vertex(U x, V y, W z);

		//! Copy constructor
		Vertex(const Vertex&);

		//! Assignment operator
		Vertex& operator = (const Vertex& v);

		//! Lesser than comparison operator (for std::map::find)
		bool operator < (const Vertex& p) const;

		//! Position of the vertex in space
		Yuni::Point3D<float>& position();
		//! Position of the vertex in space
		const Yuni::Point3D<float>& position() const;

		//! The normal vector at the vertex
		NormalPolicy& normal();
		//! The normal vector at the vertex
		const NormalPolicy& normal() const;

		//! The vertex color
		ColorPolicy& color();
		//! The vertex color
		const ColorPolicy& color() const;

		//! The UV texture coordinates on given vertex
		TexCoordPolicy& texCoord();
		//! The UV texture coordinates on given vertex
		const TexCoordPolicy& texCoord() const;

		//! Optional user data
		UserDataPolicy& user();
		//! Optional user data
		const UserDataPolicy& user() const;

	}; // class Vertex



} // namespace Gfx3D
} // namespace Yuni


# include "vertex.hxx"


#endif // __YUNI_GFX3D_VERTEX_H__
