#include "pdf_manage.h"



//the type of object to be returned will have to be determined

//this function reads through a pdf and creates a dictionary
//of words for each of the pages...will be extended to support
//ngrams shortly

//debugging purposes only
void writeIntChars(std::string str)
{
  std::fstream output;
  output.open("wtf_is_going_on.txt",std::fstream::in | std::fstream::out | std::fstream::app);
  output<<str<<'\n';
  const char* cstr = str.c_str();
  int nchars = str.length();
  for (int i = 0; i < nchars; i++)
  {
    char c = cstr[i];
    output<<c<<'\n';
    output<<(int) c<<'\n';
  }
  output<<"NEW PAGE\n";
  output.close();
}

std::string ligatureSub(std::string str)
{
  const char* cstr = str.c_str();
  //std::cout<<str<<'\n';
  int nchars = str.length();
  std::string result = "";
  bool ligatureExcept = false;
  for (int i = 0; i<nchars;i++)
  {
    if (cstr[i] >=0)
      result+=cstr[i];
    else if (cstr[i] == -61)
      ligatureExcept = true;
    else if (ligatureExcept)
    {
      switch(cstr[i])
      {
        case -98 :
          result += "fi";
          break;
        case -112 :
          result += "-";
          break;
        case -107 :
          result += "'";
          break;
        case -108 :
          result += "'";
          break;
        case -97 :
          result += "fl";
          break;
        default :
          break;
      }
      ligatureExcept = false;
    }
    //std::cout<<cint<<'\n';
  }
  //std::cout<<result;
  return result;
}

void read_pdf(std::string filename, gramDict * mainDict, int N, int trimCutoff)
{
  gramDict dictionary;
  PoDoFo::PdfMemDocument pdf(filename.c_str());
 // std::fstream testout;
  //testout.open("wtf_is_going_on.txt");
  for (int pn = 0; pn<pdf.GetPageCount();++pn)
  {
    //if (pn>18){break;}
    //std::cout<<"+++NEW PAGE+++\n";
    PoDoFo::PdfPage* page = pdf.GetPage(pn);
    PoDoFo::PdfContentsTokenizer tok(page);
    const char* token = nullptr;
    PoDoFo::PdfVariant var;
    PoDoFo::EPdfContentsType type;
    bool reading_text = false;
    std::string page_text = "";
    while (tok.ReadNext(type, token, var))
    {
      if (type==PoDoFo::ePdfContentsType_Keyword)
      {
        //std::cout<<"TOKEN: "<<token<<'\n';
        if (strcmp(token,"BT")==0)
        {
          reading_text=true;
          //td::cout<<"BT\n";
          continue;
        }
        else if (strcmp(token,"ET")==0){
          reading_text=false;
          //std::cout<<"BT\n";
          continue;
        }
        if (!reading_text)
          continue;
        std::string text = "";
        if (!string_in_vector_c(token,valid_tokens))
          continue;
        //std::cout<<"--\n";
        if (var.IsArray())
        {
          PoDoFo::PdfArray& a = var.GetArray();
          for (size_t i = 0; i < a.GetSize(); ++i)
          {
            if (a[i].IsString())
            {
              PoDoFo::PdfString curtext = a[i].GetString();
              PoDoFo::PdfString curtextu = curtext.GetStringUtf8();
              std::string tempt;
              PoDoFo::PdfVariant converter(curtextu);
              converter.ToString(tempt);
              //std::cout<<tempt;
              //tempt = thornSub(tempt);
              tempt = tempt.substr(1,tempt.length()-2);
              /*testout<<tempt<<" :--:";
              const char * tempchar = tempt.c_str();
              for (int k = 0; k < tempt.length(); k++)
                testout<<(int) tempchar[k] << "..";
              testout<<'\n';*/
              text+=  tempt;
              //std::cout<<tempt<<'\n';
              //printr(tempt);
            }
            else if (a[i].IsNumber()){
              int number = (int) a[i].GetNumber();
              if (number < -100)
                text+=' ';
              //std::cout<<number;//<<'\n';
              //text+=" ";
            }
            else if (a[i].IsReal()){
              double number = (int) a[i].GetReal();
              //std::cout<<number;//<<'\n';
              if (number < -100)
                text+=' ';
            }
          }

        }
        else
        {
          if (var.IsString())
          {
            PoDoFo::PdfString curtext = var.GetString();
            std::string tempt;
            PoDoFo::PdfVariant converter(curtext);
            converter.ToString(tempt);
            text+= tempt.substr(1,tempt.length()-2);
          }
        }
        if (text.length()==0)
          continue;
        else
          page_text += text + ' ';
          //std::cout<<text<<'\n';

      }
    }
    //std::cout<<page_text<<'\n';
    //writeIntChars(page_text);
    page_text = ligatureSub(page_text);
    page_text = removeNonAlpha(page_text);

    //std::cout<<page_text<<'\n';
    if (N==1)
      parse_page_text(&dictionary,page_text,trimCutoff);
    else
      parse_page_text_ngram(&dictionary,page_text,N,trimCutoff);
  }
  //testout.close();
  trimToTopN(&dictionary,150);
  combine_dictionaries(mainDict,&dictionary);
  //read_dictionary(&dictionary);
}
