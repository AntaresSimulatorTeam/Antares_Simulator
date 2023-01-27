
Core module
===========

This module is the heart of Yuni.
As such, it is a necessary dependency for all other modules.
It offers many features among which :

 * Useful #defines and function wrappers for cross-platform work
 * Static operations and generic template traits
 * Many useful data types either missing from or unsatisfactory in the STL
   (e.g. String, AnyString, Any, Variant, SList, TreeN, ...) and operations
   on collections (DeleteAndClear, Views, Reverse foreach, ...)
 * Application-level messaging and inter-thread communication (Event, Bind, Atomic, Suspend, ...)
 * Math and Geometry (Unit, Point{2,3}D, Vector{2,3}D, Quaternion, Matrix, ...)
 * Specific often-used tools (GetOpt, HexDump, ...)
 * Much more !
