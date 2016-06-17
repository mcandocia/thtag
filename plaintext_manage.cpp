#include "plaintext_manage.h"

void read_plaintext(std::string filename,gramDict * mainDict, int N,int trimCutoff)
{
  //std::cout<<"reading file\n";
  std::fstream plaintext;
  std::string line;
  plaintext.open(filename);
  std::string curstring = "";
  gramDict dictionary;
  while (std::getline(plaintext,line))
  {
    line = removeNonAlpha(line);
    curstring+=line;
    curstring+=' ';
    //std::cout<<line<<'\n';
    if (line.length()<2||curstring.size()>2000)
    {
      //std::cout<<curstring<<'\n';
      //std::cout<<curstring.length()<<'\n';
      //curstring = removeNonAlpha(curstring);
      if (N==1)
        parse_page_text(&dictionary,curstring);
      else
        parse_page_text_ngram(&dictionary,curstring,N);
      curstring.clear();
      if (dictionary.size() > 500000)
      {
        std::cout<<"REDUCING DICT SIZE\n";
        trimUnderCutoff(&dictionary,trimCutoff,1);
      }

    }
  }
  //after loop is over, take into account final text
  if (curstring.length()>0)
  {
    //std::cout<<curstring<<'\n';
    curstring = removeNonAlpha(curstring);
    if (N==1)
      parse_page_text(&dictionary,curstring);
    else
      parse_page_text_ngram(&dictionary,curstring,N);
  }
  plaintext.close();
  trimToTopN(&dictionary,trimCutoff,5);
  combine_dictionaries(mainDict,&dictionary);
}
