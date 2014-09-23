#pragma once


#include "ofMain.h"
#include "ofxPugiXML.h"


namespace ofx {
namespace PG {


class PGUtils
{
public:
    static std::string generateUUID(string input);

    static string getOFRoot();
    static string getAddonsRoot();
    static void setOFRoot(string path);

    static std::string loadFileAsString(const std::string & fn);

    static void findandreplace( std::string& tInput, std::string tFind, std::string tReplace );

    static void findandreplaceInTexfile(string fileName, string tFind, string tReplace );


    static bool isFolderNotCurrentPlatform(string folderName, string platform);

    static bool doesTagAndAttributeExist(pugi::xml_document & doc, string tag, string attribute, string newValue);
    static pugi::xml_node appendValue(pugi::xml_document & doc,
                                      string tag,
                                      string attribute,
                                      string newValue,
                                      bool addMultiple = false);

    static void getFoldersRecursively(const string & path, vector < string > & folderNames, string platform);
    static void getFilesRecursively(const string & path, vector < string > & fileNames);
    static void getLibsRecursively(const string & path, vector < string > & libFiles, vector < string > & libLibs, string platform = "" );

    static void splitFromLast(string toSplit, string deliminator, string & first, string & second);
    static void splitFromFirst(string toSplit, string deliminator, string & first, string & second);

    static void parseAddonsDotMake(string path, vector < string > & addons);

    static void fixSlashOrder(string & toFix);
    static string unsplitString (vector < string > strings, string deliminator );

    static string getOFRelPath(string from);

    static bool checkConfigExists();
    static bool askOFRoot();
    static string getOFRootFromConfig();

    template <class T>
    static bool isInVector(T item, vector<T> & vec)
    {
        bool bIsInVector = false;
        for(int i=0;i<vec.size();i++){
            if(vec[i] == item){
                bIsInVector = true;
                break;
            }
        }
        return bIsInVector;
    }

    static vector <string> platforms;

    static string OFRoot;

};


} } // namespace ofx::PG
