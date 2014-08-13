#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "ofAppNoWindow.h"

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/AutoPtr.h"
#include <iostream>

#include "CBLinuxProject.h"
#include "CBWinProject.h"
#include "VisualStudioProject.h"
#include "XcodeProject.h"
#include <Poco/Path.h>


using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;
using Poco::Path;


/*
 
    todo: 
    
    - commands like what chrisopher suggests
    - really check OF root
    - check if folder exits....
 
 */

class CommandLineProjectGenerator: public Poco::Util::Application
{
public:
    enum pgMode
    {
        PG_MODE_NONE,
        PG_MODE_CREATE,
        PG_MODE_UPDATE
    };
    
    
    std::string              directoryForRecursion;
    std::string              projectPath;
    std::string              ofPath;
    std::vector<std::string>     addons;
    std::vector<int>        targets;
    
    bool bHasCommand;                       // do we have an "update" or "create" command
    bool bVerbose;                          // be verbose
    bool bForce;                            // force even if things like ofRoot seem wrong of if update folder looks wonky
    int mode;                               // what mode are we in?
    bool bRecursive;                        // do we recurse in update mode?
    bool bHelpRequested;                    // did we request help?
    bool bDryRun;                           // do dry run (useful for debugging recursive update)
    
    std::string target; // the current project target platform in string form (for finding templates)
    BaseProject* project;
    
	CommandLineProjectGenerator():
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
    
	void initialize(Application& self)
    {
		loadConfiguration(); // load default configuration files, if present
		Application::initialize(self);
	}
	
	void uninitialize()
    {
		Application::uninitialize();
	}
	
	void reinitialize(Application& self)
    {
		Application::reinitialize(self);
	}
	
	void defineOptions(OptionSet& options)
    {
		Application::defineOptions(options);

        
        options.addOption(
                          Option("recursive", "r", "update recursively (applies only to update)")
                          .required(false)
                          .repeatable(false)
                          .noArgument()
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        

		options.addOption(
                          Option("help", "h", "")
                          .required(false)
                          .repeatable(false)
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        options.addOption(
                          Option("create", "c", "create a project file if it doesn't exist")
                          .required(false)
                          .noArgument()
                          .repeatable(false)
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        options.addOption(
                          Option("update", "u", "update a project file if it does exist")
                          .required(false)
                          .noArgument()
                          .repeatable(false)
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        options.addOption(
                          Option("platforms", "x", "platform list")
                          .required(false)
                          .repeatable(false)
                          .argument("\"platform list\"")
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        
        options.addOption(
                          Option("addons", "a", "addons list")
                          .required(false)
                          .repeatable(false)
                          .argument("\"addons list\"")
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
		options.addOption(
                          Option("ofPath", "o", "openframeworks path")
                          .required(false)
                          .repeatable(false)
                          .argument("\"OF path\"")
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
		options.addOption(
                          Option("projectPath", "p", "project path")
                          .required(false)
                          .repeatable(false)
                          .argument("\"project path\"")
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        options.addOption(
                          Option("verbose", "v", "run verbose")
                          .required(false)
                          .repeatable(false)
                          .noArgument()
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        options.addOption(
                          Option("dryrun", "d", "don't change files")
                          .required(false)
                          .repeatable(false)
                          .noArgument()
                          .callback(OptionCallback<CommandLineProjectGenerator>(this, &CommandLineProjectGenerator::handleOption)));
        
        
	}
    
    void handleOption(const std::string& name, const std::string& value)
    {
        if (name == "help"){
            printHelp();
        } else if (name == "create"){
            mode = PG_MODE_CREATE;
        } else if (name == "update"){
            mode = PG_MODE_UPDATE;
        } else if (name == "platforms"){
            addPlatforms(value);
        } else if (name == "addons"){
            addons = ofSplitString(value, ",", true, true);
        } else if (name == "ofPath"){
            ofPath = value;
        } else if (name == "projectPath"){
            projectPath = value;
        } else if (name == "recursive"){
            bRecursive = true;
        } else if (name == "dryrun"){
            bDryRun = true;
        }
    }
    

	void printHelp()
    {
        bHelpRequested = true;
        HelpFormatter helpFormatter(options());
        string header = "";
        header += "\nOPTIONS:\n";
        header += "A command line project generator\n";
        header += "please use a command \"create\", \"update\"\n";
        header += "lists should be comma seperated\nand in quotes if there are spaces\n";
        header += "you can use : or = for parameter based options, such as -o=/usr/...";
        helpFormatter.setHeader(header);
        helpFormatter.setFooter("\n");
        helpFormatter.format(std::cout);
		stopOptionsProcessing();
	}
    

    
    void addPlatforms(const std::string& value)
    {
        targets.clear();
        vector < string > platforms = ofSplitString(value, ",", true, true);
        
        for (int i = 0; i < platforms.size(); i++){
            
            if( platforms[i]=="linux"){
                targets.push_back( OF_TARGET_LINUX );
            }else if(platforms[i]=="linux64"){
                targets.push_back( OF_TARGET_LINUX64 );
            }else if(platforms[i]=="linuxarmv6l"){
                targets.push_back( OF_TARGET_LINUXARMV6L );
            }else if(platforms[i]=="linuxarmv7l"){
                targets.push_back( OF_TARGET_LINUXARMV7L );
            }else if(platforms[i]=="win_cb"){
                targets.push_back( OF_TARGET_WINGCC );
            }else if(platforms[i]=="vs"){
                targets.push_back( OF_TARGET_WINVS );
            }else if(platforms[i]=="osx"){
                targets.push_back( OF_TARGET_OSX );
            }else if(platforms[i]=="ios"){
                targets.push_back( OF_TARGET_IPHONE );
            }else if(platforms[i]=="android"){
                ofLogError() << "android platform not supported yet" << endl;
                std::exit(1);
            }else if(platforms[i]=="allplatforms"){
                targets.push_back( OF_TARGET_LINUX );
                targets.push_back( OF_TARGET_LINUX64 );
                targets.push_back( OF_TARGET_LINUXARMV6L );
                targets.push_back( OF_TARGET_LINUXARMV7L );
                targets.push_back( OF_TARGET_WINGCC );
                targets.push_back( OF_TARGET_WINVS );
                targets.push_back( OF_TARGET_OSX );
                targets.push_back( OF_TARGET_IPHONE );
            }
        }
	}
	
    
    void setupForTarget(int targ){
        
        if(project){
            delete project;
        }
        
        switch(targ){
            case OF_TARGET_OSX:
                project = new XcodeProject;
                target = "osx";
                break;
            case OF_TARGET_WINGCC:
                project = new CBWinProject;
                target = "win_cb";
                break;
            case OF_TARGET_WINVS:
                project = new VisualStudioProject;
                target = "vs";
                break;
            case OF_TARGET_IPHONE:
                project = new XcodeProject;
                target = "ios";
                break;
            case OF_TARGET_ANDROID:
                break;
            case OF_TARGET_LINUX:
                project = new CBLinuxProject;
                target = "linux";
                break;
            case OF_TARGET_LINUX64:
                project = new CBLinuxProject;
                target = "linux64";
                break;
            case OF_TARGET_LINUXARMV6L:
                project = new CBLinuxProject;
                target = "linuxarmv6l";
                break;
            case OF_TARGET_LINUXARMV7L:
                project = new CBLinuxProject;
                target = "linuxarmv7l";
                break;
        }
    }

	
	void handleOFPath(const std::string& name, const std::string& value){
		
        cout << "setting root of OF to " << value << endl;
        
        setOFRoot(value);
	} 
    
    void handleProjectPath(const std::string& name, const std::string& value){
		
        cout << "setting project path to " << value << endl;
        
        projectPath = value;
	}
    
    void handleAddons(const std::string& name, const std::string& value){
		
        addons = ofSplitString(value, ",", true, true);
    }
    
    
    bool isGoodProjectPath (string path ){
        
        ofDirectory dir(path);
        if (!dir.isDirectory()){
            return false;
        }
        
        dir.listDir();
        bool bHasSrc = false;
        for (int i = 0; i < dir.size(); i++){
            if (dir.getName(i) == "src"){
                bHasSrc = true;
            }
        }
        
        if (bHasSrc == true){
            return true;
        } else {
            return false;
        }
        
        
    }

    bool isGoodOFPath ( string path ){
        
        ofDirectory dir(path);
        if (!dir.isDirectory()){
            ofLog(OF_LOG_ERROR) << "ofPath seems wrong... not a directory";
            return false;
        }
        
        dir.listDir();
        bool bHasTemplates = false;
        for (int i = 0; i < dir.size(); i++){
            if (dir.getName(i) == "scripts"){
                bHasTemplates = true;
            }
        }
        
        if (bHasTemplates == true){
            return true;
        } else {
            ofLog(OF_LOG_ERROR) << "ofPath seems wrong... no scripts / templates directory";
            return false;
        }

    }
    
    void recursiveUpdate( string path ) {
        
        ofDirectory dir(path);
        
        
        // first, bail if it's just a file
        if (dir.isDirectory() == false) return;
        
        // second check if this is a folder that has src in it
        if (isGoodProjectPath(path)){
            updateProject(path);
            return;
        }
        
        // finally, recursively look at this
        dir.listDir();
        for (int i = 0; i < dir.size(); i++){
            ofDirectory subDir(dir.getPath(i));
            if (subDir.isDirectory()){
                recursiveUpdate(dir.getPath(i));
            }
        }
        
        
        
    }
    
    
    void updateProject ( string path ){
        
        ofLog(OF_LOG_NOTICE) << "updating project " << path;
        
        if (!bDryRun) project->setup(target);
        if (!bDryRun) project->create(path);
        
        vector < string > addons;
        addons.clear();
        ofFile file(path+ "addons.make");
        
        if (file.exists()){
            parseAddonsDotMake(path + "addons.make", addons);
        }
        
        for (int i = 0; i < (int)addons.size(); i++){
            ofAddon addon;
            addon.pathToOF = getOFRelPath(path);
            addon.fromFS(ofFilePath::join(ofFilePath::join(getOFRoot(), "addons"), addons[i]),target);
            
            ofLog(OF_LOG_NOTICE)  << "parsing addon " << ofFilePath::join(getOFRoot(), "addons");
            
            if (!bDryRun) project->addAddon(addon);
        }
        if (!bDryRun) project->save(false);
    }
	
    
	int main(const std::vector<std::string>& args){
 
        cout << endl;
        
          if (bHelpRequested){
            return Application::EXIT_OK;
        }
//
        
        
        if (mode == PG_MODE_NONE){
            ofLog(OF_LOG_WARNING) << endl << "no mode selected... please use create or update" << endl;
            printHelp();
            return Application::EXIT_OK;
        }


        // check things
        
        if (ofPath == ""){
            ofLog(OF_LOG_NOTICE) << endl << "no OF path set... please use -o or -ofPath" << endl;
            printHelp();
            return Application::EXIT_OK;
        } else {
            
            // let's try to resolve this path vs the current path
            // so things like ../ can work
            // see http://www.appinf.com/docs/poco/Poco.Path.html
        
            Path cwd    = Path::current();                  // get the current path
            ofPath      = cwd.resolve(ofPath).toString();   // resolve ofPath vs that.
            Path resolvedPath = Path(ofPath).absolute();    // make that new path absolute
            ofPath = resolvedPath.toString();
            
            if (!isGoodOFPath(ofPath)){
                 return Application::EXIT_OK;
            }
            
            ofLog(OF_LOG_NOTICE) << "setting OF path to: " << ofPath;
            setOFRoot(ofPath);
        }
        
        // todo : check ofPath
        
        
        if (projectPath == ""){
            ofLog(OF_LOG_WARNING) << endl << "no project path set... please use -p or -projectPath" << endl;
            printHelp();
            return Application::EXIT_OK;
        } else {
            
            Path cwd            = Path::current();                      // get the current path
            projectPath         = cwd.resolve(projectPath).toString();  // resolve projectPath vs that.
            Path resolvedPath   = Path(projectPath).absolute();         // use absolute version of this path
            projectPath = resolvedPath.toString();
            
            if (!isGoodOFPath(ofPath)){
                return Application::EXIT_OK;
            }

            
            
        }
    
        
        if (mode == PG_MODE_CREATE){
            
            for(int i = 0; i < (int)targets.size(); i++){
                setupForTarget(targets[i]);
                
                ofLog(OF_LOG_NOTICE) << "setting up a new project";
                ofLog(OF_LOG_NOTICE) << "target platform is: " << target;
                ofLog(OF_LOG_NOTICE) << "project path is: " << projectPath;
                
                if (!bDryRun) project->setup(target);
                if (!bDryRun) project->create(projectPath);
                
                for (int j = 0; j < (int)addons.size(); j++){
                    
                    ofAddon addon;
                    
                    ofLog(OF_LOG_NOTICE) << "parsing addon: " << ofFilePath::join(getOFRoot(), "addons");
                    
                    if (!bDryRun) addon.fromFS(ofFilePath::join(ofFilePath::join(getOFRoot(), "addons"), addons[j]),target);
                    if (!bDryRun) project->addAddon(addon);
                }
                if (!bDryRun) project->save(false);
            }
        } else if (mode == PG_MODE_UPDATE){
            
            if (!bRecursive){
                if (isGoodProjectPath(projectPath) || bForce ){
                    for(int i = 0; i < (int)targets.size(); i++){
                        setupForTarget(targets[i]);
                        updateProject ( projectPath );
                    }
                } else {
                    ofLog(OF_LOG_ERROR) << "there's no src folder in this project path to update, maybe use create instead? (or use force to force updating)";
                }
            } else {
                for(int i = 0; i < (int)targets.size(); i++){
                    setupForTarget(targets[i]);
                    recursiveUpdate(projectPath);
                }
            }
            
            
        }

        cout << endl;
        
        return Application::EXIT_OK;
	}

};








POCO_APP_MAIN(CommandLineProjectGenerator)
