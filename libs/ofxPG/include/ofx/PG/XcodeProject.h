#pragma once


#include "ofx/PG/BaseProject.h"


namespace ofx {
namespace PG {


class XcodeProject: public BaseProject
{
public:
    XcodeProject();

    virtual ~XcodeProject();

    void setup();

    void addSrc(const string& srcFile, const string& folder);
    void addInclude(const string& includeName);
    void addLibrary(const string& libraryName, LibType libType);

    void addAddon(ofAddon& addon);

    void saveWorkspaceXML();
    void saveScheme();
    void renameProject();

    std::string srcUUID;
	std::string addonUUID;
	std::string resourcesUUID;
	std::string buildPhaseUUID;
    std::string buildPhaseResourcesUUID;

    pugi::xml_node findOrMakeFolderSet( pugi::xml_node nodeToAddTo,
                                       std::vector<std::string>& folders,
                                       const std::string& pathForHash);

    // where are we inserting items (at the second dict tag,
    // /plist[1]/dict[1]/dict[2])
	pugi::xml_node insertPoint;

    bool findArrayForUUID(const string& UUID, pugi::xml_node& nodeMe);

private:

    bool createProjectFile();
    bool loadProjectFile();
    bool saveProjectFile();
    void saveMakefile();

};


} } // namespace ofx::PG
