#pragma once
#include "pch.h"
#include "ExternalInfo.h"


class CheckpointInjectionLogger
{
private:
	class CheckpointInjectionLoggerImpl;
	std::unique_ptr<CheckpointInjectionLoggerImpl> pimpl;

public:
	CheckpointInjectionLogger(std::string dirPath);
	~CheckpointInjectionLogger();

	void logInjection(SelectedCheckpointData& data);
};