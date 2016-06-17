//for now this will only contain simple directory commands
//later, it will contain file and directory manipulation,
//in addition to windows support for tagging

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include "boost/filesystem.hpp"
#include "text_manage.h"

namespace fs = boost::filesystem;

unsigned int getFileSize(std::string filename);

std::unordered_set<std::string> getDefaultSuffixes();
//returns the file type
std::string file_suffix(std::string filename);

std::string getFileName(std::string str);

std::vector<std::string> relevantFiles(std::string directory,
 std::unordered_set<std::string> validSuffixes, bool relative = true);

void readCall(std::string filename,gramDict * mainDict, int N, int trimCutoff);

void outputFileList(std::vector<std::string> *fileList,std::string filename);


#endif FILE_MANAGER_H
