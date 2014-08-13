#include "ofx/PG/CBWinProject.h"


namespace ofx {
namespace PG {        


const std::string CBWinProject::LOG_NAME = "CBWinProject";


void CBWinProject::setup()
{
}


bool CBWinProject::createProjectFile()
{

    string project = projectPath + projectName + ".cbp";
    string workspace = projectPath + projectName + ".workspace";


	ofFile::copyFromTo(ofFilePath::join(templatePath,"emptyExample.cbp"),project, false, true);

	ofFile::copyFromTo(ofFilePath::join(templatePath,"emptyExample.workspace"),workspace, false, true);
	ofFile::copyFromTo(ofFilePath::join(templatePath,"icon.rc"), projectPath + "icon.rc", false, true);

    //let's do some renaming:
    string relRoot = Utils::getOFRelPath(ofFilePath::removeTrailingSlash(projectPath));

    if (relRoot != "../../../"){

        string relRootWindows = relRoot;
        // let's make it windows friendly:
        for(int i = 0; i < relRootWindows.length(); i++) {
            if( relRootWindows[i] == '/' )
                relRootWindows[i] = '\\';
        }

        Utils::findandreplaceInTexfile(workspace, "../../../", relRoot);
        Utils::findandreplaceInTexfile(project, "../../../", relRoot);

        Utils::findandreplaceInTexfile(workspace, "..\\..\\..\\", relRootWindows);
        Utils::findandreplaceInTexfile(project, "..\\..\\..\\", relRootWindows);
    }

    return true;
}

bool CBWinProject::loadProjectFile()
{

    //project.open(ofFilePath::join(projectDir , projectName + ".cbp"));

    ofFile project(projectPath + projectName + ".cbp");
	if(!project.exists()){
		ofLogError(LOG_NAME) << "error loading" << project.path() << "doesn't exist";
		return false;
	}
	pugi::xml_parse_result result = doc.load(project);
	bLoaded =result.status==pugi::status_ok;
	return bLoaded;
}

bool CBWinProject::saveProjectFile()
{

    Utils::findandreplaceInTexfile(ofFilePath::join(projectPath , projectName + ".workspace"),"emptyExample",projectName);
    pugi::xpath_node_set title = doc.select_nodes("//Option[@title]");
    if(!title.empty()){
        if(!title[0].node().attribute("title").set_value(projectName.c_str())){
            ofLogError(LOG_NAME) << "can't set title";
        }
    }
    return doc.save_file((projectPath + projectName + ".cbp").c_str());
}

void CBWinProject::addSrc(const std::string& srcName, const std::string& folder)
{
	pugi::xml_node node = Utils::appendValue(doc, "Unit", "filename", srcName);
	if(!node.empty()){
		node.child("Option").attribute("virtualFolder").set_value(folder.c_str());
	}
    doc.save_file((projectPath + projectName + ".cbp").c_str());
}

void CBWinProject::addInclude(const std::string& includeName)
{
    ofLogNotice() << "adding include " << includeName;
    Utils::appendValue(doc, "Add", "directory", includeName);
}

void CBWinProject::addLibrary(const string& libraryName, LibType libType)
{
    ofLogNotice() << "adding library " << libraryName;
    Utils::appendValue(doc, "Add", "library", libraryName, true);
    // overwriteMultiple for a lib if it's there (so libsorder.make will work)
    // this is because we might need to say libosc, then ws2_32
}


} } // namespace ofx::PG
