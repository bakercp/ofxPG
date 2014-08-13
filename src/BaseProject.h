#pragma once


#include <set>
#include "ofxPugiXML.h"
#include "ofAddon.h"
#include "Utils.h"


class BaseProject
{
public:
    enum LibType
    {
        DEBUG_LIB = 0,
        RELEASE_LIB
    };

    BaseProject();

    virtual ~BaseProject(){};

    void setup(const std::string& _target);

    bool create(const std::string& path);
    bool save(bool createMakeFile);

    virtual void addSrc(const std::string& srcFile,
                        const std::string& folder) = 0;

    virtual void addInclude(const std::string& includeName) = 0;

    virtual void addLibrary(const std::string& libraryName,
                            LibType libType) = 0;

	virtual void addAddon(ofAddon& addon);

    std::string getName() const;
	std::string getPath() const;

    // this shouldn't be called by anyone.  call "create(...), save" etc

    // virtual void renameProject();
    // this should get called at the end.

protected:
    std::vector<ofAddon> addons;

    std::string projectPath;
    std::string projectName;
    std::string templatePath;
    std::string target;

    pugi::xml_document doc;
    bool bLoaded;

private:
    virtual void setup()=0;
    virtual bool createProjectFile()=0;
    virtual bool loadProjectFile()=0;
    virtual bool saveProjectFile()=0;

    void parseAddons();


};


