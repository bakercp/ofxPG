/*
 * CBLinuxProject.h
 *
 *  Created on: 28/12/2011
 *      Author: arturo
 */

#pragma once

#include "ofConstants.h"
#include "pugixml.hpp"
#include "ofAddon.h"
#include "BaseProject.h"


class CBWinProject: virtual public BaseProject
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
