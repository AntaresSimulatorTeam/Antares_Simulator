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



namespace Yuni
{
namespace Event
{

<?php
for ($i = 0; $i <= ARG_MAX; ++$i)
{
	$genArgsWithName = GenerateArgumentsWithName($i);
	$genTpl = GenerateTemplateParameters($i);
?>

	/*!
	** \brief Item for a list of observer(Interface)
	**
	** \tparam R The type of the returned value
<?=GenerateTemplateComments($i)?>
	*/
	template<typename R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy = void>
	class IObserverItemA<?=$i."\n"?>
	{
	public:
		virtual ~IObserverItemA<?=$i?>() {}
		virtual IObserver* observer() const = 0;
		virtual bool equalsTo(const IObserver* o) const = 0;
		virtual R fireEvent(<?=$genArgsWithName?>) const = 0;

	}; // class IObserverItemA<?=$i?>


<?php
}
?>





<?php
for ($i = 0; $i <= ARG_MAX; ++$i)
{
	$genArgs = GenerateArguments($i);
	$genArgsWithName = GenerateArgumentsWithName($i);
	$genArgsNameOnly = GenerateArgumentsNameOnly($i);
	$genTpl = GenerateTemplateParameters($i);
?>


	/*!
	** \tparam C The real type of the observer
	** \tparam Mode The mode when firing the event
	** \tparam R The type of the returned value
<?=GenerateTemplateComments($i)?>
	*/
	template<class C, class Mode, class R<?=$genTpl ? ", ".$genTpl : ""?>, class Dummy = void>
	class ObserverItemA<?=$i?> final : public IObserverItemA<?=$i?><R<?=$genArgs ? ", ".$genArgs : ""?>, Dummy>
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		**
		** \param o Pointer to the object
		** \param method Pointer to member
		*/
		ObserverItemA<?=$i?>(C* o, R (C::*method)(<?=$genArgs?>));
		//! Destructor
		virtual ~ObserverItemA<?=$i?>() {}
		//@}

		/*!
		** \brief Fire the event
		*/
		virtual R fireEvent(<?=$genArgsWithName?>) const;

		//! Get a pointer to the observer object
		virtual IObserver* observer() const;
		//! Test if the class contains a given observer
		virtual bool equalsTo(const IObserver* o) const;

	private:
		//! The real observer
		C* pObserver;
		//! Pointer to method
		R (C::*pMethod)(<?=$genArgs?>);

	}; // class ObserverItem


<?php
}
?>



} // namespace Event
} // namespace Yuni

# include "item.hxx"

#endif // __YUNI_CORE_EVENT_OBSERVER_ITEM_H__
<?php
$file = ob_get_clean();
 file_put_contents(dirname(__FILE__)."/item.h", $file);
 die();
?>
