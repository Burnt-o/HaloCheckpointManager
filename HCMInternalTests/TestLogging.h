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
	TestLogging()
	{
		static plog::UnitTestAppender<plog::TxtFormatter> consoleAppender;
		plog::init(plog::verbose, &consoleAppender);
	}
};