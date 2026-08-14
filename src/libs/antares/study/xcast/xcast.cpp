// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/xcast/xcast.h"

#include <limits>

#include <antares/inifile/inifile.h>
#include <antares/io/file.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>
#include "antares/study//study.h"


namespace fs = std::filesystem;

static const std::string SEP(1, '/');

namespace Antares::Data
{

const char* XCast::TSTranslationUseToCString(TSTranslationUse use)
{
    static const char* const names[3] = {"never", "before-conversion", "after-conversion"};
    assert((int)use < 3);
    return names[use];
}

XCast::TSTranslationUse XCast::CStringToTSTranslationUse(const std::string& str)
{
    if (not str.empty())
    {
        const std::string s = Antares::stringToLower(str);
        if (s == "never" || s == "do not use" || s == "none" || s == "no")
        {
            return tsTranslationNone;
        }
        if (s == "before-conversion" || s == "add before conversion" || s == "before conversion"
            || s == "before" || s == "before scaling" || s == "add before scaling")
        {
            return tsTranslationBeforeConversion;
        }
        if (s == "after-conversion" || s == "add after conversion" || s == "after conversion"
            || s == "after" || s == "after scaling" || s == "add after scaling")
        {
            return tsTranslationAfterConversion;
        }
    }
    return tsTranslationNone;
}

const char* XCast::DistributionToCString(XCast::Distribution d)
{
    assert((int)d < dtMax);
    static const char* const names[dtMax] = {"", "Uniform", "Beta", "Normal", "Weibull", "Gamma"};
    return names[d];
}

const char* XCast::DistributionToNameID(XCast::Distribution d)
{
    static const char* const names[dtMax] = {
      "unknown",
      "Uniform",
      "Beta",
      "Normal",
      "WeibullShapeA",
      "GammaShapeA",
    };
    assert((int)d < dtMax);
    return names[d];
}

XCast::Distribution XCast::StringToDistribution(const std::string& text)
{
    // temporary string for text manipulation
    std::string id = Antares::stringTrim(text);
    id = Antares::stringToLower(id);

    if (id.size() == 1)
    {
        switch (id.front()) // one letter, mainly used from the interface
        {
        case 'b':
            return dtBeta;
        case 'u':
            return dtUniform;
        case 'n':
            return dtNormal;
        case 'w':
            return dtWeibullShapeA;
        case 'g':
            return dtGammaShapeA;
        }
    }
    else
    {
        // The most usefull probability distribution is the Beta distribution
        if (id == "beta")
        {
            return dtBeta;
        }
        if (id == "uniform")
        {
            return dtUniform;
        }
        if (id == "normal" || id == "normale")
        {
            return dtNormal;
        }
        if (id == "weibullshapea" || id == "weibull" || id == "weibul")
        {
            return dtWeibullShapeA;
        }
        if (id == "gammashapea" || id == "gamma")
        {
            return dtGammaShapeA;
        }
        // auto-fix for intermediate studies in 3.3.x. Can be removed in future releases
        if (id == "weibulshapea")
        {
            return dtWeibullShapeA;
        }
    }
    return dtNone;
}

XCast::XCast(TimeSeriesType ts):
    useTranslation(tsTranslationNone),
    distribution(dtBeta),
    capacity(0),
    useConversion(false),
    timeSeries(ts)
{
    K.resize(12, 24);
    data.resize((unsigned int)dataMax, 12);
    // Do nothing
}

XCast::~XCast()
{
    data.clear();
    K.clear();
    translation.clear();
    conversion.clear();
}

void XCast::resetToDefaultValues()
{
    data.reset(dataMax, 12);
    data.fillColumn(dataCoeffAlpha, 1.f);
    data.fillColumn(dataCoeffBeta, 1.f);
    data.fillColumn(dataCoeffDelta, 1.f);
    data.fillColumn(dataCoeffTheta, 1.f);
    data.fillColumn(dataCoeffMu, 1.f);
    K.reset(12, 24);
    distribution = dtBeta;
    capacity = 0.;
    useConversion = false;
    resetTransferFunction();

    // Time-series translation
    translation.reset(1, HOURS_PER_YEAR);
    useTranslation = tsTranslationNone;
}

bool XCast::loadFromFolder(const fs::path& folder)
{
    // reset
    distribution = dtBeta;
    capacity = 0.;
    useTranslation = tsTranslationNone;
    useConversion = false;

    // A temporary buffer for reading matrices
    Matrix<>::BufferType readBuffer;
    // Return value
    bool ret = true;
    // Settings
    fs::path settingsPath = folder / "settings.ini";

    IniFile ini;
    if (ini.open(settingsPath))
    {
        ini.each(
          [this, &settingsPath](const IniFile::Section& section)
          {
              // For each property
              if (section.name == "general")
              {
                  for (const IniFile::Property* p = section.firstProperty; p != nullptr;
                       p = p->next)
                  {
                      const std::string key = Antares::stringToLower(std::string(p->key));
                      if (key == "distribution")
                      {
                          distribution = StringToDistribution(std::string(p->value));
                          if (distribution == dtNone)
                          {
                              logs.warning() << settingsPath
                                             << ": Invalid probability distribution. The beta "
                                                "distribution will be used";
                              distribution = dtBeta;
                          }
                          continue;
                      }
                      if (key == "capacity")
                      {
                          capacity = Antares::stringToDouble(std::string(p->value));
                          if (capacity < 0.)
                          {
                              logs.warning()
                                << settingsPath << ": The capacity can not be a negative value";
                              capacity = 0.;
                          }
                          continue;
                      }
                      if (key == "conversion" || key == "transfer-function" || key == "convertion")
                      {
                          useConversion = Antares::stringToBool(std::string(p->value));
                          continue;
                      }
                      if (key == "translation" || key == "ts-average")
                      {
                          useTranslation = CStringToTSTranslationUse(std::string(p->value));
                          continue;
                      }

                      logs.warning() << settingsPath << ": Unknown property '" << p->key << "'";
                  }
              }
              else
              {
                  logs.warning() << settingsPath << ": unknown section '" << section.name << "'";
              }
          });
    }
    else
    {
        logs.error() << "I/O Error: unable to open '" << settingsPath << "'";
        ret = false;
    }

    // It would be better to load the matrices after the settings in order to
    // fix invalid data

    // Coefficients
    fs::path p = folder / "data.txt";

    // Performing normal loading
    ret = data.loadFromCSVFile(p.string(), (unsigned int)dataMax, 12, Matrix<>::optFixedSize, &readBuffer)
          && ret;

    // K
    p = folder / "k.txt";
    ret = K.loadFromCSVFile(p.string(), 12, 24, Matrix<>::optFixedSize, &readBuffer) && ret;

    unsigned int opts = Matrix<>::optNone;

    // Time-series translation
    p = folder / "translation.txt";

    ret = translation.loadFromCSVFile(p.string(), 1, HOURS_PER_YEAR, opts, &readBuffer) && ret;

    if (translation.empty())
    {
        // This is not really an error
        useTranslation = tsTranslationNone;
        translation.reset(1, HOURS_PER_YEAR);
    }
    else
    {
        if (translation.width != 1 || translation.height != HOURS_PER_YEAR)
        {
            logs.warning() << folder << ": invalid size for the time-series translation.";
            translation.resizeWithoutDataLost(1, HOURS_PER_YEAR);
            useTranslation = tsTranslationNone;
        }
    }

    opts = Matrix<>::optNone;

    // Transfer function
    p = folder / "conversion.txt";

    ret = conversion.loadFromCSVFile(p.string(), 3, 2, opts, &readBuffer) && ret;
    if (conversion.width >= 3 && conversion.width <= conversionMaxPoints)
    {
        // We will overwrite the left and the right value
        // Warning !!! std::numeric_limits must not be used
        //  it produces unwanted behavior on Linux
        conversion[0][0] = (float)(-1.0e+19); // - std::numeric_limits<float>::max();
        conversion[0][1] = conversion[1][1];
        for (unsigned int x = 1; x < conversion.width - 1; ++x)
        {
            if (conversion[x][0] <= -1.0e+19 || conversion[x][0] >= +1.0e+19)
            {
                logs.error() << "TS-Generator: Conversion: Invalid range: " << p;
            }
        }
        conversion[conversion.width - 1][0] = (float)1.0e+19; // +
                                                              // std::numeric_limits<float>::max();
        conversion[conversion.width - 1][1] = conversion[conversion.width - 2][1];
    }
    else
    {
        logs.warning() << "Invalid transfer function: '" << p << "'";
        resetTransferFunction();
        useConversion = false;
    }

    return ret;
}

void XCast::resetTransferFunction()
{
    conversion.reset(3, 2);
    conversion[0][0] = (float)(-1.0e+19); // - std::numeric_limits<float>::max();
    conversion[0][1] = 0.f;
    conversion[1][0] = 0.f;
    conversion[1][1] = 0.f;
    conversion[2][0] = (float)(+1.0e+19); // + std::numeric_limits<float>::max();
    conversion[2][1] = 0.f;
}

bool XCast::saveToFolder(const std::string& folder) const
{
    std::error_code ec;
    std::filesystem::create_directories(folder, ec);
    if (ec)
    {
        logs.error() << "I/O Error: Impossible to create '" << folder << "'";
        return false;
    }

    // result
    bool ret = true;

    // Coefficients
    ret = data.saveToCSVFile(folder + SEP + "data.txt") && ret;

    // K
    ret = K.saveToCSVFile(folder + SEP + "k.txt");

    // TimeSeriesAverage
    ret = translation.saveToCSVFile(folder + SEP + "translation.txt");

    // Transfer function
    ret = conversion.saveToCSVFile(folder + SEP + "conversion.txt");

    // Settings
    IniFile ini;
    IniFile::Section* s = ini.addSection("general");
    if (distribution != dtBeta)
    {
        s->add("distribution", DistributionToNameID(distribution));
    }
    if (!Utils::isZero(capacity))
    {
        s->add("capacity", capacity);
    }
    if (useConversion)
    {
        s->add("conversion", useConversion);
    }
    if (useTranslation != tsTranslationNone)
    {
        s->add("translation", TSTranslationUseToCString(useTranslation));
    }

    // Writing the INI file
    const std::string settingsPath = folder + SEP + "settings.ini";
    if (s->empty())
    {
        // If the section is empty, an empty file will take less
        // disk space and the parsing will be faster (of course)
        return Antares::IO::fileSetContent(settingsPath, "") && ret;
    }
    return ini.save(settingsPath) && ret;
}

void XCast::copyFrom(const XCast& rhs)
{
    // Coeffs
    data = rhs.data;
    // K
    K = rhs.K;
    // translation
    translation = rhs.translation;
    useTranslation = rhs.useTranslation;

    // Conversion
    conversion = rhs.conversion;
    useConversion = rhs.useConversion;

    // capacity
    capacity = rhs.capacity;
    // distribution
    distribution = rhs.distribution;
}

} // namespace Antares::Data
