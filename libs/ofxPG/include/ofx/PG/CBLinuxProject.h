#pragma once


#include "ofx/PG/CBWinProject.h"


namespace ofx {
namespace PG {
        


class CBLinuxProject: public CBWinProject
{
public:
	enum Arch
    {
		Linux,
		Linux64,
        LinuxArmv6l,
        LinuxArmv7l
	};

    void setup();

    bool createProjectFile();
    void addInclude(string includeName){};
    void addLibrary(string libraryName, LibType libType = RELEASE_LIB){};

	static const std::string LOG_NAME;

private:
	Arch arch;

};


} } // namespace ofx::PG
