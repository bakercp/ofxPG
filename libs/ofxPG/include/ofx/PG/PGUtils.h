#pragma once


#include "ofMain.h"
#include "ofxPugiXML.h"


namespace ofx {
namespace PG {


class PGUtils
{
public:
    static std::string generateUUID(std::string input);

    static Poco::Path getOFRoot();
    static Poco::Path getAddonsRoot();

    static void setOFRoot(const Poco::Path& path);

    static void findandreplace(std::string& tInput,
                               std::string tFind,
                               std::string tReplace);

    static void findandreplaceInTexfile(std::string fileName,
                                        std::string tFind,
                                        std::string tReplace);

    static bool isFolderNotCurrentPlatform(std::string folderName,
                                           std::string platform);

    static bool doesTagAndAttributeExist(pugi::xml_document& doc,
                                         std::string tag,
                                         std::string attribute,
                                         std::string newValue);

    static pugi::xml_node appendValue(pugi::xml_document& doc,
                                      std::string tag,
                                      std::string attribute,
                                      std::string newValue,
                                      bool addMultiple = false);

    static void getFoldersRecursively(const string& path,
                                      vector<string>& folderNames,
                                      string platform);

//    static void getFilesRecursively(const string& path,
//                                    vector<string>& fileNames);

    static void getLibsRecursively(const std::string& path,
                                   std::vector<std::string>& libFiles,
                                   std::vector<std::string>& libLibs,
                                   std::string platform = "" );

    static void splitFromLast(std::string toSplit,
                              std::string deliminator,
                              std::string& first,
                              std::string& second);

    static void splitFromFirst(std::string toSplit,
                               std::string deliminator,
                               std::string& first,
                               std::string& second);

    static void parseAddonsDotMake(const std::string& path,
                                   std::vector<std::string>& addons);

    static void fixSlashOrder(std::string& toFix);

    static std::string unsplitString(std::vector<std::string> strings,
                                     std::string deliminator );

    static Poco::Path getOFRelPath(const Poco::Path& from);

    static Poco::Path makeRelativePath(const Poco::Path& from, const Poco::Path& to);


    static bool checkConfigExists();
    static bool askOFRoot();
    static std::string getOFRootFromConfig();

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

    static std::vector<std::string> platforms;

    static Poco::Path OFRoot;

};


} } // namespace ofx::PG
