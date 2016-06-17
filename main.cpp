#include "wx/wx.h"
#include <cstdlib>
#include <iostream>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/dir.h>
#include "text_manage.h"
#include "pdf_manage.h"
#include "html_manage.h"
#include "plaintext_manage.h"
#include "file_manager.h"



//don't include UI until program is otherwise done

//pdf samples to be used to test the various functions here
//length = 8, data structures index = 3

const std::string subdirectory = "examples/";
const std::string pdf_filenames[8] = {"Acoustics, Information, and Communication.pdf",
"Advances in K-means Clustering.pdf","Conductors, Semiconductors, and Superconductors.pdf",
"Data Structures Textbook (C++).pdf","General Relativity.pdf",
"Quantum Theory - A Mathematical Approach.pdf","Temporal Networks.pdf",
"Text Mining, Weiss.pdf"};
const std::string html_filenames[7] = {"Fresh Kills - Wikipedia, the free encyclopedia.html",
"General relativity - Wikipedia, the free encyclopedia.html",
"Mario's Picross - Wikipedia, the free encyclopedia.html",
"NAV-CO2 systems - Wikipedia, the free encyclopedia.html",
"Thorn (letter) - Wikipedia, the free encyclopedia.html",
"Wessagusset Colony - Wikipedia, the free encyclopedia.html",
"World's longest hot dog - Wikipedia, the free encyclopedia.html"};
const std::string txt_filenames[4] = {"crime_and_punishment.txt",
"pride_and_prejudice.txt","sense_and_sensibility.txt",
"The_Call_of_Cthulhu.txt"};

extern std::unordered_set<std::string> OUTER_STOPWORDS;

extern std::unordered_set<std::string> INNER_STOPWORDS;

bool KEEP_RESULTS_IN_MEMORY = false;//memory should be tested to see what conditions this is useful for

const std::string FILENAME_NGRAM = "ngrams.txt";

const std::string FILENAME_FILENAMES = "filenames.txt";

int main(){
  //default initializations
  initializeStopwords();
  std::unordered_set<std::string> suffixes = getDefaultSuffixes();
  //everything else
  std::vector<std::string> rf = relevantFiles("examples",suffixes,false);//home/max/workspace/thtag/
  //printStringVector(rf);
  gramDict mainDict;
  std::vector<gramDict> dictVec;

  //read all files in example subdirectory and clean each dictionary and merge them
  for (std::vector<std::string>::const_iterator current = rf.begin();
   current != rf.end();++current)
  {
    std::cout<<(*current)<<'\n';

      gramDict tempDict;
      long fileSize = getFileSize((*current));
      std::cout<<"FILE SIZE: "<<fileSize<<'\n';
      readCall((*current),&tempDict,-3,60);
      basicNLP(&tempDict);
      trimToTopN(&tempDict,(int) std::max(2*log(fileSize+500), 6.00));
      merge_dictionaries(&mainDict,&tempDict);
      dictVec.push_back(tempDict);
  }
  read_dictionary(&mainDict);
  outputDictionary(&mainDict,"testDictOutput.txt");
  //create map from files to keywords
  convertCountToIndex(&mainDict);
  writeKeywordDataFile(&dictVec,&mainDict,"keywordMap.txt");
  outputFileList(&rf,"fileList.txt");
  std::cout<<"DONE!\n";

}



