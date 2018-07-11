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
#ifndef __YUNI_UI_LINERENDERER_H__
# define __YUNI_UI_LINERENDERER_H__

# include "../../yuni.h"
# include "../../core/math.h"
# include "vertex.h"



namespace Yuni
{
namespace UI
{

	/*
	** \brief Draw a line
	**
	** this implementation uses vertex array (opengl 1.1)
	**   choose only 1 from vase_rend_draft_1.h and vase_rend_draft_2.h
	**   to your need. if you have no preference, just use vase_rend_draft_2.h
	**
	** this is the master line() function which features:
	**  - premium quality anti-aliased line drawing
	**  - smaller CPU overhead than other CPU rasterizing algorithms
	**  - line thickness control
	**  - line color control
	**  - can choose to use alpha blend or not
	**
	** sample usage using alpha blending:
	**
	** \code
	** glPushAttrib();
	** glEnable(GL_BLEND);
	** glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	** glMatrixMode(GL_PROJECTION);
	** glPushMatrix();
	** glLoadIdentity();
	** glOrtho(0, context_width, context_height, 0, 0.0f, 100.0f);
	**
	** line ( 10,100,100,300,		//coordinates
	**		1.2,			//thickness in px
	**		0.5, 0.0, 1.0, 1.0,	//line color RGBA
	**		0, 0,			//not used
	**		true);			//enable alphablend
	**
	** //other drawing code...
	** glPopMatrix();
	** glPopAttrib(); //restore blending options
	** \endcode
	**
	** and not using alpha blending (blend to background color):
	**
	** \code
	** glMatrixMode(GL_PROJECTION);
	** glPushMatrix();
	** glLoadIdentity();
	** glOrtho(0, context_width, context_height, 0, 0.0f, 100.0f);
	**
	** line ( 20,100,110,300,		//coordinates
	**		1.2,			//thickness in px
	**		0.5, 0.0, 1.0,		//line color *RGB*
	**		1.0, 1.0, 1.0,		//background color
	**		false);			//not using alphablend
	**
	** //other drawing code...
	** glPopMatrix();
	** \endcode
	**
	** \see http://artgrammer.blogspot.com/2011/05/drawing-nearly-perfect-2d-line-segments.html
	** \see http://www.codeproject.com/KB/openGL/gllinedraw.aspx
	*/
	void line(double x1, double y1, double x2, double y2, // coordinates of the line
		float w,                                          // width/thickness of the line in pixel
		float Cr, float Cg, float Cb, float Ca,	          // RGBA color components
		float Br, float Bg, float Bb, float Ba,	          // color of background, Br=alpha of color when alphablend=true
		bool alphablend = false);                         // use alpha blend or not




	/*!
	** \brief A skimmed version of line()
	**
	** no color, no thickness control
	** draws near-perfectly a black "hair line" of thickness 1px
	** when alphablend is false, it assumes drawing on a white surface
	** when alphablend is true, it draws with alpha
	*/
	void hair_line(double x1, double y1, double x2, double y2, bool alphablend = false);


	/*!
	** \brief Fallback implementation for line()
	*/
	void line_raw( double x1, double y1, double x2, double y2,
		double w,
		double Cr, double Cg, double Cb,
		double,double,double, bool);







} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_LINERENDERER_H__
