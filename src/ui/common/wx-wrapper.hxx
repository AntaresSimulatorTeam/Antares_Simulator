// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_WX_WIDGETS_HXX__
#define __ANTARES_TOOLBOX_WX_WIDGETS_HXX__

inline wxString wxStringFromUTF8(const std::string& s)
{
#if wxUSE_UNICODE
    return wxString::FromUTF8(s.c_str(), s.size());
#else
    return wxString(s.c_str(), s.size());
#endif
}

inline wxString wxStringFromUTF8(const char* s)
{
#if wxUSE_UNICODE
    return wxString::FromUTF8(s);
#else
    return wxString(s);
#endif
}

inline wxString wxStringFromUTF8(const Yuni::String& s)
{
#if wxUSE_UNICODE
    return wxString::FromUTF8(s.c_str(), s.size());
#else
    return wxString(s.c_str(), s.size());
#endif
}

inline wxString wxStringFromUTF8(const char* const s, uint length)
{
#if wxUSE_UNICODE
    return wxString::FromUTF8(s, length);
#else
    return wxString(s, length);
#endif
}

template<uint ChunkT, bool FixedT>
inline wxString wxStringFromUTF8(const Yuni::CString<ChunkT, FixedT>& s)
{
#if wxUSE_UNICODE
    return wxString::FromUTF8(s.c_str(), s.size());
#else
    return wxString(s.c_str(), s.size());
#endif
}

template<class StringT>
inline void wxStringToString(const wxString& s, StringT& out)
{
#if wxUSE_UNICODE
    const wxCharBuffer buffer(s.mb_str(wxConvUTF8));
    out.assign((const char*)buffer, (uint)strlen((const char*)buffer));
#else
    out = (const char*)s.mb_str();
#endif
}

template<int PrecisionT>
wxString DoubleToWxStringS(double f)
{
    Yuni::CString<128, false> tmp;
    switch (PrecisionT)
    {
    case 0:
        tmp.format("%.0f", f);
        break;
    case 1:
        tmp.format("%.1f", f);
        break;
    case 2:
        tmp.format("%.2f", f);
        break;
    case 3:
        tmp.format("%.3f", f);
        break;
    case 4:
        tmp.format("%.4f", f);
        break;
    case 5:
        tmp.format("%.5f", f);
        break;
    case 6:
        tmp.format("%.6f", f);
        break;
    case 7:
        tmp.format("%.7f", f);
        break;
    case 8:
        tmp.format("%.8f", f);
        break;
    case 9:
        tmp.format("%.9f", f);
        break;
    default:
        tmp.format("%.f", f);
        break;
    }
    if (tmp.contains('.'))
    {
        tmp.trimRight('0');
        if (tmp.last() == '.')
            tmp.removeLast();
    }
    return wxStringFromUTF8(tmp);
}

template<class StringT1, class StringT2>
void AppendWithQuotes(StringT1& out, const StringT2& text)
{
    if (!text.empty())
    {
        if (text.contains('"'))
        {
            Yuni::String copy = text;
            copy.replace("\"", "\\\"");
            out << '"' << copy << '"';
        }
        else
            out << '"' << text << '"';
    }
    else
        out << "\"\"";
}

#endif // __ANTARES_TOOLBOX_WX_WIDGETS_HXX__
