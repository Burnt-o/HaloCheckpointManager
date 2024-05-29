#pragma once



struct tagBlock {
	uint32_t entryCount; // how many tagRefs
	uint32_t offset; // to metaHeaderAddress + tagbase
	uint32_t blockDefinition;
};