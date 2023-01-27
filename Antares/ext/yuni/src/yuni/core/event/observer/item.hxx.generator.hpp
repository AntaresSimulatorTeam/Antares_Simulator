<?php

define('ARG_MAX', 16);


// Generated templates parameter comments, from A0 to Ai.
function GenerateTemplateComments($i)
{
	if (!$i)
		return '';
	$ret = '';
	for ($j = 0; $j < $i; $j++)
	{
		$ret .= '	** \tparam A'.$j.' Type of the '.($j + 1).'th argument'."\n";
	}
	return $ret;
}

// Generated templates parameters, from A0 to Ai.
function GenerateTemplateParameters($i)
{
	if (!$i)
		return '';
	$ret = '';
	for ($j = 0; $j < $i; $j++)
	{
		if ($j)
			$ret .= ', ';
		$ret .= 'typename A'.$j;
	}
	return $ret;
}

function GenerateArguments($i)
{
	if (!$i)
		return '';
	$ret = '';
	for ($j = 0; $j < $i; $j++)
	{
		if ($j)
			$ret .= ', ';
		$ret .= 'A'.$j;
	}
	return $ret;
}

function GenerateArgumentsWithName($i)
{
	if (!$i)
		return '';
	$ret = '';
	for ($j = 0; $j < $i; $j++)
	{
		if ($j)
			$ret .= ', ';
		$ret .= 'A'.$j.' a'.$j;
	}
	return $ret;
}

function GenerateArgumentsNameOnly($i)
{
	if (!$i)
		return '';
	$ret = '';
	for ($j = 0; $j < $i; $j++)
	{
		if ($j)
			$ret .= ', ';
		$ret .= 'a'.$j;
	}
	return $ret;
}



ob_start();
?>
#pragma once
#include "item.h"



namespace Yuni
{
namespace Event
{

<?php
for ($i = 0; $i <= ARG_MAX; ++$i)
{
	$genArgs = GenerateArguments($i);
	$genArgsWithName = GenerateArgumentsWithName($i);
	$genArgsNameOnly = GenerateArgumentsNameOnly($i);
	$genTpl = GenerateTemplateParameters($i);
?>
	template<class C, class Mode, class R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy>
	inline
	ObserverItemA<?=$i?><C,Mode, R<?=$genArgs ? ", ".$genArgs : ""?>, Dummy>::ObserverItemA<?=$i?>(C* o, R (C::*method)(<?=$genArgs?>)) :
		pObserver(o),
		pMethod(method)
	{
		/* Asserts */
		assert(o != NULL and "The given object can not be NULL");
	}

	template<class C, class Mode, class R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy>
	inline IObserver*
	ObserverItemA<?=$i?><C,Mode, R<?=$genArgs ? ", ".$genArgs : ""?>, Dummy>::observer() const
	{
		return pObserver;
	}

	template<class C, class Mode, class R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy>
	inline bool
	ObserverItemA<?=$i?><C,Mode, R<?=$genArgs ? ", ".$genArgs : ""?>, Dummy>::equalsTo(const IObserver* o) const
	{
		return (pObserver == o);
	}

	template<class C, class Mode, class R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy>
	inline R
	ObserverItemA<?=$i?><C,Mode, R<?=$genArgs ? ", ".$genArgs : ""?>, Dummy>::fireEvent(<?=$genArgsWithName?>) const
	{
		(pObserver->*pMethod)(<?=$genArgsNameOnly?>);
	}

<?php
}
?>



} // namespace Event
} // namespace Yuni

#endif // __YUNI_CORE_EVENT_OBSERVER_ITEM_HXX__
<?php
$file = ob_get_clean();
 file_put_contents(dirname(__FILE__)."/item.hxx", $file);
 die();
?>
