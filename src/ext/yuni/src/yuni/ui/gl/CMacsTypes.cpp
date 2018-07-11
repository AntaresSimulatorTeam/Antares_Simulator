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
//  CMacsTypes.c
//  CMacs
//
//  Created by Robert Widmann on 11/29/14.
//  Copyright (c) 2014 CodaFi. All rights reserved.
//

#include "CMacsTypes.h"

CMacsSimpleMessage cmacs_simple_msgSend = (CMacsSimpleMessage)objc_msgSend;
CMacsVoidMessage cmacs_void_msgSend	 = (CMacsVoidMessage)objc_msgSend;
CMacsVoidMessage1 cmacs_void_msgSend1 = (CMacsVoidMessage1)objc_msgSend;
CMacsRectMessage1 cmacs_rect_msgSend1 = (CMacsRectMessage1)objc_msgSend;
CMacsWindowInitMessage cmacs_window_init_msgSend = (CMacsWindowInitMessage)objc_msgSend;
CMacsNextEventMessage cmacs_next_event_msgSend = (CMacsNextEventMessage)objc_msgSend;
