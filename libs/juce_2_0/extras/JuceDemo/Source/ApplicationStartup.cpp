/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "jucedemo_headers.h"
#include "MainDemoWindow.h"


//==============================================================================
class JUCEDemoApplication : public JUCEApplication
{
public:
    //==============================================================================
    JUCEDemoApplication()
    {
    }

    ~JUCEDemoApplication()
    {
    }

    //==============================================================================
    void initialise (const String& /*commandLine*/)
    {
       #if JUCE_IOS || JUCE_ANDROID
        theMainWindow.setVisible (true);
        theMainWindow.setFullScreen (true);
       #else
        theMainWindow.centreWithSize (700, 600);
        theMainWindow.setVisible (true);
       #endif

        // this little function just demonstrates a few system info calls
        Logger::outputDebugString (collectSomeSystemInfo());

        /*  on return from this method, the app will go into its the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().

            In this case, JUCEAppliction::quit() will be called by the
            demo window when the user clicks on its close button.
        */
    }

    void shutdown()
    {
        // This method is where your app should do any cleaning-up that's needed
        // before being shut down.
    }

    //==============================================================================
    const String getApplicationName()
    {
        // When you use the Jucer to auto-generate a project, it puts the project's name and version in
        // this constant, so we can use that here as our return value. Alternatively you can return
        // your own string here, of course.
        return ProjectInfo::projectName;
    }

    const String getApplicationVersion()
    {
        // When you use the Jucer to auto-generate a project, it puts the project's name and version in
        // this constant, so we can use that here as our return value. Alternatively you can return
        // your own string here, of course.
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted (const String& /*commandLine*/)
    {
        // This will get called if the user launches another copy of the app, but
        // there's nothing that the demo app needs to do here.
    }

private:
    // This is the main demo window component.
    MainDemoWindow theMainWindow;

    //==============================================================================
    // this little function just demonstrates a few system info calls
    static String collectSomeSystemInfo()
    {
        String systemInfo;

        systemInfo
          << "Time and date: " << Time::getCurrentTime().toString (true, true) << newLine

          << "User logon name: "  << SystemStats::getLogonName() << newLine
          << "Full user name: "   << SystemStats::getFullUserName() << newLine
          << "Host name: "        << SystemStats::getComputerName() << newLine
          << "Operating system: " << SystemStats::getOperatingSystemName() << newLine

          << "Memory size: "    << SystemStats::getMemorySizeInMegabytes() << "MB" << newLine
          << "Number of CPUs: " << SystemStats::getNumCpus() << newLine
          << "CPU vendor: "     << SystemStats::getCpuVendor() << newLine
          << "CPU speed: "      << SystemStats::getCpuSpeedInMegaherz() << "MHz" << newLine
          << "CPU has MMX: "    << (SystemStats::hasMMX()   ? "yes" : "no") << newLine
          << "CPU has SSE: "    << (SystemStats::hasSSE()   ? "yes" : "no") << newLine
          << "CPU has SSE2: "   << (SystemStats::hasSSE2()  ? "yes" : "no") << newLine
          << "CPU has 3DNOW: "  << (SystemStats::has3DNow() ? "yes" : "no") << newLine

          << "Found network card MAC addresses: " << getMacAddressList() << newLine

          << "Current working directory: "         << File::getCurrentWorkingDirectory().getFullPathName() << newLine
          << "Current executable file: "           << File::getSpecialLocation (File::currentExecutableFile).getFullPathName() << newLine
          << "Current application file: "          << File::getSpecialLocation (File::currentApplicationFile).getFullPathName() << newLine
          << "User home directory: "               << File::getSpecialLocation (File::userHomeDirectory).getFullPathName() << newLine
          << "User documents directory: "          << File::getSpecialLocation (File::userDocumentsDirectory).getFullPathName() << newLine
          << "User application data directory: "   << File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName() << newLine
          << "Common application data directory: " << File::getSpecialLocation (File::commonApplicationDataDirectory).getFullPathName() << newLine
          << "Temp directory: "                    << File::getSpecialLocation (File::tempDirectory).getFullPathName() << newLine
          << newLine;

        return systemInfo;
    }

    static String getMacAddressList()
    {
        Array <MACAddress> macAddresses;
        MACAddress::findAllAddresses (macAddresses);

        StringArray addressStrings;
        for (int i = 0; i < macAddresses.size(); ++i)
            addressStrings.add (macAddresses[i].toString());

        return addressStrings.joinIntoString (", ");
    }
};


//==============================================================================
/*
    This macro creates the application's main() function..
*/
START_JUCE_APPLICATION (JUCEDemoApplication)
