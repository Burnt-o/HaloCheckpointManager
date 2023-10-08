#pragma once
#include "pch.h"
#include "CppUnitTest.h"

namespace plog
{
    template<class Formatter>
    class UnitTestAppender : public IAppender
    {
    public:
        virtual void write(const Record& record) PLOG_OVERRIDE
        {
            util::nstring str = Formatter::format(record); 

            Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(str.c_str());
        }
    };
}



class TestLogging
{
public:
    static inline bool alreadyInit = false;
	TestLogging()
	{
        if (!alreadyInit)
        {
            alreadyInit = true;
            static plog::UnitTestAppender<plog::TxtFormatter> unitTestAppender;
            plog::init(plog::verbose, &unitTestAppender);
        }

	}


};