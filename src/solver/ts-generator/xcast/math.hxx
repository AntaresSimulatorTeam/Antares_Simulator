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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <antares/study.h>
#include <antares/logs.h>
#include "xcast.h"
#include "constants.h"

using namespace Yuni;


namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{


	/*
	** This function is no longer used but remains for code clarity
	**
	float Minimum(float a,float b,float g,float d,int l)
	{
		switch(l)
		{
			case 1: return 0;
			case 2: return 0;
			case 3: return 0;
			case 4: return 0;
			case 5: return 0;
		}
		return 0.f;
	}
	*/


	static float maximum(float a, float b, float g, float d, int l)
	{
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
			case Data::XCast::dtUniform:
			case Data::XCast::dtNormal:
				{
					return d - g;
				}
			case Data::XCast::dtWeibullShapeA:
				{
					// on regle la borne pour que le residu sur la fonction de repartition :
					// y= 1-exp(-(x/b)^a) soit < NEGLI_WEIB
					return (b * (float(pow(log(1. / NEGLI_WEIB), double(1. / a))))) - g;
				}
			case Data::XCast::dtGammaShapeA:
				{
					// on regle la borne pour que la fonction de distribution :
					// y = b*((bx)^a-1)*exp(-bx)/gamma_euler(a) soit < NEGLI_GAMM
					// on regle la borne ou une valeur pessimiste en considerant que la
					// fonction ne diminue pas plus rapidement qu'une exponentielle
					// (alors qu'on impose a >= 1)
					// Since 3.5, B' = 1/B
					return ((float) (log(1. / NEGLI_GAMM) * a * b)) - g;
				}

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return 0.f;
		}
		return 0.f;
	}



	/*!
	** \brief Esperance de la variable de loi L
	**
	** Valeur exprimee en valeur relative par rapport au minimum
	*/
	static float esperance(float a, float b, float g, float d, int l)
	{
		// note : valeur exprimee en valeur relative par rapport au minimum
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
				return (a * (d - g)) / (a + b);

			case Data::XCast::dtUniform:
				return (d - g) / 2.f;

			case Data::XCast::dtNormal:
				return 6.f * b;

			case Data::XCast::dtWeibullShapeA:
				return b * (float)XCast::GammaEuler(1. + 1. / double(a));

			case Data::XCast::dtGammaShapeA:
				return a * b;

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return 0.f;
		}
		return 0.f;
	}



	//! Ecart type de la variable de loi L
	static float standard(float a, float b, float g, float d, int l)
	{
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
				{
					float s = a + b;
					s = s * s * (s + 1.f);
					s = a * b / s;
					s = (float) sqrt(s);
					s = (d - g) * s;
					return s;
				}
			case Data::XCast::dtUniform:
				{
					return (d - g) / (3.464101615f /* float(sqrt(12.)) */);
				}
			case Data::XCast::dtNormal:
				{
					return b;
				}
			case Data::XCast::dtWeibullShapeA:
				{
					const float x =
						(float)(XCast::GammaEuler(1. + 2. / double(a))
							- pow(XCast::GammaEuler(1. + 1. / double(a)), 2.));
					if (x < 0.f)
					{
						logs.warning() << "TS Generator: error when computing the standard deviation (Weibul Shape A)";
						return 0.f;
					}
					return b * sqrt(x);
				}
			case Data::XCast::dtGammaShapeA:
				{
					return (float)(sqrt(a) * b);
				}

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return 0.f;
		}
		return 0.f;
	}


	//! Coefficient de diffusion d'un processus
	static float diffusion(float a, float b, float g, float d, int l, float t, float x)
	{
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
				{
					return (float) sqrt(2. * t * x * ((d - g) - x) / (a + b));
				}
			case Data::XCast::dtUniform:
				{
					return (float) sqrt(t * x * ((d - g) - x));
				}
			case Data::XCast::dtNormal:
				{
					return b * (float)sqrt(2. * t);
				}
			case Data::XCast::dtWeibullShapeA:
				{
					// Warning:
					// On suppose ici que d a ete correctement initialise avec d=gamma_euler(1+1/a)
					//
					assert(!Math::Zero(b) && "division by zero");
					const double v = pow(x / b, a);
					const double w = exp(v);

					double y = (w - 1.) * double(d);
					y  -=  w * XCast::GammaInc(double(1. + 1. / a), v);
					y  /=  v;
					y  *=  x * b * 2. * t / a;
					if (y < 0.)
					{
						logs.info() << "  square diff: " << y << " (Weibul Shape A)";
						y = 1e-6;
					}
					return (float)sqrt(y);
				}
			case Data::XCast::dtGammaShapeA:
				{
					return (float)sqrt(2. * t * x * b);
				}

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return 0.f;
		}
		return 0.f;
	}



	/*
	** \brief Verification de l'acceptabilite d'une factorisation LtL
	**
	** MD 10/09/09
	** Ce code verifie si LtL est proche de A, A symetrique reelle et L triangulaire
	** inferieure qui n'est pas necessairement definie positive.
	**
	** \param L estimation a priori de la racine carrée autoconjuguée de A
	** \param A matrice symétrique NxN
	** \param N Nombre de processus
	**
	** \return True si la factorisation est correcte a la tolerance pres
	*/
	/*static bool proma(float** L, float** A, uint N)
	{
		float estim_aij;
		uint i, j, k;

		for (i = 0; i < N; i++)
		{
			for (j = 0; j <= i; ++j)
			{
				estim_aij = 0.f;
				for (k = 0; k < N; ++k)
					estim_aij += L[i][k] * L[j][k];

				if (Math::Abs(estim_aij - A[i][j]) > EPSIMAX)
					return false;
			}
		}
		return true;
	}*/



	static bool verification(float a, float b, float g, float d, int l, float t)
	{
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
				{
					return !(d < g || g < INFININ / 2.f || d > INFINIP / 2.f || t < 0.f
						|| a < 0.f || b < 0.f);
				}
			case Data::XCast::dtUniform:
				{
					return !(d < g || g < INFININ / 2.f || d > INFINIP / 2.f || t < 0.f);
				}
			case Data::XCast::dtNormal:
				{
					return !(d < g || g < INFININ / 2.f || d > INFINIP / 2.f || a < INFININ / 2.f
						|| a > INFINIP / 2.f || t < 0.f || b < 0.f);
				}
			case Data::XCast::dtWeibullShapeA:
				{
					return !(g < INFININ / 2.f || a < 1.f || a > 50.f || t < 0.f || b <= 0.f);
				}
			case Data::XCast::dtGammaShapeA:
				{
					return !(g < INFININ / 2.f || a < 1.f || a > 50.f || t < 0.f || b <= 0.f);
				}

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return false;
		}
		return false;
	}





	//! Valeur maximale du coefficient de diffusion du processus de loi L
	static float maxiDiffusion(float a, float b, float g, float d, int l, float t)
	{
		switch (Data::XCast::Distribution(l))
		{
			case Data::XCast::dtBeta:
				{
					return (float)((d - g) * sqrt(t / (2. * (a + b))));
				}
			case Data::XCast::dtUniform:
				{
					return (float)((d - g) * sqrt(t) / 2.);
				}
			case Data::XCast::dtNormal:
				{
					return (float)(b * sqrt(2. * t));
				}
			case Data::XCast::dtWeibullShapeA:
				{
					// pour une loi de Weibull, on prend comme reference le max des diffusions obtenues pour :
					// x=  e= gamma_euler(1+1/a)
					// x = Maximum
					// quand a>1, x = b*(((a-1)/a)^(1/a))
					// la valeur de la diffusion

					// necessaire si d n'a pas ete initialise au prealable
					d = float(XCast::GammaEuler(1. + 1. / double(a)));

					float x = b * d;
					float m = diffusion(a, b, g, d, l, t, x);
					float y = diffusion(a, b, g, d, l, t, maximum(a,b,g,d,l));
					if (y > m)
						m = y;
					if (a > 1.f)
					{
						x  = (a - 1.f) / a;
						x  = (float)pow((double)x, (double)(1. / a));
						x *= b;
						y  = diffusion(a, b, g, d, l, t, x);
						if (y > m)
							m = y;
					}
					return m;
				}
			case Data::XCast::dtGammaShapeA:
				{
					return diffusion(a, b, g, d, l, t, maximum(a, b, g, d, l));
				}

			case Data::XCast::dtMax:
			case Data::XCast::dtNone:
				return 0.f;
		}
		return 0.f;
	}





} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares

