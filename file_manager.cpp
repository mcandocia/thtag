#include "file_manager.h"
#include "pdf_manage.h"
#include "plaintext_manage.h"
#include "html_manage.h"

unsigned int getFileSize(std::string filename)
{
  return (unsigned int) fs::file_size(filename.c_str());
}

std::unordered_set<std::string> getDefaultSuffixes()
{
  std::unordered_set<std::string> suffixes;
  suffixes.insert("pdf");
  suffixes.insert("txt");
  suffixes.insert("htm");
  suffixes.insert("html");
  return suffixes;

}

std::string file_suffix(std::string str)
{
  int pos = str.find_last_of('.');
  int n = str.length();
  if (pos >= n)
    return "";
  return getLastNLetters(str,n-pos-1);
}

std::string getFileName(std::string str)
{
  int pos = str.find_last_of('/');
  int n = str.length();
  if (pos >= n )
    return "";
  return getLastNLetters(str,n-pos);
}

std::vector<std::string> relevantFiles(std::string directory,
 std::unordered_set<std::string> validSuffixes,bool relative)
{

 /* if (!relative)
    fs::path target_path(fs::absolute(fs::path(directory)));
  else*/
    fs::path target_path(fs::complete(fs::path(directory)));
  std::vector<std::string> results;
  fs::directory_iterator end_iter;
  for (fs::directory_iterator dir_itr(target_path);
   dir_itr != end_iter;++dir_itr)
  {
    if (fs::is_regular_file(dir_itr->status()))
    {
      fs::path filepath = dir_itr->path();
      results.push_back(filepath.string());
    }
  }


  return results;
}

void readCall(std::string filename,gramDict * mainDict, int N, int trimCutoff)
{
  std::string suffix = file_suffix(filename);
  //std::cout<<"SUFFIX: "<<suffix<<'\n';
  const char * sfc = suffix.c_str();
  if (strcmp(sfc, "html")==0)
  {
    std::cout<<"READING HTML\n";
    read_html(filename,mainDict,N,trimCutoff);
  }
  else if (strcmp(sfc, "htm")==0)
  {
    std::cout<<"READING HTML"<<'\n';
    read_html(filename,mainDict,N,trimCutoff);
  }
  else if (strcmp(sfc, "txt")==0)
  {
    std::cout<<"READING PLAINTEXT"<<'\n';
    read_plaintext(filename,mainDict,N,trimCutoff);
  }
  else if (strcmp(sfc, "pdf")==0)
  {
    std::cout<<"READING PDF\n";
    read_pdf(filename,mainDict,N,trimCutoff);
  }
  else
    std::cout<<"ERROR, FILENAME SUFFIX NOT RECOGNIZED";

}

void outputFileList(std::vector<std::string> *fileList,std::string filename)
{
  std::fstream output;
  output.open(filename.c_str(),std::fstream::out);
  for (std::vector<std::string>::const_iterator it = fileList->begin();
   it != fileList->end();++it)
  {
    output<<(*it)<<'\n';
  }
  output.close();
}
