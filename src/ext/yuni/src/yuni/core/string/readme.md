
String
======

*Yuni::String* is a full-featured, lightning-fast, UTF-8 character string.

*AnyString* is a lightweight wrapper for in-place substrings.

*Yuni::String* goes well beyond all the research and manipulation features
you might expect :
 * It can be looped over as a byte string or as a UTF-8 char string
 * It is fully compatible with std::string and can be used as a drop-in
   replacement for it.
 * It offers globbing
 * It offers conversion to and from arithmetic values
 * It can be *split* using user-defined separators
and last but not least :
 * It parses your string into *words* invoking callbacks on-the-fly
   (best leveraged with C++11).
