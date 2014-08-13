#pragma once


#include "ofx/PG/BaseProject.h"


namespace ofx {
namespace PG {


class CBWinProject: public BaseProject
{
public:
    void setup();

    bool createProjectFile();
    bool loadProjectFile();
    bool saveProjectFile();

	void addSrc(const std::string& srcName, const std::string& folder);
	void addInclude(const std::string& includeName);
	void addLibrary(const std::string& libraryName, LibType libType = RELEASE_LIB);

	static const std::string LOG_NAME;

};


} } // namespace ofx::PG
