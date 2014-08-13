#include "ofx/PG/CBLinuxProject.h"
#include "ofFileUtils.h"
#include "ofLog.h"


namespace ofx {
namespace PG {


const std::string CBLinuxProject::LOG_NAME = "CBLinuxProject";


void CBLinuxProject::setup() {
	templatePath = ofFilePath::join(Utils::getOFRoot(),"scripts/linux/template/"+target);
}

bool CBLinuxProject::createProjectFile(){
	ofDirectory dir(projectPath);
	if(!dir.exists()) dir.create(true);

    ofFile project(ofFilePath::join(projectPath, projectName + ".cbp"));
    string src =  ofFilePath::join(templatePath,"emptyExample_" + target + ".cbp");
    string dst = project.path();
    bool ret;

    if(!project.exists()){
		ret = ofFile::copyFromTo(src,dst);
		if(!ret){
			ofLogError(LOG_NAME) << "error copying cbp template from " << src << " to " << dst;
			return false;
		}else{
			Utils::findandreplaceInTexfile(dst, "emptyExample", projectName);
		}
    }

    ofFile workspace(ofFilePath::join(projectPath, projectName + ".workspace"));
    if(!workspace.exists()){
		src = ofFilePath::join(templatePath,"emptyExample_" + target + ".workspace");
		dst = workspace.path();
		ret = ofFile::copyFromTo(src,dst);
		if(!ret){
			ofLogError(LOG_NAME) << "error copying workspace template from "<< src << " to " << dst;
			return false;
		}else{
			Utils::findandreplaceInTexfile(dst, "emptyExample", projectName);
		}
    }

    ofFile makefile(ofFilePath::join(projectPath,"Makefile"));
    if(!makefile.exists()){
		src = ofFilePath::join(templatePath,"Makefile");
		dst = makefile.path();
		ret = ofFile::copyFromTo(src,dst);
		if(!ret){
			ofLogError(LOG_NAME) << "error copying Makefile template from " << src << " to " << dst;
			return false;
		}
    }

    ofFile config(ofFilePath::join(projectPath,"config.make"));
    if(!config.exists()){
    	src = ofFilePath::join(templatePath,"config.make");
    	dst = config.path();
    	ret = ofFile::copyFromTo(src,dst);
    	if(!ret){
    		ofLogError(LOG_NAME) << "error copying config.make template from " << src << " to " << dst;
    		return false;
    	}
    }


    // handle the relative roots.
    string relRoot = Utils::getOFRelPath(ofFilePath::removeTrailingSlash(projectPath));
    if (relRoot != "../../../"){
        string relPath2 = relRoot;
        relPath2.erase(relPath2.end()-1);
        Utils::findandreplaceInTexfile(projectPath + "config.make", "../../..", relPath2);
        Utils::findandreplaceInTexfile(ofFilePath::join(projectPath , projectName + ".workspace"), "../../../", relRoot);
        Utils::findandreplaceInTexfile(ofFilePath::join(projectPath , projectName + ".cbp"), "../../../", relRoot);
    }

    return true;
}


} } // namespace ofx::PG
