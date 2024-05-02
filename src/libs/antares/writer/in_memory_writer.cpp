#include <algorithm>

#include <antares/writer/in_memory_writer.h>
#include <antares/logs/logs.h>

#include <antares/benchmarking/timer.h>
#include <antares/benchmarking/DurationCollector.h>

#include <yuni/io/file.h> // Yuni::IO::File::LoadFromFile

namespace fs = std::filesystem;

namespace Antares::Solver {

namespace
{
void logErrorAndThrow [[noreturn]] (const std::string& errorMessage)
{
    Antares::logs.error() << errorMessage;
    throw IOError(errorMessage);
}

  template<class ContentT>
  void addToMap(InMemoryWriter::MapType& entries,
                const std::string& entryPath,
                ContentT& content,
                std::mutex& mutex,
                Benchmarking::IDurationCollector& duration_collector)
  {
    std::string entryPathSanitized = entryPath;
    std::replace(entryPathSanitized.begin(),
                 entryPathSanitized.end(),
                 '\\',
                 '/');

    Benchmarking::Timer timer_wait;
    std::lock_guard lock(mutex);
    timer_wait.stop();
    duration_collector.addDuration("in_memory_wait", timer_wait.get_duration());

    Benchmarking::Timer timer_insert;
    entries.insert({entryPathSanitized, content});
    timer_insert.stop();
    duration_collector.addDuration("in_memory_insert", timer_insert.get_duration());
  }
}


InMemoryWriter::InMemoryWriter(Benchmarking::IDurationCollector& duration_collector) : pDurationCollector(duration_collector) {}

InMemoryWriter::~InMemoryWriter() = default;

void InMemoryWriter::addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent)
{
   addToMap(pEntries,
            entryPath,
            entryContent,
            pMapMutex,
            pDurationCollector);
}
void InMemoryWriter::addEntryFromBuffer(const std::string& entryPath, std::string& entryContent)
{
   addToMap(pEntries,
            entryPath,
            entryContent,
            pMapMutex,
            pDurationCollector);
}

void InMemoryWriter::addEntryFromFile(const fs::path& entryPath, const fs::path& filePath)
{
    // Shamelessly copy-pasted from zip_writer.cpp
    // TODO refactor
    Yuni::Clob buffer;
    switch (Yuni::IO::File::LoadFromFile(buffer, filePath.c_str()))
    {
        using namespace Yuni::IO;
    case errNone:
         addToMap(pEntries,
                  entryPath,
                  buffer,
                  pMapMutex,
                  pDurationCollector);
        break;
    // Since logErrorAndThrow does not return, we don't need 'break's here
    case errNotFound: logErrorAndThrow(filePath.string() + ": file does not exist");
    case errReadFailed: logErrorAndThrow("Read failed '" + filePath.string() + "'");
    case errMemoryLimit: logErrorAndThrow("Size limit hit for file '" + filePath.string() + "'");
    default: logErrorAndThrow("Unhandled error");
    }
}
void InMemoryWriter::flush()
{
    // Nothing to do here
}

bool InMemoryWriter::needsTheJobQueue() const
{
    // We may reconsider if performance is atrocious
    return false;
}

void InMemoryWriter::finalize(bool /* verbose */)
{
    // Nothing to do here
}

const InMemoryWriter::MapType& InMemoryWriter::getMap() const
{
    return pEntries;
}

}
