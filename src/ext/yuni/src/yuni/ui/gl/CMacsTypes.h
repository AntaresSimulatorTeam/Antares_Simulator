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
//
//  CMacsTypes.h
//  CMacs
//
//  Created by Robert Widmann on 11/29/14.
//  Copyright (c) 2014 CodaFi. All rights reserved.
//
//  Modified by the Yuni project
//

#include <objc/message.h>
#include <objc/runtime.h>

#ifndef CMACS_CMACSTYPES_H
# define CMACS_CMACSTYPES_H

typedef struct CMPoint
{
	double x;
	double y;
} CMPoint;

typedef struct CMSize
{
	double width;
	double height;
} CMSize;

typedef struct CMRect
{
	CMPoint origin;
	CMSize size;
} CMRect;

typedef struct AppDel
{
	Class isa;
	
	// Will be an NSWindow later.
	id window;
} AppDelegate;

enum
{
	NSBorderlessWindowMask		= 0,
	NSTitledWindowMask			= 1 << 0,
	NSClosableWindowMask		= 1 << 1,
	NSMiniaturizableWindowMask	= 1 << 2,
	NSResizableWindowMask		= 1 << 3,
};

typedef id(*CMacsSimpleMessage)(id, SEL);
typedef void(*CMacsVoidMessage)(id, SEL);
typedef void(*CMacsVoidMessage1)(id, SEL, void *);
typedef id(*CMacsRectMessage1)(id, SEL, CMRect);
typedef id(*CMacsWindowInitMessage)(id, SEL, CMRect, int, int, bool);
typedef id(*CMacsNextEventMessage)(id, SEL, void*, int, bool);

extern CMacsSimpleMessage cmacs_simple_msgSend;
extern CMacsVoidMessage cmacs_void_msgSend;
extern CMacsVoidMessage1 cmacs_void_msgSend1;
extern CMacsRectMessage1 cmacs_rect_msgSend1;
extern CMacsWindowInitMessage cmacs_window_init_msgSend;
extern CMacsNextEventMessage cmacs_next_event_msgSend;

#endif
