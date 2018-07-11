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



namespace Yuni
{
namespace Event
{

	/*!
	** \brief Observer (Interface)
	** \ingroup Events
	*/
	class YUNI_DECL IObserver
	{
		YUNI_EVENT_ALLFRIEND_DECL_E;
	public:
		virtual ~IObserver() {}
	protected:
		virtual void internalAttachEvent(IEvent* evt) = 0;
		virtual void internalDetachEvent(const IEvent* evt) = 0;

	}; // class IObserver



	/*!
	** \brief Observer (Base class)
	** \ingroup Events
	**
	** Only classes derived from the class `Observer` can pretend to receive
	** signals from an event. This is due to the fact that there is a strong
	** relationship between the emitter and the receiver, to prevent the
	** corruption of pointers.
	**
	** The class uses the CRTP (Curiously recurring template pattern) Idiom to
	** broadcast the good type to the threading policy.
	**
	** The derived class *must* disconnect all event emitters as soon as possible
	** when the instance is being destroyed to avoid race exceptions (the VTable may
	** partially be deleted). The method `destroyingObserver()` prevents any further
	** connection with an event emitter and must always be called at least from the
	** destructor.
	**
	**
	** A complete example using the threading policies :
	** \code
	** #include <iostream>
	** #include <yuni/core/event.h>
	**
	**
	** template<template<class> class TP = Policy::ObjectLevelLockable>
	** class ThermalSensor : public TP<ThermalSensor>
	** {
	** public:
	** 		//! The threading policy
	** 		typedef TP<ThermalSensor>  ThreadingPolicy;
	**
	** public:
	** 		ThermalSensor() :pLastValue(0.) {}
	** 		~ThermalSensor() {}
	**
	** 		Event<void (float)> eventOnChanged;
	**
	** 		void update(const float newT)
	** 		{
	** 			{
	** 				typename ThreadingPolicy::MutexLocker locker(*this);
	** 				if (pLastValue - newT < 0.1f)
	** 					pLastValue = newT;
	** 				else
	** 					return;
	** 			}
	** 			onChanged(newT);
	** 		}
	**
	** private:
	** 		float pLastValue;
	** };
	**
	**
	** template<template<class> class TP = Policy::ObjectLevelLockable>
	** class Radiator : Event::Observer<Radiator<TP>, TP>
	** {
	** public:
	** 		//! The threading policy
	** 		typedef TP<ThermalSensor>  ThreadingPolicy;
	**
	** public:
	** 		Radiator(const String& name, const float limit)
	** 			:pName(name), pStarted(false), pLimit(limit)
	** 		{}
	**
	** 		~Radiator()
	** 		{
	** 			destroyingObserver();
	** 		}
	**
	** 		String name()
	** 		{
	** 			typename ThreadingPolicy::MutexLocker locker(*this);
	** 			return pName;
	** 		}
	**
	** 		void start()
	** 		{
	** 			typename ThreadingPolicy::MutexLocker locker(*this);
	** 			if (!pStarted)
	** 			{
	** 				pStarted = true;
	** 				std::cout << "The radiator `" << pName << "` has started." << std::endl;
	** 			}
	** 		}
	**
	** 		void stop()
	** 		{
	** 			typename ThreadingPolicy::MutexLocker locker(*this);
	** 			if (pStarted)
	** 			{
	** 				pStarted = false;
	** 				std::cout << "The radiator `" << pName << "`has stopped." << std::endl;
	** 			}
	** 		}
	**
	** 		void onTemperatureChanged(float t)
	** 		{
	** 			std::cout << "Temperature: " << t << std::endl;
	** 			checkTemperature(t);
	** 		}
	**
	** 		float limit() const {return pLimit;}
	** 		void limit(const float newL) {pLimit = newL;}
	**
	** private:
	** 		void checkTemperature(float t)
	** 		{
	** 			typename ThreadingPolicy::MutexLocker locker(*this);
	** 			if (t >= pLimit)
	** 			{
	** 				if (pStarted)
	** 					stop();
	** 			}
	** 			else
	** 			{
	** 				if (!pStarted)
	** 					start();
	** 			}
	** 		}
	**
	** private:
	** 		const String pName;
	** 		bool pStarted;
	** 		float pLimit;
	** };
	**
	**
	** int main(void)
	** {
	** 		// Our thermal sensor
	** 		ThermalSensor<> sensor;
	**
	** 		// Our observer
	** 		Radiator<> radiatorA("A", 30.0);
	** 		Radiator<> radiatorB("B", 16.0);
	** 		sensor.eventOnChanged.connect(&radiatorA, &Radiator::onTemperatureChanged);
	** 		sensor.eventOnChanged.connect(&radiatorB, &Radiator::onTemperatureChanged);
	**
	** 		sensor.update(-2.);
	** 		sensor.update(10.1);
	** 		sensor.update(15.9);
	** 		sensor.update(22.7);
	** 		sensor.update(42.);
	**
	** 		return 0;
	** }
	** \endcode
	**
	** \tparam D The Derived class (CRTP)
	** \tparam TP The threading policy
	*/
	template<class D, template<class> class TP = Policy::ObjectLevelLockable>
	class YUNI_DECL Observer : public TP<D>, public IObserver
	{
		YUNI_EVENT_ALLFRIEND_DECL_E;
	public:
		//! The threading policy
		typedef TP<D>  ThreadingPolicy;

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		Observer();
		//! Destructor
		virtual ~Observer();
		//@}

	protected:
		/*!
		** \brief Disconnect all event emitters and prevents any further connection
		*/
		void destroyingObserver();

		/*!
		** \brief Disconnect all event emitters, if any
		*/
		void disconnectAllEventEmitters();

		/*!
		** \brief Disconnect an event emitter, if connected to the observer
		**
		** \param event The event to disconnect
		*/
		void disconnectEvent(const IEvent* event);

	private:
		/*!
		** \internal Attach an event emitter without doing anything else
		*/
		virtual void internalAttachEvent(IEvent* evt);

		/*!
		** \internal Detach an event emitter without doing anything else
		*/
		virtual void internalDetachEvent(const IEvent* evt);

	private:
		//! List of events that are linked with the observer
		IEvent::List pEvents;
		/*!
		** \brief True if we can connect to the observer
		**
		** False would mean actually that the object is being destroying.
		*/
		bool pCanObserve;

	}; // class Observer





} // namespace Event
} // namespace Yuni

#include "observer.hxx"

