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
#ifndef __YUNI_UI_VIEW_H__
# define __YUNI_UI_VIEW_H__

# include "../../yuni.h"
# include "../../core/dictionary.h"
# include "../../core/math.h"
# include "../../core/smartptr.h"
# include "../../uuid/uuid.h"
# include "../control/control.h"
# include "../eventpropagation.h"
# include "drawingsurface.h"
# include "shaderprogram.h"
# include "../textoverlay.h"
# include "../pictureoverlay.h"
# include <list>


namespace Yuni
{
namespace UI
{


	//! Forward declarations (for friend)
	class RenderWindow;
	class GLWindow;


	/*!
	** \brief A view is a rectangle where 3D data is rendered inside a window
	**
	** View position and size is given in pixels
	** There can be several views in a window, overlapping or not.
	** The main usage is split-screen games.
	**
	** For overlapping, views have a Z-order.
	** Higher Z-order means closer to the viewer, 0 is furthest, 255 is closest.
	** Default Z-order is 127 (at mid-way).
	**
	** \warning Overlapping is not recommended for the moment, because the implementation is sub-efficient.
	*/
	class View
	{
	public:
		//! Smart pointer
		typedef SmartPtr<View>  Ptr;
		//! Linked list
		typedef std::list<Ptr>  List;

	public:
		//! Constructor
		View(float x, float y, float w, float h, uint8 z = 127, bool visible = true);

		//! Destructor
		virtual ~View();

		//! Identifier of the view
		const UUID& id() const { return pID; }

		//! X position
		float x() const { return pX; }

		//! Y position
		float y() const { return pY; }

		//! Z order position, 0 is bottom / back, 255 is top / front
		uint8 z() const { return pZ; }
		void z(uint8 z) { pZ = z; }

		//! Width of the view
		float width() const { return pWidth; }

		//! Height of the view
		float height() const { return pHeight; }

		//! Move the view to an absolute position
		void moveTo(float x, float y) { pX = x; pY = y; }
		//! Move the view by a relative amount
		void moveBy(float x, float y) { pX += x; pY += y; }

		//! Resize the view
		void resize(float width, float height)
		{
			assert(height > 0 && "Resizing view to a null or negative height !");
			pWidth = width;
			pHeight = Math::Max(height, 1.0f);
		}

		//! Get the visibility of the view
		bool visible() const { return pVisible; }
		//! Set the visibility of the view
		void show() { pVisible = true; }
		void hide() { pVisible = false; }
		void show(bool isVisible) { pVisible = isVisible; }

		//! Overlay management
		void addOverlay(const TextOverlay::Ptr& text) { pTexts.push_back(text); }
		void addOverlay(const PictureOverlay::Ptr& picture) { pPictures.push_back(picture); }
		void removeOverlay(const TextOverlay::Ptr& text)
		{
			pTexts.erase(std::remove_if(pTexts.begin(), pTexts.end(),
				[&text](const TextOverlay::Ptr& removeText) -> bool
				{
					return removeText == text;
				}), pTexts.end());
		}
		void clearOverlays() { pTexts.clear(); pPictures.clear(); }

		//! Add a UI control to the view
		void rootControl(const IControl::Ptr& control)
		{
			if (pControl != control)
			{
				pEnteredControls.clear();
				pControl = control;
			}
		}
		//! Get the UI controls
		const IControl::Ptr& rootControl() const { return pControl; }
		IControl::Ptr& rootControl() { return pControl; }

		//! Get the top-most control at given coordinates. (Mainly useful for clicking)
		IControl* getControlAt(float x, float y);

		/*!
		** \brief Draw the view
		**
		** \note If you see nothing in this view, make sure the camera is set and the view is visible
		*/
		virtual void draw(uint msMultiplier = 1) const;

	protected:
		EventPropagation doMouseMove(int x, int y);
		virtual EventPropagation doMouseDown(Input::IMouse::Button btn, int x, int y);
		EventPropagation doMouseUp(Input::IMouse::Button btn, int x, int y);
		EventPropagation doMouseDblClick(Input::IMouse::Button btn, int x, int y);
		EventPropagation doMouseScroll(float delta, int x, int y);
		EventPropagation doMouseHover(int x, int y);
		EventPropagation doMouseLeave();

		EventPropagation doKeyDown(Input::Key key, int x, int y);
		EventPropagation doKeyUp(Input::Key key, int x, int y);
		EventPropagation doCharInput(const AnyString& str, int x, int y);

		//! Draw a text overlay
		void drawOverlay(TextOverlay& text) const;

		//! Draw a picture overlay
		void drawOverlay(const PictureOverlay& text) const;

		//! Draw a texture at the given coordinates (in pixels)
		void drawPicture(const Gfx3D::Texture::Ptr& texture, float x, float y, float width,
			float height, bool flip = false, bool invert = false) const;

		//! Draw all 2D elements : overlays and UI
		void draw2D() const;

		//! Necessary shaders for overlay
		bool initShaders();

	protected:
		//! ID of the view
		UUID pID;

		//! X position in pixels (relative to the window)
		float pX;
		//! Y position in pixels (relative to the window)
		float pY;

		//! Width of the view in pixels
		float pWidth;

		//! Height of the view in pixels
		float pHeight;

		//! Z-order, the higher the closer to the viewer
		uint8 pZ;

		/*!
		** \brief Is the view visible or hidden ?
		**
		** \note This is a hiding feature and has nothing to do with whether the view is hidden by other views on top of it
		*/
		bool pVisible;

		//! UI Text overlays
		TextOverlay::Vector pTexts;

		//! UI Picture overlays
		PictureOverlay::Vector pPictures;

		//! UI Control root for this view
		IControl::Ptr pControl;

		//! UI Controls that currently under the mouse cursor
		IControl::Set pEnteredControls;

		//! Shaders for text rendering
		mutable Gfx3D::ShaderProgram::Ptr pTextShaders;

		//! Shaders for picture rendering
		mutable Gfx3D::ShaderProgram::Ptr pPictureShaders;

		//! Drawing surface for UI controls
		mutable DrawingSurface::Ptr pUISurface;

		//! Friend declarations
		friend class RenderWindow;
		friend class GLWindow;

	}; // class View



} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_VIEW_H__
