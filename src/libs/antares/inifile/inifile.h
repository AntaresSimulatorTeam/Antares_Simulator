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
#ifndef __ANTARES_LIBS_INIFILE_INIFILE_H__
#define __ANTARES_LIBS_INIFILE_INIFILE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares
{
/*!
** \brief Data for an INI file
*/
class IniFile final
{
public:
    /*!
    ** \brief A single entry in an INI file
    ** \ingroup inifile
    **
    ** This struct is barely a pair : key/value
    */
    class Property final
    {
    public:
        Property();
        explicit Property(const AnyString& key);
        template<class U>
        Property(const AnyString& key, const U& value);
        ~Property();

        template<class StreamT>
        void saveToStream(StreamT& file, yuint64& written) const;

    public:
        //! The key
        YString key;
        //! Its associated value
        YString value;
        //! The next value
        Property* next;
    };

    /*!
    ** \brief A single section, with all its keys
    ** \ingroup inifile
    */
    class Section final
    {
    public:
        Section();
        explicit Section(const AnyString& name);
        ~Section();

        /*!
        ** \brief Add a new property
        */
        template<class U>
        Property* add(const AnyString& key, const U& value);

        template<class StreamT>
        void saveToStream(StreamT& file, yuint64& written) const;

        Property* find(const AnyString& key);
        const Property* find(const AnyString& key) const;

        /*!
        ** \brief Try to read a property
        **
        ** If the property can not be found, the default value will be used.
        ** \param key The property name
        ** \param defValue The default value
        ** \return The associated value
        */
        template<class U, class StringT>
        U read(const StringT& key, const U& defValue) const;

        /*!
        ** \brief Iterate through all properties
        **
        ** \code
        ** IniFile ini;
        ** if (ini.open("/tmp/test.ini"))
        ** {
        **	ini.each([&] (const IniFile::Section& section)
        **	{
        **		std::cout << '[' << section.name << "]\n";
        **		section.properties([&] (const IniFile::Property& property)
        **		{
        **			std::cout << property.key << " = " << property.value << '\n';
        **		});
        **		std::cout << '\n';
        **	});
        ** }
        ** \endcode
        */
        template<class CallbackT>
        void each(const CallbackT& callback);

        /*!
        ** \brief Iterate through all properties (const)
        **
        ** \code
        ** IniFile ini;
        ** if (ini.open("/tmp/test.ini"))
        ** {
        **	ini.each([&] (const IniFile::Section& section)
        **	{
        **		std::cout << '[' << section.name << "]\n";
        **		section.properties([&] (const IniFile::Property& property)
        **		{
        **			std::cout << property.key << " = " << property.value << '\n';
        **		});
        **		std::cout << '\n';
        **	});
        ** }
        ** \endcode
        */
        template<class CallbackT>
        void each(const CallbackT& callback) const;

        //! Get if the section is empty
        bool empty() const;

        //! Get the number of properties
        uint size() const;

    public:
        //! The name of the section
        Yuni::ShortString256 name;
        //! The first property of the section
        IniFile::Property* firstProperty;
        //! The last property of the section
        IniFile::Property* lastProperty;
        //! The next section
        Section* next;

    }; // class Section

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    IniFile();
    /*!
    ** \brief Load an INI file
    */
    explicit IniFile(const AnyString& filename);
    /*!
    ** \brief Destructor
    */
    ~IniFile();
    //@}

    //! \name Load / Save
    //@{
    /*!
    ** \brief Empty the INI File
    */
    void clear();

    /*!
    ** \brief Get if the INI file has been loaded/written
    */
    bool loaded() const;

    /*!
    ** \brief Load an INI file
    **
    ** \param filename Filename to load
    ** \return True if the operation succeeded, false otherwise
    */
    bool open(const AnyString& filename, bool warnings = true);

    /*!
    ** \brief Save the entire INI into a file
    */
    bool save(const AnyString& filename) const;

    //! Get the last filename saved or loaded
    const YString& filename() const;
    //@}

    //! \name Sections
    //@{
    /*!
    ** \brief Add an existing section into the INI structure
    */
    Section* add(Section* s);

    /*!
    ** \brief Create a new section
    */
    Section* addSection(const AnyString& name);

    /*!
    ** \brief Get if the inifile is empty
    */
    bool empty() const;

    /*!
    ** \brief Try to find a section by its name
    */
    Section* find(const AnyString& name);

    /*!
    ** \brief Try to find a section by its name (const)
    */
    const Section* find(const AnyString& name) const;

    /*!
    ** \brief Iterate through all sections
    **
    ** \code
    ** IniFile ini;
    ** if (ini.open("/tmp/test.ini"))
    ** {
    **	ini.each([&] (IniFile::Section& section)
    **	{
    **		std::cout << section.name << std::endl;
    **	});
    ** }
    ** \endcode
    */
    template<class CallbackT>
    void each(const CallbackT& callback);

    /*!
    ** \brief Iterate through all sections (const)
    **
    ** \code
    ** IniFile ini;
    ** if (ini.open("/tmp/test.ini"))
    ** {
    **	ini.each([&] (const IniFile::Section& section)
    **	{
    **		std::cout << section.name << std::endl;
    **	});
    ** }
    ** \endcode
    */
    template<class CallbackT>
    void each(const CallbackT& callback) const;

    /*!
    ** \brief Iterate through all properties of all sections
    **
    ** \code
    ** IniFile ini;
    ** if (ini.open("/tmp/test.ini"))
    ** {
    **	ini.properties([&] (const IniFile::Section& section, const IniFile::Property& property)
    **	{
    **		std::cout << section.name << ": " << property.key << " = " << property.value <<
    *'\n'; *	});
    ** }
    ** \endcode
    */
    template<class CallbackT>
    void properties(const CallbackT& callback);

    /*!
    ** \brief Iterate through all properties of all sections (const)
    **
    ** \code
    ** IniFile ini;
    ** if (ini.open("/tmp/test.ini"))
    ** {
    **	ini.properties([&] (const IniFile::Section& section, const IniFile::Property& property)
    **	{
    **		std::cout << section.name << ": " << property.key << " = " << property.value <<
    *'\n'; *	});
    ** }
    ** \endcode
    */
    template<class CallbackT>
    void properties(const CallbackT& callback) const;
    //@}

public:
    //! The first section
    Section* firstSection;
    //! The last section
    Section* lastSection;

private:
    /*!
    ** \brief The last filename used for loading/saving
    **
    ** \internal This variable to keep const-correctness for the
    ** public API (cf save).
    */
    mutable YString pFilename;

}; // class IniFile

} // namespace Antares

#include "inifile.hxx"

#endif /* __ANTARES_LIBS_INIFILE_INIFILE_H__ */
