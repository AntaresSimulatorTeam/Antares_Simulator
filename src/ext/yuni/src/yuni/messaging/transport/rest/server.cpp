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
#include "server.h"
#include "../../../thread/signal.h"
#include <cassert>
#ifndef _MSC_VER
#define _MSC_VER 0 // seems to be required with this version of mongoose
#endif
#include "../../../private/net/messaging/transport/rest/mongoose.h"
#include "request.inc.hpp"
#include "../../../io/filename-manipulation.h"


namespace Yuni
{
namespace Messaging
{
namespace Transport
{
namespace REST
{


	Server::Server() :
		ITransport(tmServer)
	{
		pData = new ServerData();
	}


	Server::~Server()
	{
		ServerData* ptr = pData;
		pData = nullptr;
		delete ptr;
	}


	Net::Error  Server::start()
	{
		assert(pData and "internal error");
		pData->service = pService;
		assert(pService != NULL and "invalid reference to Messaging::Service");

		// stopping mongoose if not alreayd done
		if (pData->ctx)
			mg_stop(pData->ctx);

		// reset all internal states
		pData->thread = nullptr;
		pData->signal.reset();

		// re-create mongoose options
		pData->prepareOptionsForMongoose(port, 4);

		// starting mongoose
		pData->ctx = mg_start(& TransportRESTCallback, pData, pData->options);
		if (not pData->ctx)
			return Net::errStartFailed;

		return Net::errNone;
	}


	void Server::wait()
	{
		if (YUNI_LIKELY(pData))
		{
			// wait for being stopped
			if (pData->signal.valid())
			{
				pData->signal.wait();
			}
			else
			{
				// the code should never reach this location (unless the signal is invalid)
				pData->waitWithoutSignal();
			}
		}
	}


	Net::Error  Server::run()
	{
		assert(pData and "internal error");

		// Get the attached thread
		pData->thread = pAttachedThread;

		// infinite wait, until we receive a message to stop
		wait();

		// waiting for mongoose to stop
		mg_stop(pData->ctx);

		pData->ctx = nullptr;
		pData->thread = nullptr;
		return Net::errNone;
	}


	void Server::stop()
	{
		assert(pData and "internal error");
		// notifying that we should stop as soon as possible
		if (pData->signal.valid())
		{
			pData->signal.notify();
		}
		else
		{
			if (pData->thread)
				pData->thread->gracefulStop();
		}
	}


	void Server::protocol(const Protocol& protocol)
	{
		DecisionTree* decisionTree = new DecisionTree();
		String url;
		String tmp;
		String httpMethod;

		// walking through all schemas
		const Schema::Hash& allSchemas = protocol.allSchemas();
		Schema::Hash::const_iterator end = allSchemas.end();
		for (Schema::Hash::const_iterator i = allSchemas.begin(); i != end; ++i)
		{
			// relative path access
			const String& schemaName = i->first;
			// alias to the current schema
			const Schema& schema = i->second;

			API::Method::Hash::const_iterator jend = schema.methods.all().end();
			for (API::Method::Hash::const_iterator j = schema.methods.all().begin(); j != jend; ++j)
			{
				const API::Method& method = j->second;

				// The method will be ignored is no callback has been provided
				if (not method.invoke())
					continue;

				httpMethod = method.option("http.method");
				// using const char* to avoid assert from Yuni::String
				RequestMethod rqmd = StringToRequestMethod(httpMethod.c_str());
				if (rqmd == rqmdInvalid)
					rqmd = rqmdGET;

				// the full url
				tmp.clear() << '/' << schemaName << '/' << method.name();
				IO::Normalize(url, tmp);

				// keeping the url somewhere
				// IMPORTANT: read notes about how urls are stored
				std::set<String>& mapset = decisionTree->mapset[rqmd];
				mapset.insert(url);
				// retrieving the real pointer to the string
				std::set<String>::const_iterator mapi = mapset.find(url);
				AnyString urlstr = *mapi;

				// alias to the corresponding method handler
				DecisionTree::MethodHandler& mhandler = decisionTree->requestMethods[rqmd][urlstr];

				mhandler.schema = schemaName;
				mhandler.name = method.name();
				mhandler.httpMethod = httpMethod;
				mhandler.invoke = method.invoke();

				// copying parameters
				const API::Method::Parameter::Hash& parameters = method.params();
				if (not parameters.empty())
				{
					API::Method::Parameter::Hash::const_iterator pend = parameters.end();
					API::Method::Parameter::Hash::const_iterator pi = parameters.begin();
					for (; pi != pend; ++pi)
					{
						const API::Method::Parameter& param = pi->second;
						if (param.hasDefault)
						{
							mhandler.parameters[param.name] = param.defvalue;
						}
						else
						{
							API::Method::Parameter::Hash::const_iterator defit = schema.defaults.params().find(param.name);
							if (defit != schema.defaults.params().end())
								mhandler.parameters[param.name] = defit->second.defvalue;
							else
								mhandler.parameters[param.name].clear();
						}
					}
				}
				else
					mhandler.parameters.clear();
			}
		}

		// Switching to the new protocol
		pData->decisionTree = decisionTree;
	}





} // namespace REST
} // namespace Transport
} // namespace Messaging
} // namespace Yuni

