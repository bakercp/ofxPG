#include "ofx/PG/BaseProject.h"
#include "ofx/IO/DirectoryUtils.h"
#include "ofx/IO/FileExtensionFilter.h"


namespace ofx {
namespace PG {
        


BaseProject::BaseProject(): _bLoaded(false)
{
}


void BaseProject::setup(const std::string& target)
{
    ofLogVerbose("BaseProject::setup") << "Setting up project target: " << _target;
    _target = target;

    _templatePath = PGUtils::getOFRoot();
    _templatePath.pushDirectory("scripts");
    _templatePath.pushDirectory(target);
    _templatePath.pushDirectory("template");

    ofLogVerbose("BaseProject::setup") << "Using template path: " << _templatePath.toString();

    setup(); // call the inherited class setup(), now that target is set.
}


bool BaseProject::create(const Poco::Path& path)
{
    _projectPath = path;

    ofLogVerbose("BaseProject::setup") << "Creating project: " << _projectPath.toString();

    _addons.clear();

    _projectName = path.directory(path.depth() - 1);

    ofLogVerbose("BaseProject::setup") << "Project Name: " << _projectName;

    bool bDoesDirExist = false;

    Poco::Path sourcePath(_projectPath, "src/");
    Poco::File sourceDirectory(sourcePath);

    if (sourceDirectory.exists())
    {
        bDoesDirExist = true;
    }
    else
    {
        Poco::Path templateSrcPath(_templatePath);
        templateSrcPath.pushDirectory("src");

        Poco::Path templateBinPath(_templatePath);
        templateBinPath.pushDirectory("bin");

        Poco::Path projectSrcPath(_projectPath);
        projectSrcPath.pushDirectory("src");

        Poco::Path projectBinPath(_projectPath);
        projectBinPath.pushDirectory("bin");

        Poco::File templateSrc(templateSrcPath);
        Poco::File templateBin(templateBinPath);

        templateSrc.copyTo(projectSrcPath.toString());
        templateBin.copyTo(projectBinPath.toString());
    }

    
    // if overwrite then ask for permission...

    bool ret = createProjectFile();

    if(!createProjectFile())
        return false;
    
    if(!loadProjectFile())
        return false;

    if (bDoesDirExist)
    {
        std::vector<std::string> fileNames;

        IO::FileExtensionFilter fileExtensionFilter;

        fileExtensionFilter.addExtension("c");
        fileExtensionFilter.addExtension("cpp");
        fileExtensionFilter.addExtension("hpp");
        fileExtensionFilter.addExtension("h");
        fileExtensionFilter.addExtension("m");
        fileExtensionFilter.addExtension("mm");

        Poco::File sourceDirectory(sourcePath);


        IO::DirectoryUtils::listRecursive(sourcePath.toString(),
                                          fileNames,
                                          true,
                                          &fileExtensionFilter);

        for (std::size_t i = 0; i < fileNames.size(); ++i)
        {

            fileNames[i].erase(fileNames[i].begin(),
                               fileNames[i].begin() + _projectPath.toString().length());

            std::string first;
            std::string last;

#ifdef TARGET_WIN32
            PGUtils::splitFromLast(fileNames[i], "\\", first, last);
#else
            PGUtils::splitFromLast(fileNames[i], "/", first, last);
#endif
            if (fileNames[i] != "src/ofApp.cpp" &&
                fileNames[i] != "src/ofApp.h" &&
                fileNames[i] != "src/main.cpp" &&
                fileNames[i] != "src/ofApp.mm" &&
                fileNames[i] != "src/main.mm")
            {
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
        std::list<std::string> paths;
        std::vector<std::string> includePaths;

        for (std::size_t i = 0; i < fileNames.size(); ++i)
        {
            std::size_t found;
    #ifdef TARGET_WIN32
            found = fileNames[i].find_last_of("\\");
    #else
            found = fileNames[i].find_last_of("/");
    #endif
            paths.push_back(fileNames[i].substr(0,found));
        }

        paths.sort();
        paths.unique();

        for (std::list<std::string>::iterator it=paths.begin(); it != paths.end(); ++it)
        {
            includePaths.push_back(*it);
        }

        for (std::size_t i = 0; i < includePaths.size(); ++i)
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

    if (createMakeFile)
    {
        ofFile addonsMake(ofFilePath::join(_projectPath.toString(), "addons.make"),
                          ofFile::WriteOnly);

        for(std::size_t i = 0; i < _addons.size(); ++i)
        {
            addonsMake << _addons[i].name << endl;
        }
    }

	return saveProjectFile();
}

void BaseProject::addAddon(ofAddon& addon)
{

    for (std::size_t i = 0; i < _addons.size(); ++i)
    {
		if (_addons[i].name == addon.name)
        {
            return;
        }
	}

	_addons.push_back(addon);

    for (std::size_t i = 0; i < addon.includePaths.size(); ++i)
    {
        ofLogVerbose("BaseProject::addAddon") << "Adding addon include path: " << addon.includePaths[i];
        addInclude(addon.includePaths[i]);
    }

    for (std::size_t i = 0; i < addon.libs.size(); ++i)
    {
        ofLogVerbose("BaseProject::addAddon") << "Adding addon libs: " << addon.libs[i];
        addLibrary(addon.libs[i], RELEASE_LIB);
    }

    for (std::size_t i = 0; i < addon.srcFiles.size(); ++i)
    {
        ofLogVerbose("BaseProject::addAddon") << "Adding addon srcFiles: " << addon.srcFiles[i];
        addSrc(addon.srcFiles[i],addon.filesToFolders[addon.srcFiles[i]]);
    }
}

void BaseProject::parseAddons()
{
    ofFile addonsMake(ofFilePath::join(_projectPath.toString(), "addons.make"));

    ofBuffer addonsMakeMem;

    addonsMake >> addonsMakeMem;

    ofBuffer::Line lineIter = addonsMakeMem.getLines().begin();

    while (lineIter != addonsMakeMem.getLines().end())
    {
        ofAddon addon;

        addon.pathToOF = PGUtils::getOFRelPath(_projectPath.toString()).toString();

        Poco::Path addonsPath(PGUtils::getOFRoot(), "addons/");

        addon.fromFS(Poco::Path(addonsPath, *lineIter).toString(), _target);
        
        addAddon(addon);
        
        ++lineIter;
    }
}


std::string BaseProject::getName() const
{
    return _projectName;
}


Poco::Path BaseProject::getPath() const
{
    return _projectPath;
}


} } // namespace ofx::PG
