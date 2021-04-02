
#include <fstream>
#include <ctime>

#include <wx/string.h>
#include <yuni/io/file/file.h>

#include "on-line-consent.h"
#include <antares/sys/appdata.h>
#include "../message.h"

using namespace Yuni;

namespace Antares
{
onLineConsent::onLineConsent(wxWindow* parent) :
 timeStamp_(0), consent_(0), banned_(0), parent_(parent)
{
    // search for the GDPR configuration file
    GDPR_filename_.clear();

    String localAppData;
    if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
    {
        localAppData.clear();
    }

    if (not localAppData.empty())
    {
        GDPR_filename_ = localAppData;
    }

    GDPR_filename_ << Yuni::IO::Separator << "antares.hwb";
}

bool onLineConsent::checkGDPRStatus()
{
    if (IO::File::Exists(GDPR_filename_))
    {
        // load the GDPR status from file
        std::ifstream ifs;

        ifs.open(GDPR_filename_.to<std::string>(), std::ifstream::in);
        ifs >> timeStamp_;
        ifs >> consent_;
        ifs >> banned_;

        ifs.close();

        if (banned_ == 1)
        {
            timeStamp_ = (long)(std::time(nullptr));
            banned_ = 0;
            consent_ = -1;

            // save the GDPR status to file
            std::ofstream ofs;

            ofs.open(GDPR_filename_.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
            ofs << timeStamp_;
            ofs << "\n";
            ofs << consent_;
            ofs << "\n";
            ofs << banned_;

            ofs.close();
        }
    }
    else
    {
        // show GDPR compliance notice if not found

        wxString msg
          = wxT("By letting us know that our software is now running for a new user, you\n\
can give the Antares_Simulator Team a powerful incentive to keep on further\n\
developing and improving the tool.\n\
\n\
This is why we ask for your consent to the automated communication, through\n\
the internet, of the activation of this copy of Antares_Simulator.\n\
\n\
At any time, You can :\n\
\n\
- Withdraw your consent  with the \"continue off - line\" command\n\
- Renew your consent  with the \"continue on - line\" command\n\
- Display the signature of this copy with the \"show signature\" command\n\
\n\
Click on Continue to express consent");

        Window::Message message(
          parent_,
          wxT(""),
          wxT("Congratulations for the successful installation of Antares_Simulator"),
          msg,
          "images/128x128/antares.png");
        message.add(Window::Message::btnContinue, true);
        message.add(Window::Message::btnCancel);
        // get the timestamp
        timeStamp_ = std::time(nullptr);
        if (message.showModal() == Window::Message::btnCancel)
        {
            consent_ = -1;
        }
        else
        {
            consent_ = 1;
        }

        // save the GDPR status to file
        std::ofstream ofs;

        ofs.open(GDPR_filename_.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
        ofs << timeStamp_;
        ofs << "\n";
        ofs << consent_;
        ofs << "\n";
        ofs << banned_;

        ofs.close();
    }

    return consent_ == 1;
}

void onLineConsent::setGDPRStatus(bool checkOnline)
{
    if (IO::File::Exists(GDPR_filename_))
    {
        std::ifstream ifs;

        ifs.open(GDPR_filename_.to<std::string>(), std::ifstream::in);
        ifs >> timeStamp_;
        ifs >> consent_;
        ifs >> banned_;
        ifs.close();

        if (checkOnline)
        {
            if (banned_ == 1)
            {
                Window::Message message(parent_,
                                        wxT(""),
                                        "Usage metrics",
                                        "Antares_Simulator signature is no longer valid. Please "
                                        "restart to be able to join again",
                                        "images/128x128/antares.png");
                message.add(Window::Message::btnOk, true);
                message.showModal();
            }
            else
            {
                if (consent_ == 1) // display  "  Antares_Simulator is already online (anonymous
                                   // usage metrics)" // nothing to do
                {
                    Window::Message message(
                      parent_,
                      wxT(""),
                      "Usage metrics",
                      "Antares_Simulator is already online (anonymous usage metrics)",
                      "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                }
                else
                {
                    Window::Message message(
                      parent_,
                      wxT(""),
                      "Usage metrics",
                      "Antares_Simulator will start on-line next time (anonymous usage metrics)",
                      "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                    consent_ = 1;
                }
            }
        }
        else
        {
            if (banned_ == 0)
            {
                if (consent_ == -1)
                {
                    Window::Message message(parent_,
                                            wxT(""),
                                            "Usage metrics",
                                            "Antares_Simulator is already off-line",
                                            "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                } // no need to BAN
                else
                {
                    Window::Message message(parent_,
                                            wxT(""),
                                            "Usage metrics",
                                            "Antares_Simulator will continue off-line",
                                            "images/128x128/antares.png");
                    message.add(Window::Message::btnOk, true);
                    message.showModal();
                    banned_ = 1;
                } // wait restart to set CONSENT to -1 and compute new timestamp and hostid
            }
            else // new call of the BAN command by this ui instance, or by another one ; nothing to
                 // do until next restart
            {
                Window::Message message(parent_,
                                        wxT(""),
                                        "Usage metrics",
                                        "Antares_Simulator will stay off-line",
                                        "images/128x128/antares.png");
                message.add(Window::Message::btnOk, true);
                message.showModal();
                banned_ = 1;
            }
        }

        // save the GDPR status to file
        std::ofstream ofs;

        ofs.open(GDPR_filename_.to<std::string>(), std::ifstream::out | std::ofstream::trunc);
        ofs << timeStamp_;
        ofs << "\n";
        ofs << consent_;
        ofs << "\n";
        ofs << banned_;

        ofs.close();
    }
}
} // namespace Antares
