#include "ofApp.h"
#include "ofx/PG/PGUtils.h"


void ofApp::setup()
{

    ofSetLogLevel(OF_LOG_VERBOSE);

	project = 0;

	while(!ofx::PG::PGUtils::checkConfigExists())
    {
		ofx::PG::PGUtils::askOFRoot();
	}

	ofx::PG::PGUtils::setOFRoot(ofx::PG::PGUtils::getOFRootFromConfig());

	setupDrawableOFPath();

	int targ = ofGetTargetPlatform();

	//plat = OF_TARGET_IPHONE;

    setupForTarget(targ);

    if (projectPath != "" || buildAllExamples)
    {
    	for(std::size_t i = 0; i < targetsToMake.size(); ++i)
        {
			setupForTarget(targetsToMake[i]);

            if(buildAllExamples)
            {
				generateExamples();
			}
            else
            {
				project->setup(target);
				project->create(projectPath);
				vector < string > addons;

				ofx::PG::PGUtils::parseAddonsDotMake(project->getPath() + "addons.make", addons);

				for (std::size_t i = 0; i < addons.size(); ++i)
                {
					ofx::PG::ofAddon addon;

					addon.fromFS(ofFilePath::join(ofFilePath::join(ofx::PG::PGUtils::getOFRoot(), "addons"), addons[i]),target);

                    project->addAddon(addon);
				}

				project->save(false);
			}
    	}

        std::exit(0);
    }

#ifndef COMMAND_LINE_ONLY
    panelAddons.setup();

    ofDirectory addons(ofFilePath::join(ofx::PG::PGUtils::getOFRoot(),"addons"));

    addons.listDir();

    for (std::size_t i = 0; i < addons.size(); ++i)
    {
    	string addon = addons.getName(i);

    	if(addon.find("ofx") == 0)
        {
    		ofxToggle* toggle = new ofxToggle();

    		panelAddons.add(toggle->setup(addon,false,300));
    	}
    }

    panelOptions.setup("","settings.xml",ofGetWidth()-panelAddons.getWidth()-10,120);
    panelOptions.add(createProject.setup("create project",300));
    panelOptions.add(updateProject.setup("update project",300));
    panelOptions.add(createAndOpen.setup("create and open project",300));
    panelOptions.add(changeOFRoot.setup("change OF path",300));

    createProject.addListener(this,&ofApp::createProjectPressed);
    updateProject.addListener(this,&ofApp::updateProjectPressed);
    createAndOpen.addListener(this,&ofApp::createAndOpenPressed);
    changeOFRoot.addListener(this,&ofApp::changeOFRootPressed);

	examplesPanel.setup("generate examples", "examples.xml", 400, 10);
	examplesPanel.add(generateButton.setup("<--Generate"));
	examplesPanel.add(wincbToggle.setup("win CB projects",ofGetTargetPlatform()==OF_TARGET_WINGCC));
	examplesPanel.add(winvsToggle.setup("win VS projects", ofGetTargetPlatform()==OF_TARGET_WINVS));
	examplesPanel.add(linuxcbToggle.setup("linux CB projects",ofGetTargetPlatform()==OF_TARGET_LINUX));
	examplesPanel.add(linux64cbToggle.setup("linux64 CB projects",ofGetTargetPlatform()==OF_TARGET_LINUX64));
	examplesPanel.add(linuxarmv6lcbToggle.setup("linuxarmv6l CB projects",ofGetTargetPlatform()==OF_TARGET_LINUXARMV6L));
	examplesPanel.add(linuxarmv7lcbToggle.setup("linuxarmv7l CB projects",ofGetTargetPlatform()==OF_TARGET_LINUXARMV7L));
	examplesPanel.add(osxToggle.setup("osx projects",ofGetTargetPlatform()==OF_TARGET_OSX));
	examplesPanel.add(iosToggle.setup("ios projects",ofGetTargetPlatform()==OF_TARGET_IPHONE));

	generateButton.addListener(this,&ofApp::generateExamplesCB);

    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
	ofSetFrameRate(60);
#else
	std::exit(0);
#endif
}

void ofApp::setupForTarget(int targ)
{
    if(project)
    {
		delete project;
	}

    switch(targ)
    {
        case OF_TARGET_OSX:
            project = new ofx::PG::XcodeProject;
            target = "osx";
            break;
        case OF_TARGET_WINGCC:
            project = new ofx::PG::CBWinProject;
            target = "win_cb";
            break;
        case OF_TARGET_WINVS:
            project = new ofx::PG::VisualStudioProject;
            target = "vs";
            break;
        case OF_TARGET_IPHONE:
            project = new ofx::PG::XcodeProject;
            target = "ios";
            break;
        case OF_TARGET_ANDROID:
            break;
        case OF_TARGET_LINUX:
            project = new ofx::PG::CBLinuxProject;
            target = "linux";
            break;
        case OF_TARGET_LINUX64:
            project = new ofx::PG::CBLinuxProject;
            target = "linux64";
            break;
        case OF_TARGET_LINUXARMV6L:
            project = new ofx::PG::CBLinuxProject;
            target = "linuxarmv6l";
            break;
        case OF_TARGET_LINUXARMV7L:
            project = new ofx::PG::CBLinuxProject;
            target = "linuxarmv7l";
            break;
    }
}

void ofApp::generateExamplesCB()
{

#ifndef COMMAND_LINE_ONLY

	targetsToMake.clear();
	if ( osxToggle )		targetsToMake.push_back(OF_TARGET_OSX);
	if ( iosToggle )		targetsToMake.push_back(OF_TARGET_IPHONE);
	if ( wincbToggle )	targetsToMake.push_back(OF_TARGET_WINGCC);
	if ( winvsToggle )	targetsToMake.push_back(OF_TARGET_WINVS);
	if ( linuxcbToggle )         targetsToMake.push_back(OF_TARGET_LINUX);
	if ( linux64cbToggle )       targetsToMake.push_back(OF_TARGET_LINUX64);
	if ( linuxarmv6lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV6L);
	if ( linuxarmv7lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV7L);

	if (targetsToMake.size() == 0)
    {
		cout << "Error: generateExamplesCB - must specifiy a project to generate " <<endl;
	}

	for(std::size_t i = 0; i < targetsToMake.size(); ++i)
    {
		setupForTarget(targetsToMake[i]);
		generateExamples();
	}

	int target = ofGetTargetPlatform();
    setupForTarget(target);
#endif

}

void ofApp::generateExamples()
{
    ofDirectory dir;

    string examplesPath = ofFilePath::join(ofx::PG::PGUtils::getOFRoot(), "examples");

	ofLogNotice() << "Generating examples (from: " << examplesPath << ")";

    dir.listDir(examplesPath);

    for (std::size_t i = 0; i < dir.size(); ++i)
    {

        // don't check subdirectories that aren't directories! (eg., .gitignore etc)
        if (!dir.getFile(i).isDirectory())
            continue;

		if (target == "ios")
        {
			if (dir.getName(i) != "ios")
                continue;
		}
        else
        {
			if (dir.getName(i) == "android" || dir.getName(i) == "ios")
                continue;
        }

        ofDirectory subdir;
        string examplesPath = dir.getPath(i);

		ofLogNotice() << "Generating examples in folder: " << examplesPath;

        subdir.listDir(examplesPath);

        for (std::size_t j = 0; j < subdir.size(); ++j)
        {

            // don't create projects that aren't directories! (eg., .gitkeep etc)
            if(!subdir.getFile(j).isDirectory()) continue;

			ofLogNotice() << "------------------------------------------------";
			ofLogNotice() << "Generating example: " << subdir.getPath(j);
			ofLogNotice() << "------------------------------------------------";

            project->setup(target);
            project->create(subdir.getPath(j));
            vector < string > addons;
            ofx::PG::PGUtils::parseAddonsDotMake(project->getPath() + "addons.make", addons);

            for (std::size_t i = 0; i < addons.size(); ++i)
            {
                ofx::PG::ofAddon addon;
                addon.pathToOF = ofx::PG::PGUtils::getOFRelPath(subdir.getPath(j));
                addon.fromFS(ofFilePath::join(ofFilePath::join(ofx::PG::PGUtils::getOFRoot(), "addons"), addons[i]),target);
                project->addAddon(addon);
            }
            project->save(false);

        }
    }

    ofLogNotice() << " ";
    ofLogNotice() << "Finished generating examples for " << target;
}

ofFileDialogResult ofApp::makeNewProjectViaDialog()
{

#ifndef COMMAND_LINE_ONLY
    ofFileDialogResult res = ofSystemSaveDialog("newProjectName", "choose a folder for a new OF project :)");
    if (res.fileName == "" || res.filePath == "") return res;
    //base.pushDirectory(res.fileName);   // somehow an extra things here helps?

    vector <int> targetsToMake;

    if ( osxToggle )		targetsToMake.push_back(OF_TARGET_OSX);
	if ( iosToggle )		targetsToMake.push_back(OF_TARGET_IPHONE);
	if ( wincbToggle )	targetsToMake.push_back(OF_TARGET_WINGCC);
	if ( winvsToggle )	targetsToMake.push_back(OF_TARGET_WINVS);
	if ( linuxcbToggle )	targetsToMake.push_back(OF_TARGET_LINUX);
	if ( linux64cbToggle )	targetsToMake.push_back(OF_TARGET_LINUX64);
	if ( linuxarmv6lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV6L);
 	if ( linuxarmv7lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV7L);

	if (targetsToMake.size() == 0)
    {
		cout << "Error: makeNewProjectViaDialog - must specifiy a project to generate " <<endl;
		ofSystemAlertDialog("Error: makeNewProjectViaDialog - must specifiy a project platform to generate");
	}

	for(std::size_t i = 0; i < targetsToMake.size(); ++i)
    {
		setupForTarget(targetsToMake[i]);

        project->setup(target);

        if(project->create(res.filePath))
        {
            vector<string> addonsToggles = panelAddons.getControlNames();
            for (std::size_t i = 0; i < addonsToggles.size(); ++i)
            {
                ofxToggle toggle = panelAddons.getToggle(addonsToggles[i]);

                if(toggle)
                {
                    ofx::PG::ofAddon addon;
                    addon.pathToOF = ofx::PG::PGUtils::getOFRelPath(res.filePath);
                    addon.fromFS(ofFilePath::join(ofFilePath::join(ofx::PG::PGUtils::getOFRoot(), "addons"), addonsToggles[i]),target);
                    printf("adding %s addons \n", addonsToggles[i].c_str());
                    project->addAddon(addon);

                }
            }

            project->save(true);
        }
	}

    return res;
#endif
}

ofFileDialogResult ofApp::updateProjectViaDialog(){

#ifndef COMMAND_LINE_ONLY
    ofFileDialogResult res = ofSystemLoadDialog("choose a folder to update an OF project :)",true);
    if (res.fileName == "" || res.filePath == "") return res;
    //base.pushDirectory(res.fileName);   // somehow an extra things here helps?

    vector <int> targetsToMake;
	if( osxToggle )		targetsToMake.push_back(OF_TARGET_OSX);
	if( iosToggle )		targetsToMake.push_back(OF_TARGET_IPHONE);
	if( wincbToggle )	targetsToMake.push_back(OF_TARGET_WINGCC);
	if( winvsToggle )	targetsToMake.push_back(OF_TARGET_WINVS);
	if( linuxcbToggle )	targetsToMake.push_back(OF_TARGET_LINUX);
	if( linux64cbToggle )	targetsToMake.push_back(OF_TARGET_LINUX64);
	if( linuxarmv6lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV6L);
	if( linuxarmv7lcbToggle )	targetsToMake.push_back(OF_TARGET_LINUXARMV7L);

	if( targetsToMake.size() == 0 )
    {
		cout << "Error: updateProjectViaDialog - must specifiy a project to generate " <<endl;
		ofSystemAlertDialog("Error: updateProjectViaDialog - must specifiy a project platform to generate");
	}

	for(std::size_t i = 0; i < targetsToMake.size(); ++i)
    {
		setupForTarget(targetsToMake[i]);
        project->setup(target);
        project->create(res.filePath);
        vector<string> addonsToggles = panelAddons.getControlNames();
        for (int i = 0; i < (int)addonsToggles.size(); i++){
            ofxToggle toggle = panelAddons.getToggle(addonsToggles[i]);
            // TODO: make this remove existing addons that are unchecked????
            // probably requires a more complex logic chain: loadProject
            // (ticks the addons) and then you can untick etc???
            if(toggle){
                ofx::PG::ofAddon addon;
                addon.pathToOF = ofx::PG::PGUtils::getOFRelPath(res.filePath);
                addon.fromFS(ofFilePath::join(ofFilePath::join(ofx::PG::PGUtils::getOFRoot(), "addons"), addonsToggles[i]),target);
                printf("adding %s addons \n", addonsToggles[i].c_str());
                project->addAddon(addon);

            }
        }
        project->save(true);
	}

	return res;
#endif

}

void ofApp::createProjectPressed()
{
	makeNewProjectViaDialog();
}

void ofApp::updateProjectPressed()
{
	updateProjectViaDialog();
}

void ofApp::createAndOpenPressed()
{
	ofFileDialogResult res = makeNewProjectViaDialog();

	if (res.bSuccess)
    {
		#ifdef TARGET_LINUX
			system(("/usr/bin/codeblocks " + ofFilePath::join(res.filePath, res.fileName+".workspace ") + "&").c_str());
		#elif defined(TARGET_OSX)
			system(("open " + ofFilePath::join(res.filePath, res.fileName+".xcodeproj ") + "&").c_str());
		#elif defined(TARGET_WIN32)
			system(("open " + ofFilePath::join(res.filePath, res.fileName+".workspace ") + "&").c_str());
		#endif
	}
}

void ofApp::changeOFRootPressed()
{
	ofx::PG::PGUtils::askOFRoot();
	cout << ofx::PG::PGUtils::getOFRootFromConfig()<<endl;
	ofx::PG::PGUtils::setOFRoot(ofx::PG::PGUtils::getOFRootFromConfig());
	setupDrawableOFPath();
}


void ofApp::draw(){

#ifndef COMMAND_LINE_ONLY
    //ofBackgroundGradient(ofColor::gray,ofColor::black);

    panelAddons.draw();
	panelOptions.draw();
	examplesPanel.draw();

	ofSetColor(0,0,0,100);

	ofRect(ofPathRect);

    /*ofDrawBitmapString("press 'm' to make all files\npress ' ' to make a specific file", ofPoint(30,30));*/
	ofSetColor(255);
    ofDrawBitmapString(drawableOfPath, ofPathDrawPoint);
#endif

}


void ofApp::keyPressed(int key)
{

    if (key == 'm'){

        printf("generating examples \n");
        generateExamples();
        printf("finished generating examples \n");

    }

    if (key == ' '){
        makeNewProjectViaDialog();
    }


}


void ofApp::windowResized(int w, int h){
	setupDrawableOFPath();
}


void ofApp::setupDrawableOFPath()
{

#ifndef COMMAND_LINE_ONLY
	vector<string> subdirs = ofSplitString("OF path: " + ofx::PG::PGUtils::getOFRoot(), "/");
	int textLength = 0;
	int padding = 5;
	string path = "";
	int lines=1;
	int fontSize = 8;
	float leading = 1.7;

	ofPathRect.x = padding;
	ofPathRect.y = padding;
	ofPathDrawPoint.x = padding*2;
	ofPathDrawPoint.y = padding*2 + fontSize * leading;

	for(std::size_t i = 0; i < subdirs.size(); ++i)
    {
		if (i > 0 && i<subdirs.size()-1)
        {
			subdirs[i] += "/";
		}

        if(textLength + subdirs[i].length()*fontSize < ofGetWidth()-padding)
        {
			textLength += subdirs[i].length()*fontSize;
			path += subdirs[i];
		}
        else
        {
			path += "\n";
			textLength = 0;
			lines++;
		}
	}

	ofPathRect.width = textLength + padding*2;

    if (lines > 1)
    {
		ofPathRect.width = ofGetWidth() - padding*2;
	}

    ofPathRect.height = lines * fontSize * leading + (padding*2);

	drawableOfPath = path;

	panelAddons.setPosition(panelAddons.getPosition().x, ofPathRect.y + ofPathRect.height + padding);
	examplesPanel.setPosition(examplesPanel.getPosition().x, ofPathRect.y + ofPathRect.height + padding);
#endif


}
