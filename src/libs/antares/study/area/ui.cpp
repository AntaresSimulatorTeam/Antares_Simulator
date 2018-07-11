/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "ui.h"
#include "../../logs.h"
#include <yuni/core/math.h>
#include "../../inifile.h"
#include <sstream>

using namespace Yuni;
using namespace Antares;


#define MIN3(x,y,z)  ((y) <= (z) \
						? ((x) <= (y) ? (x) : (y)) \
						: ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) \
						? ((x) >= (y) ? (x) : (y)) \
						: ((x) >= (z) ? (x) : (z)))

#define APPEND_COLOR_VALUE(V,OUT)  \
		if (V > 9) \
		{ \
			if (V > 99) \
				OUT << (uint) V; \
			else \
				OUT << '0' << (uint) V; \
		} \
		else \
			OUT << "00" << (uint) V






namespace Antares
{
namespace Data
{

	namespace // anonymous
	{

		enum
		{
			// (230,108,44)
			defaultRed   = 230,
			defaultGreen = 108,
			defaultBlue  = 44,
		};

		struct rgb_color final
		{
			unsigned char r, g, b;  // Channel intensities between 0.0 and 1.0
		};

		struct hsv_color final
		{
			unsigned char hue;      // Hue degree between 0.0 and 360.0
			unsigned char sat;      // Saturation between 0.0 (gray) and 1.0
			unsigned char val;      // Value between 0.0 (black) and 1.0
		};


		template<class StringT>
		static inline void Convert(const hsv_color& hsv, StringT& out)
		{
			out.clear();
			APPEND_COLOR_VALUE(hsv.hue, out);
			out += '.';
			APPEND_COLOR_VALUE(hsv.sat, out);
			out += '.';
			APPEND_COLOR_VALUE(hsv.val, out);
		}

		template<class StringT>
		static inline void AreaColorToHSVModel(const AreaUI& area, StringT& out)
		{
			rgb_color rgb;
			rgb.r = (unsigned char) area.color[0];
			rgb.g = (unsigned char) area.color[1];
			rgb.b = (unsigned char) area.color[2];

			hsv_color hsv;
			unsigned char rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
			unsigned char rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

			hsv.val = rgb_max;
			if (0 == hsv.val)
			{
				out = "000.000.000"; // black
				return;
			}

			hsv.sat = 255 * (rgb_max - rgb_min) / hsv.val;
			if (hsv.sat == 0)
			{
				hsv.hue = 0;
				Convert(hsv, out);
				return;
			}

			// Compute hue
			if (rgb_max == rgb.r)
			{
				hsv.hue = 0 + 43 * (rgb.g - rgb.b) / (rgb_max - rgb_min);
			}
			else
			{
				if (rgb_max == rgb.g)
					hsv.hue = 85  + 43 * (rgb.b - rgb.r) / (rgb_max - rgb_min);
				else // rgb_max == rgb.b */
					hsv.hue = 171 + 43 * (rgb.r - rgb.g) / (rgb_max - rgb_min);
			}
			Convert(hsv, out);
		}




		template<class StringT>
		static inline  bool AreaUIInternalLoadProperty(AreaUI& ui, const StringT& key, const StringT& value)
		{
			// Coordinates
			if (key.size() == 1)
			{
				if (key == "x")
					return value.template to<int>(ui.x);
				if (key == "y")
					return value.template to<int>(ui.y);
			}
			else
			{
				// Color
				if (key == "color_r")
					return value.template to<int>(ui.color[0]);
				if (key == "color_g")
					return value.template to<int>(ui.color[1]);
				if (key == "color_b")
					return value.template to<int>(ui.color[2]);
				if (key == "layers")
				{
					std::stringstream stringStream(value.template to<std::string>());
					while (1) {
						size_t n;
						stringStream >> n;
						if(n != 0)
							ui.mapLayersVisibilityList.push_back(n);
						if (stringStream.eof())
							break;
					}
					return true;
				}
			}
			// Error
			return false;
		}

	} // anonymous namespace






	AreaUI::AreaUI() :
		x(0),
		y(0),
		pModified(true)
	{
		color[0] = defaultRed;
		color[1] = defaultGreen;
		color[2] = defaultBlue;
		mapLayersVisibilityList.clear();
		mapLayersVisibilityList.push_back(0);
	}


	void AreaUI::reset()
	{
		x = 0;
		y = 0;
		color[0] = defaultRed;
		color[1] = defaultGreen;
		color[2] = defaultBlue;
		pModified = true;
		mapLayersVisibilityList.clear();
		mapLayersVisibilityList.push_back(0);
	}


	void AreaUI::rebuildCache()
	{
		AreaColorToHSVModel(*this, cacheColorHSV);
	}


	bool AreaUI::loadFromFile(const AnyString& filename)
	{
		IniFile ini;
		if (ini.open(filename))
		{
			// The section
			auto* section = ini.find("ui");
			if (section)
			{
				CString<50, false> key;
				CString<50, false> value;

				for (auto* p = section->firstProperty; p; p = p->next)
				{
					// We convert the key and the value into the lower case format,
					// since several tests will be done with these string */
					key = p->key;
					value = p->value;
					key.toLower();
					value.toLower();

					// Property
					if (!AreaUIInternalLoadProperty(*this, key, value))
						logs.warning() << ": `" << key << "`: Unknown property";
				}

				// Bound checking - limits 0..255
				for (uint i = 0; i < 3; ++i)
					color[i] = Math::MinMax<int>(color[i], 0, 255);

				pModified = false;
			
			}
			else
			{
				pModified = true;
				logs.warning() << ": The section `ui` can not be found";
				return false;
			}

			section = ini.find("layerX");
			if (section)
			{
				CString<50, false> key;
				CString<50, false> value;

				for (auto* p = section->firstProperty; p; p = p->next)
				{
					key = p->key;
					value = p->value;
					layerX[key.to<size_t>()] = value.to<int>();
				}
			}

			section = ini.find("layerY");
			if (section)
			{
				CString<50, false> key;
				CString<50, false> value;

				for (auto* p = section->firstProperty; p; p = p->next)
				{
					key = p->key;
					value = p->value;
					layerY[key.to<size_t>()] = value.to<int>();
				}
			}

			section = ini.find("layerColor");
			if (section)
			{
				CString<50, false> key;
				CString<50, false> value;

				for (auto* p = section->firstProperty; p; p = p->next)
				{
					key = p->key;
					value = p->value;
					std::istringstream colorAsStream(value.to<std::string>());
					char item[50];
					int i = 0;
					while (colorAsStream.getline(item,50,','))
					{
						layerColor[key.to<size_t>()][i++] = atoi(item);
					}
				}
			}
			return true;
		}

		// still modified - not saved
		pModified = true;
		logs.warning() << ": The section `ui` can not be found";
		return false;
	}



	bool AreaUI::saveToFile(const AnyString& filename, bool force) const
	{
		if (pModified || force)
		{
			IO::File::Stream file;
			if (file.openRW(filename))
			{
				CString<256,true> data;
				data << "[ui]\nx = "
					<< x << "\ny = " << y
					<< "\ncolor_r = " << color[0]
					<< "\ncolor_g = " << color[1]
					<< "\ncolor_b = " << color[2]
					<< "\nlayers =";
				for (size_t i = 0, size = mapLayersVisibilityList.size(); i < size; i++ )
					data << " " <<mapLayersVisibilityList[i];
				data << '\n';
				data << "[layerX]\n";
				for (std::map<size_t, int>::const_iterator iterator = layerX.begin(); iterator != layerX.end(); iterator++)
				{
					data << iterator->first << " = " << iterator->second;
					data << '\n';
				}
				data << '\n';
				data << "[layerY]\n";
				for (std::map<size_t, int>::const_iterator iterator = layerY.begin(); iterator != layerY.end(); iterator++)
				{
					data << iterator->first << " = " << iterator->second;
					data << '\n';
				}
				data << '\n';
				data << "[layerColor]\n";
				for (std::map<size_t, int[3]>::const_iterator iterator = layerColor.begin(); iterator != layerColor.end(); iterator++)
				{
					data << iterator->first << " = " << iterator->second[0] << " , " << iterator->second[1] << " , " << iterator->second[2];
					data << '\n';
				}
				file << data;

				pModified = false;
				return true;
			}
			logs.error() << "I/O error: impossible to write " << filename;
			return false;
		}
		return true;
	}





} // namespace Data
} // namespace Antares
