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
#pragma once
#include "../../../yuni.h"
#include <vector>
#include <list>
#include "../../smartptr.h"
#include "vertex.h"


namespace Yuni
{

	/*!
	** \brief An edge is a segment between two vertices
	** \ingroup Gfx
	*/
	class YUNI_DECL Edge final
	{
	public:
		typedef SmartPtr<Edge> Ptr;
		//! Vector of Edges
		typedef std::vector< SmartPtr<Edge> >  Vector;
		//! List of Edges
		typedef std::list< SmartPtr<Edge> >  List;

	public:
		//! \name Constructors and destructors
		//@{
		Edge() : pVertex1(), pVertex2() {}
		Edge(const Vertex& v1, const Vertex& v2) :pVertex1(v1), pVertex2(v2) {}
		~Edge() {}
		//@}


		//! \brief Access to the vertices composing the edge
		const Vertex& vertex1() const { return pVertex1; }
		const Vertex& vertex2() const { return pVertex2; }


		/*!
		** \brief Comparison operator (equal with)
		**
		** \param other The other edge to compare with
		** \return True if the two edges share the same points
		*/
		bool operator == (const Edge& other) const
		{
			return (pVertex1 == other.vertex1() && pVertex2 == other.vertex2())
				|| (pVertex1 == other.vertex2() && pVertex2 == other.vertex1());
		}

		/*!
		** \brief Comparison operator (non equal with)
		**
		** \param other The other edge to compare with
		** \return True if the two edges do not share the same points
		*/
		bool operator != (const Edge& other) const
		{ return !(*this == other); }


	private:
		//! The first vertex
		Vertex pVertex1;
		//! The second vertex
		Vertex pVertex2;

	}; // Edge



} // namespace Yuni
