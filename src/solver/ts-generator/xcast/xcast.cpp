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
#include <antares/study.h>
#include "xcast.h"
#include "predicate.hxx"
#include <antares/logs.h>
#include <antares/emergency.h>
#include <limits>
#include <yuni/io/directory.h>
#include <antares/memory/memory.h>
#include <antares/study/area/constants.h>


using namespace Yuni;


#define SEP (IO:: Separator)


namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{


	
	enum
	{
		alpha    = Data::XCast::dataCoeffAlpha,
		beta     = Data::XCast::dataCoeffBeta,
		gamma    = Data::XCast::dataCoeffGamma,
		delta    = Data::XCast::dataCoeffDelta,
		theta    = Data::XCast::dataCoeffTheta,
		mu       = Data::XCast::dataCoeffMu,
	};



	XCast::XCast(Data::Study& study, Data::TimeSeries ts) :
		study(study),
		timeSeriesType(ts),
		pNeverInitialized(true),
		pAccuracyOnCorrelation(false)
	{}


	XCast::~XCast()
	{
		destroyTemporaryData();
	}


	template<class PredicateT>
	void XCast::exportTimeSeriesToTheOutput(Progression::Task& progression, PredicateT& predicate)
	{
		if (study.parameters.noOutput)
		{
			for (uint i = 0; i != study.areas.size(); ++i)
				++progression;
		}
		else
		{
			logs.info() << "Exporting " << predicate.timeSeriesName() << " time-series into the output (year:"
				<< year << ')';

			String output;
			String filename;
			output.reserve(study.folderOutput.size() + 80 );

			output << study.folderOutput << SEP << "ts-generator" << SEP << predicate.timeSeriesName()
				<< SEP << "mc-" << year;
			filename.reserve(output.size() + 80 );

			if (IO::Directory::Create(output))
			{
				
				study.areas.each([&] (Data::Area& area)
				{
					filename.clear() << output << SEP << area.id << ".txt";
					predicate.matrix(area).saveToCSVFile(filename);

					
					++progression;
				});
			}
			else
				logs.error() << "Impossible to create the directory '" << output << "'";
		}
	}



	template<class PredicateT>
	void XCast::applyTransferFunction(PredicateT& predicate)
	{
		
		
		
		
		

		enum
		{
			
			nbHoursADay = 24,
			
			x = 0,
			
			y = 1,
			
			maxPoints = Data::XCast::conversionMaxPoints,
		};

		uint h, i, j, k;

		float a[maxPoints];
		float b[maxPoints];
		
		float* dailyResults;

		
		const uint processCount = (uint) pData.localareas.size();

		for (uint s = 0; s != processCount; ++s)
		{
			if (pUseConversion[s])
			{
				auto& data = predicate.xcastData(*(pData.localareas[s]));
				
				auto& tf = data.conversion;

				uint last_i = 0;

				
				
				a[tf.width] = 0.f;
				b[tf.width] = 0.f;
				for (i = 0; i != tf.width - 1; ++i)
				{
					auto&  p0 = tf[i];
					auto&  p1 = tf[i + 1];
					if (p1[x] <= p0[x])
					{
						logs.fatal() << "Transfer function: invalid X-coordinate at index ("
							<< i << ", " << (i+1) << ")";
						AntaresSolverEmergencyShutdown();
					}
					a[i] = (p1[y] - p0[y])  /  (p1[x] - p0[x]);
					b[i] = (p0[y] * p1[x] - p1[y] * p0[x])  /  (p1[x] - p0[x]);
				}

				
				dailyResults = DATA[s];
				for (h = 0; h != nbHoursADay; ++h) 
				{
					for (i = 0; i != tf.width;  ++i)
					{
						j = (i + last_i)     % (tf.width);
						k = (i + last_i + 1) % (tf.width);
						auto&  pj = tf.column(j);
						auto&  pk = tf.column(k);

						
						if (dailyResults[h] >= pj[x] && dailyResults[h] <= pk[x])
						{
							assert(0 == Math::Infinite(b[j]) && "Infinite value");
							dailyResults[h] = (a[j] * dailyResults[h]) + b[j];
							last_i = i;
							break;
						}
					}
				}

				
				tf.flush();
			}
		}
	}



	template<class PredicateT>
	void XCast::updateMissingCoefficients(PredicateT& predicate)
	{
		
		

		
		
		for (uint realmonth = 0; realmonth != 12; ++realmonth)
		{
			
			for (uint s = 0; s != pData.localareas.size(); ++s)
			{
				auto& data = predicate.xcastData(*(pData.localareas[s]));

				switch (data.distribution)
				{
					case Data::XCast::dtNormal:
						{
							
							
							
							# ifdef ANTARES_SWAP_SUPPORT
							data.data[gamma][realmonth] = data.data[alpha][realmonth] - 6.f * data.data[beta][realmonth];
							data.data[delta][realmonth] = data.data[alpha][realmonth] + 6.f * data.data[beta][realmonth];
							# else
							float** v = data.data.entry;
							v[gamma][realmonth] = v[alpha][realmonth] - 6.f * v[beta][realmonth];
							v[delta][realmonth] = v[alpha][realmonth] + 6.f * v[beta][realmonth];
							# endif
							break;
						}
					case Data::XCast::dtWeibullShapeA:
						{
							# ifdef ANTARES_SWAP_SUPPORT
							data.data[delta][realmonth] = (float) GammaEuler(1. + 1. / data.data[alpha][realmonth]);
							# else
							float** v = data.data.entry;
							v[delta][realmonth] = (float) GammaEuler(1. + 1. / v[alpha][realmonth]);
							# endif
							break;
						}
					default:
						
						break;
				}

				
				data.data.flush();

			} 
		} 
	}


	namespace 
	{
		template<int DebugT>
		class Allocator
		{
		public:
			Allocator() :
				allocated(0)
			{}
			~Allocator()
			{
				logs.debug() << "  allocated " << (allocated / 1024) << "Ko";
			}
			template<class T> inline T* allocate (const size_t s)
			{
				allocated += sizeof(T) * s;
				return new T[s];
			}

		public:
			size_t allocated;
		};

		template<>
		class Allocator<0>
		{
		public:
			template<class T> inline T* allocate (const size_t s) const
			{
				return new T[s];
			}
		};

	} 




	void XCast::allocateTemporaryData()
	{
		uint p = (uint) pData.localareas.size();

		Allocator<Yuni::Logs::Verbosity::Debug::enabled> m;

		A       = m.allocate<float>(p);
		B       = m.allocate<float>(p);
		G       = m.allocate<float>(p);
		D       = m.allocate<float>(p);
		M       = m.allocate<int>  (p);
		T       = m.allocate<float>(p);
		BO      = m.allocate<bool> (p);
		MA      = m.allocate<float>(p);
		MI      = m.allocate<float>(p);
		L       = m.allocate<Data::XCast::Distribution>(p);
		POSI    = m.allocate<float>(p);
		MAXI    = m.allocate<float>(p);
		MINI    = m.allocate<float>(p);
		ESPE    = m.allocate<float>(p);
		STDE    = m.allocate<float>(p);
		DIFF    = m.allocate<float>(p);
		TREN    = m.allocate<float>(p);
		WIEN    = m.allocate<float>(p + 1);
		BROW    = m.allocate<float>(p);

		BASI	= m.allocate<float>(p);
		ALPH	= m.allocate<float>(p);
		BETA	= m.allocate<float>(p);

		D_COPIE = m.allocate<float>(p);

		pUseConversion = m.allocate<bool>(p);

		Presque_maxi = m.allocate<float>(p);
		Presque_mini = m.allocate<float>(p);
		pQCHOLTotal  = m.allocate<float>(p);

		CORR = m.allocate<float*>(p);
		Triangle_reference = m.allocate<float*>(p);
		Triangle_courant = m.allocate<float*>(p);
		FO = m.allocate<float*>(p);
		LISS = m.allocate<float*>(p);
		DATL = m.allocate<float*>(p);
		DATA = m.allocate<float*>(p);
		Carre_courant = m.allocate<float*>(p);
		Carre_reference = m.allocate<float*>(p);

		for (uint i = 0; i != p; ++i)
		{
			Triangle_reference[i] = m.allocate<float>(p);
			Triangle_courant[i]   = m.allocate<float>(p);
			Carre_courant[i]      = m.allocate<float>(p);
			Carre_reference[i]    = m.allocate<float>(p);

			CORR[i] = m.allocate<float>(p);
			FO[i]   = m.allocate<float>(24);
			LISS[i] = m.allocate<float>(24);
			DATL[i] = m.allocate<float>(24);
			DATA[i] = m.allocate<float>(24);
		}
	}


	void XCast::destroyTemporaryData()
	{
		if (!pNeverInitialized)
		{
			uint p = (uint) pData.localareas.size();
			for (uint i = 0; i != p; ++i)
			{
				delete[] CORR[i];
				delete[] FO[i];
				delete[] LISS[i];
				delete[] DATL[i];
				delete[] Triangle_reference[i];
				delete[] Triangle_courant[i];
				delete[] DATA[i];
				delete[] Carre_courant[i];
				delete[] Carre_reference[i];
			}
			delete[] Carre_courant;
			delete[] Carre_reference;
			delete[] D_COPIE;
			delete[] DATA;
			delete[] Triangle_reference;
			delete[] Triangle_courant;
			delete[] LISS;
			delete[] DATL;
			delete[] CORR;
			delete[] FO;
			delete[] A;
			delete[] B;
			delete[] G;
			delete[] D;
			delete[] M;
			delete[] T;
			delete[] L;
			delete[] BO;
			delete[] MA;
			delete[] MI;
			delete[] POSI;
			delete[] MAXI;
			delete[] MINI;
			delete[] Presque_maxi;
			delete[] Presque_mini;
			delete[] ESPE;
			delete[] STDE;
			delete[] DIFF;
			delete[] TREN;
			delete[] WIEN;
			delete[] BROW;
			delete[] pQCHOLTotal;
			delete[] pUseConversion;
		}
	}


	template<class PredicateT>
	bool XCast::runWithPredicate(PredicateT& predicate, Progression::Task& progression)
	{
		enum
		{
			
			nbHoursADay = 24,
		};

		pTSName = predicate.timeSeriesName();

		
		{
			logs.info(); 
			logs.info() << "Generating the " << predicate.timeSeriesName() << " time-series";
		}
		for (uint s = 0; s != pData.localareas.size(); ++s)
		{
			if (not predicate.preproDataIsReader(*pData.localareas[s]))
			{
				logs.warning() << "The timeseries will not be regenerated. All data related to the ts-generator for "
					<< "'" << predicate.timeSeriesName() << "' have been released.";
				return false;
			}
		}

		if (pNeverInitialized)
		{
			
			const uint nbHours = 8760;
			
			loadFromStudy(predicate.correlation(study), predicate);
			
			allocateTemporaryData();

			for (uint s = 0; s != pData.localareas.size(); ++s)
			{
				
				auto& area = *(pData.localareas[s]);

				
				predicate.matrix(area).resize(nbTimeseries, nbHours);
				predicate.matrix(area).flush();
				auto& xcast = predicate.xcastData(area);
				
				pUseConversion[s] = (xcast.useConversion && xcast.conversion.width >= 3);
			}

			pAccuracyOnCorrelation = ((study.parameters.timeSeriesAccuracyOnCorrelation & timeSeriesType) != 0);
		}

		
		const uint processCount = (uint) pData.localareas.size();

		
		
		
		if (study.areas.size() > pData.localareas.size())
			progression += (nbTimeseries * 365) * ((uint) study.areas.size() - (uint) pData.localareas.size());

		if (processCount == 0)
		{
			
			if (study.parameters.timeSeriesToArchive & timeSeriesType)
				exportTimeSeriesToTheOutput(progression, predicate);
			return true;
		}

		
		
		updateMissingCoefficients(predicate);

		
		pComputedPointCount = 0;
		pNDPMatrixCount     = 0;
		pLevellingCount     = 0;


		for (uint tsIndex = 0; tsIndex != nbTimeseries; ++tsIndex)
		{
			
			uint hourInTheYear = 0;

			
			
			
			for (uint month = 0; month != 12; ++month)
			{
				
				uint realmonth = study.calendar.months[month].realmonth;

				
				pNewMonth = true;
				
				pCorrMonth = pData.correlation[realmonth];

				
				for (uint s = 0; s != processCount; ++s)
				{
					assert(s < pData.localareas.size() && "Bound checking");
					auto& xcastdata = predicate.xcastData(*(pData.localareas[s]));
					A[s] = xcastdata.data       [alpha] [realmonth];
					B[s] = xcastdata.data       [beta]  [realmonth];
					G[s] = xcastdata.data       [gamma] [realmonth];
					D[s] = xcastdata.data       [delta] [realmonth];
					M[s] = (int) xcastdata.data [mu]    [realmonth];
					T[s] = xcastdata.data       [theta] [realmonth];
					L[s] = xcastdata.distribution;

					switch (xcastdata.distribution)
					{
						case Data::XCast::dtUniform:
							{
								BO[s] = true;
								MI[s] = G[s];
								MA[s] = D[s];
								break;
							}
						case Data::XCast::dtBeta:
							{
								BO[s] = true;
								MI[s] = G[s];
								MA[s] = D[s];
								break;
							}
						case Data::XCast::dtNormal:
							{
								BO[s] = false;
								MI[s] = - std::numeric_limits<float>::max();
								MA[s] = + std::numeric_limits<float>::max();
								break;
							}
						default:
							{
								BO[s] = false;
								MI[s] = G[s];
								MA[s] = + std::numeric_limits<float>::max();
							}
					}
					memcpy(FO[s], xcastdata.K[realmonth], sizeof(float) * nbHoursADay);

					
					xcastdata.data.flush();
					xcastdata.K.flush();
				}

				uint nbDaysPerMonth = study.calendar.months[month].days;
				for (uint j = 0; j != nbDaysPerMonth; ++j)
				{
					if (not generateValuesForTheCurrentDay())
						AntaresSolverEmergencyShutdown(); 

					# ifndef NDEBUG
					
					for (uint s = 0; s != processCount; ++s)
					{
						float* dailyResults  = DATA[s];
						
						for (uint h = 0; h != nbHoursADay; ++h)
						{
							assert(0 == Math::Infinite(dailyResults[h]) && "Infinite value");
						}
					}
					# endif


					
					for (uint s = 0; s != processCount; ++s)
					{
						assert(s < pData.localareas.size() && "Bound checking");
						auto& currentArea = *pData.localareas[s];
						
						auto& srcData = predicate.xcastData(currentArea);
						if (srcData.useTranslation != Data::XCast::tsTranslationBeforeConversion)
							continue;

						
						auto& column = srcData.translation[0];
						float* dailyResults  = DATA[s];
						assert(hourInTheYear + nbHoursADay <= srcData.translation.height && "Bound checking");

						
						for (uint h = 0; h != nbHoursADay; ++h)
						{
							assert(0 == Math::Infinite(dailyResults[h]) && "Infinite value");
							dailyResults[h] += (float) column[hourInTheYear + h];
						}

						
						srcData.translation.flush();
					}

					
					applyTransferFunction(predicate);

					# ifndef NDEBUG
					
					for (uint s = 0; s != processCount; ++s)
					{
						float* dailyResults  = DATA[s];
						
						for (uint h = 0; h != nbHoursADay; ++h)
						{
							assert(0 == Math::Infinite(dailyResults[h]) && "Infinite value");
						}
					}
					# endif

					for (uint s = 0; s != processCount; ++s)
					{
						assert(s < pData.localareas.size() && "Bound checking");
						auto& currentArea = *pData.localareas[s];
						
						auto& srcData = predicate.xcastData(currentArea);

						
						auto& series = predicate.matrix(currentArea);
						assert(tsIndex < series.width);
						auto& column = series.column(tsIndex);
						float* dailyResults = DATA[s];

						
						for (uint h = 0; h != nbHoursADay; ++h)
						{
							assert(0 == Math::Infinite(dailyResults[h]) && "Infinite value");
							dailyResults[h] *= (float)srcData.capacity;
						}

						
						if (srcData.useTranslation == Data::XCast::tsTranslationAfterConversion)
						{
							assert(hourInTheYear + nbHoursADay <= srcData.translation.height && "Bound checking");
							auto& tsavg = srcData.translation[0];
							for (uint h = 0; h != nbHoursADay; ++h)
								dailyResults[h] += (float) tsavg[hourInTheYear + h];
						}

						
						assert(hourInTheYear + nbHoursADay <= series.height && "Bound checking");
						for (uint h = 0; h != nbHoursADay; ++h)
							column[hourInTheYear + h] = Math::Round(dailyResults[h]);

						
						series.flush();
						srcData.translation.flush();

						
						++progression;
					}

					hourInTheYear += nbHoursADay;

					
					pNewMonth = false;
				}
			}

		} 



		
		{
			uint y = ((pAccuracyOnCorrelation) ? pComputedPointCount : (nbTimeseries * 365));
			uint z = pNDPMatrixCount;

			logs.info() << "  "
				<< pComputedPointCount << " points calculated, using "
				<< y << " correlation matrices, out of which "
				<< z << " were npsd and transformed";
		}

		if (study.parameters.derated)
		{
			study.areas.each([&] (Data::Area& area)
			{
				predicate.matrix(area).averageTimeseries();
				predicate.matrix(area).flush();
			});
		}

		
		if (study.parameters.timeSeriesToArchive & timeSeriesType)
			exportTimeSeriesToTheOutput(progression, predicate);

		
		if (timeSeriesType == Data::timeSeriesLoad)
		{
			logs.debug() << "  [ts-generator] adding DSM";
			for (uint s = 0; s != study.areas.size(); ++s)
			{
				auto& area = *(study.areas.byIndex[s]);

				
				assert(area.load.series);
				assert(static_cast<uint>(Data::fhrDSM) < area.reserves.width);

				auto& matrix = area.load.series->series;
				auto& dsmvalues = area.reserves.column(Data::fhrDSM);

				
				assert(matrix.width  > 0);
				assert(matrix.height > 0);
				for (uint timeSeries = 0; timeSeries < matrix.width; ++timeSeries)
				{
					auto& perHour = matrix.column(timeSeries);
					for (uint h = 0; h < matrix.height; ++h)
					{
						perHour[h] += dsmvalues[h];
						assert(!Math::NaN(perHour[h]));
					}
				}

				
				area.reserves.flush();
				matrix.flush();
			}
		}

		# ifdef ANTARES_SWAP_SUPPORT
		
		Antares::memory.flushAll();
		# endif
		return true;
	}






	bool XCast::run()
	{
		switch (timeSeriesType)
		{
			case Data::timeSeriesLoad:
				{
					
					Solver::Progression::Task p(study, year, Progression::sectTSGLoad);

					
					nbTimeseries = study.parameters.nbTimeSeriesLoad;
					
					Predicate::Load predicate;
					return runWithPredicate(predicate, p);
				}
			case Data::timeSeriesSolar:
				{
					
					Solver::Progression::Task p(study, year, Progression::sectTSGSolar);

					
					nbTimeseries = study.parameters.nbTimeSeriesSolar;
					
					Predicate::Solar predicate;
					return runWithPredicate(predicate, p);
				}
			case Data::timeSeriesWind:
				{
					
					Solver::Progression::Task p(study, year, Progression::sectTSGWind);

					
					nbTimeseries = study.parameters.nbTimeSeriesWind;
					
					Predicate::Wind predicate;
					return runWithPredicate(predicate, p);
				}

			default:
				{
					
					exit(1);
				}
		}
		return false;
	}





} 
} 
} 
} 

