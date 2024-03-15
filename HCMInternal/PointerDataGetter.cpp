#include "pch.h"
#include "PointerDataGetter.h"


namespace PointerDataGetter
{


    std::string getXMLDocument(std::string HCMdirPath)
    {
        constexpr std::string_view xmlFileName = "InternalPointerData.xml";



        auto pointerDataLocation = HCMdirPath + xmlFileName.data();

        std::string downloadFailureMessage = "Download not attempted";

        // RELEASE will try to download the latest version of the pointer data, overwriting the local file.
        // DEBUG skips the download and always uses the local file, since the build events are setup to copy paste the latest version every time; thus immediately reflecting any changes.
#ifndef HCM_DEBUG  
        try
        {
            PLOG_INFO << "Downloading pointer data from Github repo";
            downloadFileTo(githubPath, pointerDataLocation);
            downloadFailureMessage = "Download succeeded!";
        }
        catch (HCMInitException ex)
        {
            ex.prepend("Failed to download pointer data from Github repo. Download error: ");
            downloadFailureMessage = ex.what();

            if (!fileExists(pointerDataLocation)) // no local copy? we're doomed, throw the DL exception
            {
                throw HCMInitException(ex);
            }

        }
#endif

        if (!fileExists(pointerDataLocation)) // no local copy? we're doomed, throw
        {
            throw HCMInitException(std::format("No pointer data existed at location {}", pointerDataLocation));
        }
        else
        {
            PLOG_VERBOSE << "Pointer Data getting contents of file at: " << pointerDataLocation;
        }

        return readFileContents(pointerDataLocation);

    }
}