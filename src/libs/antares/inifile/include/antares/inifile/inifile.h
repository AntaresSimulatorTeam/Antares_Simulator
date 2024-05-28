/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <optional>

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
        Property() = default;
        explicit Property(const AnyString& key);
        template<class U>
        Property(const AnyString& key, const U& value);
        ~Property() = default;

        void saveToStream(std::ostream& file, uint64_t& written) const;

    public:
        //! The key
        YString key;
        //! Its associated value
        YString value;
        //! The next value
        Property* next = nullptr;
    };

    /*!
    ** \brief A single section, with all its keys
    ** \ingroup inifile
    */
    class Section final
    {
    public:
        Section() = default;
        explicit Section(const AnyString& name);
        ~Section();

        /*!
        ** \brief Add a new property
        */
        template<class U>
        Property* add(const AnyString& key, const U& value);

        template<class U>
        Property* add(const AnyString& key, const std::optional<U>& value);

        void add(const Property& property);

        void saveToStream(std::ostream& file, uint64_t& written) const;

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
        IniFile::Property* firstProperty = nullptr;
        //! The last property of the section
        IniFile::Property* lastProperty = nullptr;
        //! The next section
        Section* next = nullptr;

    }; // class Section

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    IniFile() = default;
    /*!
    ** \brief Load an INI file
    */
    explicit IniFile(const std::filesystem::path& filename);
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
    bool open(const std::string& filename, bool warnings = true);

    bool open(const std::filesystem::path& filename, bool warnings = true);

    bool readStream(std::istream& in_stream);

    /*!
    ** \brief Save the entire INI into a file
    */
    bool save(const AnyString& filename) const;
    void saveToStream(std::ostream&, uint64_t&) const;

    std::string toString() const;

    //! Get the last filename saved or loaded
    const std::string& filename() const;
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
    Section* firstSection = nullptr;
    Section* lastSection = nullptr;

private:
    /*!
    ** \brief The last filename used for loading/saving
    **
    ** \internal This variable to keep const-correctness for the
    ** public API (cf save).
    */
    std::string filename_;

}; // class IniFile

} // namespace Antares

#include "inifile.hxx"
