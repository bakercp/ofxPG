#include "ofx/PG/BaseProject.h"


namespace ofx {
namespace PG {
        


BaseProject::BaseProject(): bLoaded(false)
{
}


void BaseProject::setup(const std::string& _target)
{
    target = _target;

    templatePath = PGUtils::getOFRoot();
    templatePath.pushDirectory("scripts");
    templatePath.pushDirectory(target);
    templatePath.pushDirectory("template");

    setup(); // call the inherited class setup(), now that target is set.
}


bool BaseProject::create(const std::string& path)
{
    addons.clear();

    projectPath = ofFilePath::addTrailingSlash(path);
    projectName = ofFilePath::getFileName(path);
    bool bDoesDirExist = false;
    
    ofDirectory project(ofFilePath::join(projectPath,"src"));    // this is a directory, really?
    if (project.exists())
    {
        bDoesDirExist = true;
    }
    else
    {
        Poco::Path templateSrcPath(templatePath);
        templateSrcPath.pushDirectory("src");

        Poco::Path templateBinPath(templatePath);
        templateBinPath.pushDirectory("bin");

        Poco::Path projectSrcPath(projectPath);
        projectSrcPath.pushDirectory("src");

        Poco::Path projectBinPath(projectPath);
        projectBinPath.pushDirectory("bin");

        Poco::File templateSrc(templateSrcPath);
        Poco::File templateBin(templateBinPath);

        templateSrc.copyTo(projectSrcPath.toString());
        templateBin.copyTo(projectBinPath.toString());

//        ofFile::copyFromTo(ofFilePath::join(templatePath,"src"),ofFilePath::join(projectPath,"src"));
//
//        ofFile::copyFromTo(ofFilePath::join(templatePath,"bin"),ofFilePath::join(projectPath,"bin"));
    }

    
    // if overwrite then ask for permission...

    bool ret = createProjectFile();
    if(!ret) return false;
    
    ret = loadProjectFile();
    if(!ret) return false;

    if (bDoesDirExist){
        vector < string > fileNames;
        PGUtils::getFilesRecursively(ofFilePath::join(projectPath , "src"), fileNames);

        for (int i = 0; i < (int)fileNames.size(); i++){

            fileNames[i].erase(fileNames[i].begin(), fileNames[i].begin() + projectPath.length());

            string first, last;
#ifdef TARGET_WIN32
            PGUtils::splitFromLast(fileNames[i], "\\", first, last);
#else
            PGUtils::splitFromLast(fileNames[i], "/", first, last);
#endif
            if (fileNames[i] != "src/ofApp.cpp" &&
                fileNames[i] != "src/ofApp.h" &&
                fileNames[i] != "src/main.cpp" &&
                fileNames[i] != "src/ofApp.mm" &&
                fileNames[i] != "src/main.mm"){
                addSrc(fileNames[i], first);
            }
        }

//		if( target == "ios" ){
//			getFilesRecursively(ofFilePath::join(projectDir , "bin/data"), fileNames);
//
//	        for (int i = 0; i < (int)fileNames.size(); i++){
//				fileNames[i].erase(fileNames[i].begin(), fileNames[i].begin() + projectDir.length());
//
//				string first, last;
//				splitFromLast(fileNames[i], "/", first, last);
//				if (fileNames[i] != "Default.png" &&
//					fileNames[i] != "src/ofApp.h" &&
//					fileNames[i] != "src/main.cpp" &&
//					fileNames[i] != "src/ofApp.mm" &&
//					fileNames[i] != "src/main.mm"){
//					addSrc(fileNames[i], first);
//				}
//			}
//		}

#if defined(TARGET_LINUX) || defined(TARGET_OSX)
    		parseAddons();
#endif
        // get a unique list of the paths that are needed for the includes.
        list < string > paths;
        vector < string > includePaths;
        for (int i = 0; i < (int)fileNames.size(); i++){
            size_t found;
    #ifdef TARGET_WIN32
            found = fileNames[i].find_last_of("\\");
    #else
            found = fileNames[i].find_last_of("/");
    #endif
            paths.push_back(fileNames[i].substr(0,found));
        }

        paths.sort();
        paths.unique();

        for (list<string>::iterator it=paths.begin(); it!=paths.end(); ++it)
        {
            includePaths.push_back(*it);
        }

        for (int i = 0; i < includePaths.size(); i++)
        {
            addInclude(includePaths[i]);
        }
    }
    
    return true;
}

bool BaseProject::save(bool createMakeFile)
{

    // only save an addons.make file if requested on ANY platform
    // this way we don't thrash the git repo for our examples, but
    // we do make the addons.make file for any new projects...that
    // way it can be distributed and re-used by others with the PG

    if(createMakeFile){
        ofFile addonsMake(ofFilePath::join(projectPath,"addons.make"), ofFile::WriteOnly);
        for(int i = 0; i < addons.size(); i++){
            addonsMake << addons[i].name << endl;
        }
    }

	return saveProjectFile();
}

void BaseProject::addAddon(ofAddon& addon)
{

    for (std::size_t i = 0; i < addons.size(); ++i)
    {
		if(addons[i].name == addon.name)
        {
            return;
        }
	}

	addons.push_back(addon);

    for (std::size_t i = 0; i < addon.includePaths.size(); ++i)
    {
        ofLogVerbose() << "adding addon include path: " << addon.includePaths[i];
        addInclude(addon.includePaths[i]);
    }

    for (std::size_t i = 0; i < addon.libs.size(); ++i)
    {
        ofLogVerbose() << "adding addon libs: " << addon.libs[i];
        addLibrary(addon.libs[i], RELEASE_LIB);
    }

    for (std::size_t i = 0; i < addon.srcFiles.size(); ++i)
    {
        ofLogVerbose() << "adding addon srcFiles: " << addon.srcFiles[i];
        addSrc(addon.srcFiles[i],addon.filesToFolders[addon.srcFiles[i]]);
    }
}

void BaseProject::parseAddons()
{
	ofFile addonsMake(ofFilePath::join(projectPath,"addons.make"));
	ofBuffer addonsMakeMem;
	addonsMake >> addonsMakeMem;

	while (!addonsMakeMem.isLastLine())
    {
		ofAddon addon;
		addon.pathToOF = PGUtils::getOFRelPath(projectPath);
		addon.fromFS(ofFilePath::join(ofFilePath::join(PGUtils::getOFRoot().toString(), "addons"),
                                      addonsMakeMem.getNextLine()),
                                      target);
		addAddon(addon);
	}
}


std::string BaseProject::getName() const
{
    return projectName;
}


std::string BaseProject::getPath() const
{
    return projectPath;
}


} } // namespace ofx::PG
