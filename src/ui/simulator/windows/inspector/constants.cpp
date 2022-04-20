#include <antares/xpansion.h>
#include "constants.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
const wxChar* const arrayMinUpDownTime[] = {wxT("1"),
                                            wxT("24"),
                                            wxT("168"),
                                            // --
                                            wxT(" -- "),
                                            // --
                                            wxT("1"),
                                            wxT("2"),
                                            wxT("3"),
                                            wxT("4"),
                                            wxT("5"),
                                            wxT("6"),
                                            wxT("7"),
                                            wxT("8"),
                                            wxT("9"),
                                            wxT("10"),
                                            wxT("11"),
                                            wxT("12"),
                                            wxT("13"),
                                            wxT("14"),
                                            wxT("15"),
                                            wxT("16"),
                                            wxT("17"),
                                            wxT("18"),
                                            wxT("19"),
                                            wxT("20"),
                                            wxT("21"),
                                            wxT("22"),
                                            wxT("23"),
                                            wxT("24"),
                                            wxT("25"),
                                            wxT("26"),
                                            wxT("27"),
                                            wxT("28"),
                                            wxT("29"),
                                            wxT("30"),
                                            wxT("31"),
                                            wxT("32"),
                                            wxT("33"),
                                            wxT("34"),
                                            wxT("35"),
                                            wxT("36"),
                                            wxT("37"),
                                            wxT("38"),
                                            wxT("39"),
                                            wxT("40"),
                                            wxT("41"),
                                            wxT("42"),
                                            wxT("43"),
                                            wxT("44"),
                                            wxT("45"),
                                            wxT("46"),
                                            wxT("47"),
                                            wxT("48"),
                                            wxT("49"),
                                            wxT("50"),
                                            wxT("51"),
                                            wxT("52"),
                                            wxT("53"),
                                            wxT("54"),
                                            wxT("55"),
                                            wxT("56"),
                                            wxT("57"),
                                            wxT("58"),
                                            wxT("59"),
                                            wxT("60"),
                                            wxT("61"),
                                            wxT("62"),
                                            wxT("63"),
                                            wxT("64"),
                                            wxT("65"),
                                            wxT("66"),
                                            wxT("67"),
                                            wxT("68"),
                                            wxT("69"),
                                            wxT("70"),
                                            wxT("71"),
                                            wxT("72"),
                                            wxT("73"),
                                            wxT("74"),
                                            wxT("75"),
                                            wxT("76"),
                                            wxT("77"),
                                            wxT("78"),
                                            wxT("79"),
                                            wxT("80"),
                                            wxT("81"),
                                            wxT("82"),
                                            wxT("83"),
                                            wxT("84"),
                                            wxT("85"),
                                            wxT("86"),
                                            wxT("87"),
                                            wxT("88"),
                                            wxT("89"),
                                            wxT("90"),
                                            wxT("91"),
                                            wxT("92"),
                                            wxT("93"),
                                            wxT("94"),
                                            wxT("95"),
                                            wxT("96"),
                                            wxT("97"),
                                            wxT("98"),
                                            wxT("99"),
                                            wxT("100"),
                                            wxT("101"),
                                            wxT("102"),
                                            wxT("103"),
                                            wxT("104"),
                                            wxT("105"),
                                            wxT("106"),
                                            wxT("107"),
                                            wxT("108"),
                                            wxT("109"),
                                            wxT("110"),
                                            wxT("111"),
                                            wxT("112"),
                                            wxT("113"),
                                            wxT("114"),
                                            wxT("115"),
                                            wxT("116"),
                                            wxT("117"),
                                            wxT("118"),
                                            wxT("119"),
                                            wxT("120"),
                                            wxT("121"),
                                            wxT("122"),
                                            wxT("123"),
                                            wxT("124"),
                                            wxT("125"),
                                            wxT("126"),
                                            wxT("127"),
                                            wxT("128"),
                                            wxT("129"),
                                            wxT("130"),
                                            wxT("131"),
                                            wxT("132"),
                                            wxT("133"),
                                            wxT("134"),
                                            wxT("135"),
                                            wxT("136"),
                                            wxT("137"),
                                            wxT("138"),
                                            wxT("139"),
                                            wxT("140"),
                                            wxT("141"),
                                            wxT("142"),
                                            wxT("143"),
                                            wxT("144"),
                                            wxT("145"),
                                            wxT("146"),
                                            wxT("147"),
                                            wxT("148"),
                                            wxT("149"),
                                            wxT("150"),
                                            wxT("151"),
                                            wxT("152"),
                                            wxT("153"),
                                            wxT("154"),
                                            wxT("155"),
                                            wxT("156"),
                                            wxT("157"),
                                            wxT("158"),
                                            wxT("159"),
                                            wxT("160"),
                                            wxT("161"),
                                            wxT("162"),
                                            wxT("163"),
                                            wxT("164"),
                                            wxT("165"),
                                            wxT("166"),
                                            wxT("167"),
                                            wxT("168"),
                                            // --
                                            nullptr};

const unsigned int arrayClusterGroupCount = 10;

const wxChar* const arrayClusterGroup[] = {wxT("Gas"),
                                           wxT("Hard coal"),
                                           wxT("Lignite"),
                                           wxT("Mixed fuel"),
                                           wxT("Nuclear"),
                                           wxT("Oil"),
                                           wxT("Other"),
                                           wxT("Other 2"),
                                           wxT("Other 3"),
                                           wxT("Other 4"),
                                           nullptr};

const unsigned int arrayRnClusterGroupCount = 9;

const wxChar* const arrayRnClusterGroup[] = {wxT("Wind Onshore"),
                                             wxT("Wind Offshore"),
                                             wxT("Solar Thermal"),
                                             wxT("Solar PV"),
                                             wxT("Solar Rooftop"),
                                             wxT("Other RES 1"),
                                             wxT("Other RES 2"),
                                             wxT("Other RES 3"),
                                             wxT("Other RES 4"),
                                             nullptr};

const unsigned int renewableTSModeCount = 2;

const wxChar* const renewableTSMode[]
  = {wxT("power generation"), wxT("production factor"), nullptr};

const unsigned int thermalLawCount = 2;
const wxChar* const thermalLaws[] = {wxT("uniform"), wxT("geometric"), nullptr};

const unsigned int localGenTSCount = 3;

const wxChar* const localGenTS[]
  = {wxT("Use global parameter"), wxT("Force generation"), wxT("Force no generation"), nullptr};

const wxChar* const weekday[] = {wxT("Monday"),
                                 wxT("Tuesday"),
                                 wxT("Wednesday"),
                                 wxT("Thursday"),
                                 wxT("Friday"),
                                 wxT("Saturday"),
                                 wxT("Sunday"),
                                 nullptr};

const wxChar* const buildingMode[] = {wxT("Automatic"), wxT("Custom"), wxT("Derated"), nullptr};

const wxChar* const playlist[] = {wxT("Automatic"), wxT("Custom"), nullptr};

const wxChar* const geographicTrimming[] = {wxT("None"), wxT("Custom"), nullptr};

const wxChar* const thematicTrimming[] = {wxT("None"), wxT("Custom"), nullptr};

const wxChar* const calendarMonths[] = {wxT("JAN  -  DEC"),
                                        wxT("FEB  -  JAN"),
                                        wxT("MAR  -  FEB"),
                                        wxT("APR  -  MAR"),
                                        wxT("MAY  -  APR"),
                                        wxT("JUN  -  MAY"),
                                        wxT("JUL  -  JUN"),
                                        wxT("AUG  -  JUL"),
                                        wxT("SEP  -  AUG"),
                                        wxT("OCT  -  SEP"),
                                        wxT("NOV  -  OCT"),
                                        wxT("DEC  -  NOV"),
                                        nullptr};

const wxChar* const calendarWeeks[] = {wxT("MON  -  SUN"),
                                       wxT("TUE  -  MON"),
                                       wxT("WED  -  TUE"),
                                       wxT("THU  -  WED"),
                                       wxT("FRI  -  THU"),
                                       wxT("SAT  -  FRI"),
                                       wxT("SUN  -  SAT"),
                                       nullptr};

const wxChar* const studyMode[] = {wxT("Economy"),
                                   wxT("Adequacy"),
                                   wxT("Draft"),
#if STUDY_MODE_EXPANSION
                                   wxT("Expansion"),
#endif
                                   nullptr};

const wxChar* adequacyPatchMode[] = {wxT("virtual area"),
                                     wxT("physical area outside patch"),
                                     wxT("physical area inside patch"),
                                     nullptr};
} // namespace Inspector
} // namespace Window
} // namespace Antares
