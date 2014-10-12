#include "CommandLineProjectGenerator.h"
#include "Poco/Path.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/AutoPtr.h"
#include <iostream>


CommandLineProjectGenerator::CommandLineProjectGenerator():
    mode(PG_MODE_NONE),
    bHasCommand(false),
    bVerbose(false),
    bForce(false),
    bRecursive(false),
    bHelpRequested(false)
{
    // assume our own platform unless it's set:
    targets.push_back( ofGetTargetPlatform() );
}


void CommandLineProjectGenerator::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    Application::initialize(self);
}


void CommandLineProjectGenerator::uninitialize()
{
    Application::uninitialize();
}


void CommandLineProjectGenerator::reinitialize(Application& self)
{
    Application::reinitialize(self);
}


void CommandLineProjectGenerator::defineOptions(Poco::Util::OptionSet& options)
{
    Application::defineOptions(options);

    options.addOption(
                      Poco::Util::Option("recursive", "r", "update recursively (applies only to update)")
                      .required(false)
                      .repeatable(false)
                      .noArgument()
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));


    options.addOption(
                      Poco::Util::Option("help", "h", "")
                      .required(false)
                      .repeatable(false)
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("create", "c", "create a project file if it doesn't exist")
                      .required(false)
                      .noArgument()
                      .repeatable(false)
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("update", "u", "update a project file if it does exist")
                      .required(false)
                      .noArgument()
                      .repeatable(false)
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("platforms", "x", "platform list")
                      .required(false)
                      .repeatable(false)
                      .argument("\"platform list\"")
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));


    options.addOption(
                      Poco::Util::Option("addons", "a", "addons list")
                      .required(false)
                      .repeatable(false)
                      .argument("\"addons list\"")
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("ofPath", "o", "openframeworks path")
                      .required(false)
                      .repeatable(false)
                      .argument("\"OF path\"")
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("projectPath", "p", "project path")
                      .required(false)
                      .repeatable(false)
                      .argument("\"project path\"")
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
    options.addOption(
                      Poco::Util::Option("verbose", "v", "run verbose")
                      .required(false)
                      .repeatable(false)
                      .noArgument()
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));

    options.addOption(
                      Poco::Util::Option("dryrun", "d", "don't change files")
                      .required(false)
                      .repeatable(false)
                      .noArgument()
                      .callback(Poco::Util::OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));


}

void CommandLineProjectGenerator::handleOption(const std::string& name,
                                               const std::string& value)
{
    if (name == "help")
    {
        printHelp();
    }
    else if (name == "verbose")
    {
        bVerbose = true;
    }
    else if (name == "create")
    {
        mode = PG_MODE_CREATE;
    }
    else if (name == "update")
    {
        mode = PG_MODE_UPDATE;
    }
    else if (name == "platforms")
    {
        addPlatforms(value);
    }
    else if (name == "addons")
    {
        addons = ofSplitString(value, ",", true, true);
    }
    else if (name == "ofPath")
    {
        ofPath = Poco::Path::forDirectory(value);
    }
    else if (name == "projectPath")
    {
        projectPath = Poco::Path::forDirectory(value);
    }
    else if (name == "recursive")
    {
        bRecursive = true;
    }
    else if (name == "dryrun")
    {
        bDryRun = true;
    }
}


void CommandLineProjectGenerator::printHelp()
{
    bHelpRequested = true;
    Poco::Util::HelpFormatter helpFormatter(options());
    std::stringstream header;

    header << std::endl;
    header << "OPTIONS:" << std::endl;
    header << "A command line project generator" << std::endl;;
    header << "please use a command \"create\", \"update\""<< std::endl;
    header << "lists should be comma seperated" << std::endl;
    header << "and in quotes if there are spaces" << std::endl;;
    header << "you can use : or = for parameter based options, such as -o=/usr/..." << std::endl;;

    helpFormatter.setHeader(header.str());
    helpFormatter.setFooter("\n");
    helpFormatter.format(std::cout);

    stopOptionsProcessing();
}


void CommandLineProjectGenerator::addPlatforms(const std::string& value)
{
    targets.clear();

    std::vector<std::string> platforms = ofSplitString(value, ",", true, true);

    for (std::size_t i = 0; i < platforms.size(); ++i)
    {
        if (platforms[i] == "linux")
        {
            targets.push_back(OF_TARGET_LINUX);
        }
        else if (platforms[i] == "linux64")
        {
            targets.push_back(OF_TARGET_LINUX64);
        }
        else if (platforms[i] == "linuxarmv6l")
        {
            targets.push_back(OF_TARGET_LINUXARMV6L);
        }
        else if (platforms[i] == "linuxarmv7l")
        {
            targets.push_back(OF_TARGET_LINUXARMV7L);
        }
        else if (platforms[i] == "win_cb")
        {
            targets.push_back(OF_TARGET_WINGCC);
        }
        else if (platforms[i] == "vs")
        {
            targets.push_back(OF_TARGET_WINVS);
        }
        else if (platforms[i] == "osx")
        {
            targets.push_back(OF_TARGET_OSX);
        }
        else if(platforms[i] == "ios")
        {
            targets.push_back(OF_TARGET_IOS);
        }
        else if(platforms[i] == "android")
        {
            ofLogError() << "android platform not supported yet" << endl;
            std::exit(1);
        }
        else if(platforms[i] == "allplatforms")
        {
            targets.push_back(OF_TARGET_LINUX);
            targets.push_back(OF_TARGET_LINUX64);
            targets.push_back(OF_TARGET_LINUXARMV6L);
            targets.push_back(OF_TARGET_LINUXARMV7L);
            targets.push_back(OF_TARGET_WINGCC);
            targets.push_back(OF_TARGET_WINVS);
            targets.push_back(OF_TARGET_OSX);
            targets.push_back(OF_TARGET_IOS);
        }
    }
}


void CommandLineProjectGenerator::setupForTarget(int targ)
{
    if (pProject)
    {
        pProject.reset();
    }

    switch(targ)
    {
        case OF_TARGET_OSX:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::XcodeProject);
            target = "osx";
            break;
        case OF_TARGET_WINGCC:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::CBWinProject);
            target = "win_cb";
            break;
        case OF_TARGET_WINVS:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::VisualStudioProject);
            target = "vs";
            break;
        case OF_TARGET_IPHONE:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::XcodeProject);
            target = "ios";
            break;
        case OF_TARGET_ANDROID:
            break;
        case OF_TARGET_LINUX:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::CBLinuxProject);
            target = "linux";
            break;
        case OF_TARGET_LINUX64:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::CBLinuxProject);
            target = "linux64";
            break;
        case OF_TARGET_LINUXARMV6L:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::CBLinuxProject);
            target = "linuxarmv6l";
            break;
        case OF_TARGET_LINUXARMV7L:
            pProject = std::shared_ptr<ofx::PG::BaseProject>(new ofx::PG::CBLinuxProject);
            target = "linuxarmv7l";
            break;
    }
}

bool CommandLineProjectGenerator::isValidProjectPath(const Poco::Path& path)
{
    Poco::Path projectSourcePath(path, "src/");

    ofLogVerbose("CommandLineProjectGenerator::isValidProjectPath") << "Checking: " << path.toString();
    ofLogVerbose("CommandLineProjectGenerator::isValidProjectPath") << "Checking: " << projectSourcePath.toString();

    return Poco::File(projectSourcePath).exists();
}

bool CommandLineProjectGenerator::isValidOFPath(const Poco::Path& path)
{
    Poco::Path templatesPath(path);
    templatesPath.pushDirectory("scripts");

    if (!Poco::File(templatesPath).exists())
    {
        ofLogError("CommandLineProjectGenerator::isValidOFPath") << "ofPath has no scripts directory: " << path.toString();
        return false;
    }
    else
    {
        return true;
    }
}

void CommandLineProjectGenerator::recursiveUpdate(const Poco::Path& path)
{
    Poco::File projectDirectory(path);

    if (!projectDirectory.exists() || !projectDirectory.isDirectory())
    {
        return false;
    }

    if (isValidProjectPath(path))
    {
        updateProject(path);
        return;
    }

    std::vector<Poco::File> files;

    projectDirectory.list(files);

    for (std::size_t i = 0; i < files.size(); ++i)
    {
        if (files[i].isDirectory())
        {
            recursiveUpdate(files[i].path());
        }
    }
}


void CommandLineProjectGenerator::updateProject(const Poco::Path& path)
{
    if (!pProject)
    {
        ofLogFatalError("CommandLineProjectGenerator::updateProject") << "Project pointer is not defined!";
        return;
    }

    if (!bDryRun)
    {
        pProject->setup(target);
    }

    if (!bDryRun)
    {
        pProject->create(path.toString());
    }

    std::vector<std::string> addons;

    Poco::Path addonsMakePath(path, "addons.make");

    Poco::File addonsMakeFile(addonsMakePath);

    if (addonsMakeFile.exists())
    {
        ofx::PG::PGUtils::parseAddonsDotMake(addonsMakePath.toString(), addons);
    }

    for (std::size_t i = 0; i < addons.size(); ++i)
    {
        ofx::PG::ofAddon addon;

        addon.pathToOF = ofx::PG::PGUtils::getOFRelPath(path.toString()).toString();

        Poco::Path addonsPath(ofx::PG::PGUtils::getOFRoot());
        
        addonsPath.pushDirectory("addons");

        Poco::Path addonPath(addonsPath, Poco::Path::forDirectory(addons[i]));

        addon.fromFS(addonsPath.toString(), target);

        ofLogNotice() << "parsing addon " << addonPath.toString();

        if (!bDryRun)
        {
            pProject->addAddon(addon);
        }
    }

    if (!bDryRun)
    {
        pProject->save(false);
    }

}


int CommandLineProjectGenerator::main(const std::vector<std::string>& args)
{
    if (bVerbose)
    {
        ofSetLogLevel(OF_LOG_VERBOSE);
    }

    if (bHelpRequested)
    {
        return Application::EXIT_OK;
    }

    if (PG_MODE_NONE == mode)
    {
        ofLogWarning() << "No mode selected.  Please use create or update.";
        printHelp();
        return Application::EXIT_OK;
    }

    if (ofPath.toString().empty())
    {
        ofLogWarning() << "Empty OF path.  Please use -o or -ofPath.";
        printHelp();
        return Application::EXIT_OK;
    }
    else
    {
        ofPath = Poco::Path(Poco::Path::current()).resolve(ofPath);

        ofLogVerbose() << "ofPath: " << ofPath.toString();

        if (!isValidOFPath(ofPath))
        {
            return Application::EXIT_OK;
        }

        ofLogNotice() << "Setting OF path to: " << ofPath.toString();

        ofx::PG::PGUtils::setOFRoot(ofPath.toString());
    }

    if (projectPath.toString().empty())
    {
        ofLogWarning() << "Empty project path.  Please use -p or -projectPath.";
        printHelp();
        return Application::EXIT_OK;
    }
    else
    {
        projectPath = Poco::Path(Poco::Path::current()).resolve(projectPath);

        ofLogVerbose() << "Project Path: " << projectPath.toString();

        if (!isValidProjectPath(projectPath))
        {
            ofLogWarning() << "Invalid project path: " << projectPath.toString();
            return Application::EXIT_OK;
        }
    }


    if (mode == PG_MODE_CREATE)
    {
        for (std::size_t i = 0; i < targets.size(); ++i)
        {
            setupForTarget(targets[i]);

            ofLogNotice() << "setting up a new project";
            ofLogNotice() << "target platform is: " << target;
            ofLogNotice() << "project path is: " << projectPath.toString();

            if (!bDryRun)
                pProject->setup(target);

            if (!bDryRun)
                pProject->create(projectPath.toString());

            for (std::size_t j = 0; j < addons.size(); ++j)
            {
                ofx::PG::ofAddon addon;

                ofLogNotice() << "parsing addon: " << ofFilePath::join(ofx::PG::PGUtils::getOFRoot().toString(), "addons");

                if (!bDryRun) addon.fromFS(ofFilePath::join(ofFilePath::join(ofx::PG::PGUtils::getOFRoot().toString(), "addons"), addons[j]),target);
                if (!bDryRun) pProject->addAddon(addon);
            }

            if (!bDryRun) pProject->save(false);
        }
    }
    else if (mode == PG_MODE_UPDATE)
    {
        if (!bRecursive)
        {
            if (isValidProjectPath(projectPath) || bForce )
            {
                for(std::size_t i = 0; i < targets.size(); ++i)
                {
                    ofLogVerbose("CommandLineProjectGenerator::main") << "Setting up for target: " << targets[i];
                    setupForTarget(targets[i]);
                    ofLogVerbose("CommandLineProjectGenerator::main") << "Target now: " << target;

                    ofLogVerbose("CommandLineProjectGenerator::main") << "Updating project: " << projectPath.toString();
                    updateProject(projectPath);
                }
            }
            else
            {
                ofLogError("CommandLineProjectGenerator::main") << "There is no src/ folder in this project path to update.  Maybe use create instead? (or use force to force updating).";
            }
        }
//        else
//        {
//            for (std::size_t i = 0; i < targets.size(); ++i)
//            {
//                setupForTarget(targets[i]);
//                recursiveUpdate(projectPath);
//            }
//        }


    }

    return Application::EXIT_OK;
}
