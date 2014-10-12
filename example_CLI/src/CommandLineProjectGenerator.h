#pragma once


#include "Poco/Util/Application.h"
#include "ofxPG.h"


class CommandLineProjectGenerator: public Poco::Util::Application
{
public:
    CommandLineProjectGenerator();
    void initialize(Application& self);
    void uninitialize();
    void reinitialize(Application& self);
    void defineOptions(Poco::Util::OptionSet& options);
    void handleOption(const std::string& name, const std::string& value);

    void printHelp();
    void addPlatforms(const std::string& value);
    void setupForTarget(int targ);

    bool isValidProjectPath(const Poco::Path& path);
    bool isValidOFPath(const Poco::Path& path);

    void recursiveUpdate(const Poco::Path& path);

    void updateProject(const Poco::Path& path);

    int main(const std::vector<std::string>& args);

    enum Mode
    {
        PG_MODE_NONE,
        PG_MODE_CREATE,
        PG_MODE_UPDATE
    };

    std::string directoryForRecursion;
    Poco::Path projectPath;
    Poco::Path ofPath;
    std::vector<std::string> addons;
    std::vector<int> targets;

    bool bHasCommand;   // do we have an "update" or "create" command
    bool bVerbose;  // be verbose
    bool bForce;    // force even if things like ofRoot seem wrong of if update folder looks wonky
    Mode mode;   // what mode are we in?
    bool bRecursive;    // do we recurse in update mode?
    bool bHelpRequested;    // did we request help?
    bool bDryRun;   // do dry run (useful for debugging recursive update)

    std::string target; // the current project target platform in string form (for finding templates)

    std::shared_ptr<ofx::PG::BaseProject> pProject;

};
