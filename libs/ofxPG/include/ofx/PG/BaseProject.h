#pragma once


#include <set>
#include "ofConstants.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofxPugiXML.h"
#include "ofx/PG/ofAddon.h"
#include "ofx/PG/PGUtils.h"


namespace ofx {
namespace PG {


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

    bool create(const Poco::Path& path);
    
    bool save(bool createMakeFile);

    virtual void addSrc(const std::string& srcFile,
                        const std::string& folder) = 0;

    virtual void addInclude(const std::string& includeName) = 0;

    virtual void addLibrary(const std::string& libraryName,
                            LibType libType) = 0;

	virtual void addAddon(ofAddon& addon);

    std::string getName() const;
	Poco::Path getPath() const;

    // this shouldn't be called by anyone.  call "create(...), save" etc

    // virtual void renameProject();
    // this should get called at the end.

protected:
    std::vector<ofAddon> _addons;

    Poco::Path _projectPath;
    std::string _projectName;
    Poco::Path _templatePath;
    std::string _target;

    pugi::xml_document _doc;
    bool _bLoaded;

private:
    virtual void setup()=0;
    virtual bool createProjectFile()=0;
    virtual bool loadProjectFile()=0;
    virtual bool saveProjectFile()=0;

    void parseAddons();

};


} } // namespace ofx::PG
