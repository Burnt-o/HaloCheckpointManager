#pragma once
struct DirPathContainer
{
    std::string dirPath;
    DirPathContainer() = delete;
    DirPathContainer(std::string d) : dirPath(d) {}
};
