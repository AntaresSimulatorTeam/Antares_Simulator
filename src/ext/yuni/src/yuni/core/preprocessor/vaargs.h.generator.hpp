/*
** YUNI's default license is the GNU Lesser Public License (LGPL), with some
** exclusions (see below). This basically means that you can get the full source
** code for nothing, so long as you adhere to a few rules.
**
** Under the LGPL you may use YUNI for any purpose you wish, and modify it if you
** require, as long as you:
**
** Pass on the (modified) YUNI source code with your software, with original
** copyrights intact :
**  * If you distribute electronically, the source can be a separate download
**    (either from your own site if you modified YUNI, or to the official YUNI
**    website if you used an unmodified version) – just include a link in your
**    documentation
**  * If you distribute physical media, the YUNI source that you used to build
**    your application should be included on that media
** Make it clear where you have customised it.
**
** In addition to the LGPL license text, the following exceptions / clarifications
** to the LGPL conditions apply to YUNI:
**
**  * Making modifications to YUNI configuration files, build scripts and
**    configuration headers such as yuni/platform.h in order to create a
**    customised build setup of YUNI with the otherwise unmodified source code,
**    does not constitute a derived work
**  * Building against YUNI headers which have inlined code does not constitute a
**    derived work
**  * Code which subclasses YUNI classes outside of the YUNI libraries does not
**    form a derived work
**  * Statically linking the YUNI libraries into a user application does not make
**    the user application a derived work.
**  * Using source code obsfucation on the YUNI source code when distributing it
**    is not permitted.
** As per the terms of the LGPL, a "derived work" is one for which you have to
** distribute source code for, so when the clauses above define something as not
** a derived work, it means you don't have to distribute source code for it.
** However, the original YUNI source code with all modifications must always be
** made available.
*/
#pragma once


<%
require File.dirname(__FILE__) + '/../../../tools/generators/commons.rb'
generator = Generator.new()

maxArgs = 512
%>
<%=generator.thisHeaderHasBeenGenerated("vaargs.h.generator.hpp")%>

/*!
** \internal This header must remain compatible with C code.
*/


/*!
** \brief Get the number of arguments
**
** \warning YUNI_VAARGS_COUNT() without arguments would violate 6.10.3p4 of ISO C99
** \note This code was initially created by 'Laurent Deniau'
** \see http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5
*/
#define YUNI_VAARGS_COUNT(...) \
		YUNI_PRIVATE_VAARGS_N_ARG_(__VA_ARGS__, YUNI_PRIVATE_VAARGS_RSEQ_N())


/*!
** \brief Get the value of the Nth argument (zero-based)
**
** \param Index The index of the argument (zero-based)
*/
#define YUNI_VAARGS_VALUE(Index, ...) \
		YUNI_PRIVATE_VAARGS_N_ARG_VALUE(Index, __VA_ARGS__, YUNI_PRIVATE_VAARGS_RSEQ_N())


/*!
** \brief Call an arbitrary macro for each argument given in a varadiac parameter
**
** The macro must have the following signature :
** \code
** # define MY_MACRO(Index, Value)  DoSomething...
** \endcode
** The index is zero-based.
**
** \param Macro The macro to call for each argument
*/
#define YUNI_VAARGS_FOREACH(Macro, ...) \
		YUNI_PRIVATE_VAARGS_CALL_MACRO(Macro, YUNI_EMPTY, \
		YUNI_VAARGS_COUNT(__VA_ARGS__), __VA_ARGS__)

/*!
** \brief Call an arbitrary macro for each argument given in a variadic parameter
**
** The macro must have the following signature :
** \code
** # define MY_MACRO(Index, Value)  DoSomething...
** \endcode
** The index is zero-based.
**
** \param Macro The macro to call for each argument
** \param MacroSeparator The separator to add between each call to the previously given macro
*/
#define YUNI_VAARGS_FOREACH_WITH_SEPARATOR(Macro, MacroSeparator, ...) \
		YUNI_PRIVATE_VAARGS_CALL_MACRO( \
			Macro, MacroSeparator,	YUNI_VAARGS_COUNT(__VA_ARGS__), __VA_ARGS__)








/* Private macros, that _should not_ be used directly */

/*!
** \brief String concatenation
**
** This define is a replacement for YUNI_JOIN, in order to be able to use this
** header alone.
*/
#define YUNI_PRIVATE_VAARGS_JOIN(X,Y)  X ## Y


/*!
** \brief An empty value
*/
#define YUNI_PRIVATE_VAARGS_EMPTY()

/* Related to YUNI_VAARGS_COUNT */

#define YUNI_PRIVATE_VAARGS_N_ARG_(...) \
		YUNI_PRIVATE_VAARGS_ARG_N_INTERMEDIATE(__VA_ARGS__)

#define YUNI_PRIVATE_VAARGS_ARG_N_INTERMEDIATE( \
		<%
	(0..maxArgs).each do |i|
		%>_<%=i%>,<%
	end
%> \
		N,...) N


/* Related to both YUNI_VAARGS_VALUE and YUNI_VAARGS_COUNT */

#define YUNI_PRIVATE_VAARGS_RSEQ_N() \
		<%
	(1..(maxArgs+1)).reverse_each do |i|
		%><%=i%>,<%
	end
%>0



/* Related to YUNI_VAARGS_VALUE */

#define YUNI_PRIVATE_VAARGS_N_ARG_VALUE(Index, ...) \
		YUNI_PRIVATE_VAARGS_ARG_N_INTERMEDIATE_VALUE(Index, __VA_ARGS__)


#define YUNI_PRIVATE_VAARGS_ARG_N_INTERMEDIATE_VALUE(I , \
		<%
	(0..maxArgs).each do |i|  %>_<%=i%>,<% end %> \
		N,...) \
	YUNI_PRIVATE_VAARGS_CALL_INDEX(YUNI_PRIVATE_VAARGS_JOIN(YUNI_PRIV_VAINX_ , I), \
		<%
	(0..maxArgs-1).each do |i| %>_<%=i%>,<% end %>_<%=maxArgs%>)



#define YUNI_PRIVATE_VAARGS_CALL_INDEX( C, \
		<%
	(0..maxArgs-1).each do |i| %>_<%=i%>,<% end %>_<%=maxArgs%>) \
	C( \
		<%
	(0..maxArgs-1).each do |i| %>_<%=i%>,<% end %>_<%=maxArgs%>)



<%
	(0..maxArgs - 1).each do |i| %># define YUNI_PRIV_VAINX_<%=i%>( \
	<%
	(1..(i+1)).each do |j| %>_<%=j%>,<% end %>...) \
		_<%=(i+1)%>
<% end %>



/* Related to YUNI_VAARGS_FOREACH */

#define YUNI_PRIVATE_VAARGS_CALL_MACRO(Macro, MacroSeparator, Total, ...) \
		YUNI_EMPTY, YUNI_PRIVATE_VAARGS_CALL_MC_INTERMEDIATE(Macro, MacroSeparator, \
			YUNI_PRIVATE_VAARGS_JOIN(YUNI_PRIVATE_VAARGS_CALL_MC_ , Total), __VA_ARGS__,piko )

#define YUNI_PRIVATE_VAARGS_CALL_MC_INTERMEDIATE(Macro, MacroSeparator, C, ...) \
		C(Macro, MacroSeparator, __VA_ARGS__)


#define YUNI_PRIVATE_VAARGS_CALL_MC_0(Macro, MacroSeparator, ...)

<%
	(1..maxArgs).each do |i| %># define YUNI_PRIVATE_VAARGS_CALL_MC_<%=i%>(Macro, MacroSeparator, \
	<%
	(1..(i)).each do |j| %>_<%=j%>,<% end %>...) \
		YUNI_PRIVATE_VAARGS_CALL_MC_<%=(i-1)%>(Macro, MacroSeparator, <% (1..(i-1)).each do |k| %>_<%=k%>,<% end %> Dummy) \
		<% if i > 1 %> MacroSeparator <% end %> Macro(<%=(i-1)%>, _<%=(i)%>)
<% end %>

