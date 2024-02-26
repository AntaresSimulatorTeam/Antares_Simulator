#include <memory>
#include <string>

#include <antares/study/header.h>
#include <antares/study/study.h>
#include <antares/logs/logs.h>

using namespace Antares;

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    std::cout << "hello" << std::endl;

    auto study = std::make_shared<Data::Study>(true);
    Data::StudyLoadOptions studyOptions;
    study->loadFromFolder(argv[1], studyOptions);

    for (auto& [name, area] : study->areas)
        for (auto& c : area->thermal.list.all())
            logs.notice() << c->name();

    return 0;
}

