#include "text_manage.h"


const std::string OUTER_STOPWORDS_FILENAME = "outer_stopwords.txt";
const std::string INNER_STOPWORDS_FILENAME = "inner_stopwords.txt";

const int NGRAM_DICTIONARY_MAX_SIZE = 120000;

std::unordered_set<std::string> OUTER_STOPWORDS;

std::unordered_set<std::string> INNER_STOPWORDS;


void printStringVector(std::vector<std::string> strv)
{
  for(std::vector<std::string>::iterator it = strv.begin();
   it != strv.end();++it)
    std::cout<<*it<<'\n';

}

//http://stackoverflow.com/a/15813530/1362215
bool nonAscii(char c)
{
return (c>127 || c<0);
}

bool isAlpha(char c)
{
  return ((c==32||(c>=65 && c <= 122))&&!(c>=91&&c<=96));
}

void print(std::string &str)
{
    std::cout<<str<<'\n';
    return;
}

void printr(std::string str)
{
    std::cout<<str<<'\n';
    return;
}


bool string_in_vector_c(const std::string &str,const std::vector<std::string> &arr)
{
  return std::find(arr.begin(),arr.end(),str) != arr.end();
}

bool string_in_vector(const std::string &str, std::vector<std::string> &arr)
{
  return std::find(arr.begin(),arr.end(),str) != arr.end();
}

//these methods use up a decent amount of memory if you are using very large strings
//limit usage to page
std::string removeNonAscii(std::string& str)
{
  std::string result;
  for (size_t i = 0; i<str.size();++i)
  {
    if (!nonAscii(str[i]))
      result+=str[i];
  }
  return result;
}

std::string removeNonAlpha(std::string& str)
{
  std::string result;
  bool prev_space = false;
  for (size_t i = 0; i<str.size();++i)
  {
    if (str[i]==32 && !prev_space)
    {
      prev_space = true;
      result +=' ';
    }
    else if (str[i]==32)
      continue;
    else if(isAlpha(str[i]))
    {
      prev_space = false;
      result+=tolower(str[i]);
    }
  }
  return result;

}

void negatizeArray(long * input, int arraySize)
{
  for (int i = 0; i < arraySize;i++)
    input[i] = -input[i];
}

void initializeStopwords()
{
  std::ifstream outer(OUTER_STOPWORDS_FILENAME.c_str());
  std::string word;

  while (!outer.eof())
  {
    getline(outer,word);
    OUTER_STOPWORDS.insert(removeNonAlpha(word));
  }
  std::ifstream inner(INNER_STOPWORDS_FILENAME.c_str());
  while (!inner.eof())
  {
    getline(inner,word);
    INNER_STOPWORDS.insert(removeNonAlpha(word));
  }
}

void read_set(std::unordered_set<std::string> *sset)
{
  for (std::unordered_set<std::string>::iterator it = sset->begin();
   it!=sset->end();++it)
    std::cout<<*it<<'\n';
}

void print(long * input,int max_length)
{
  for (int i = 0; i < max_length; i++)
    std::cout<<input[i]<<' ';
  std::cout<<'\n';
}

int partition(long* input, int p, int r)
{
  int pivot = input[r];
  while(p<r)
  {
    while (input[p] < pivot)
      p++;
    while (input[r] > pivot)
      r--;
    if (input[p] == input[r])
      p++;
    else if (p < r)
    {
      int tmp = input[p];
      input[p] = input[r];
      input[r] = tmp;
    }
  }
  return r;
}


int quick_select(long* input, int p, int r, int k)
{
  if (p==r) return input[p];
  int j = partition(input, p, r);
  int length = j - p + 1;
  if (length==k) return input[j];
  else if (k<length) return quick_select(input,p,j-1,k);
  else return quick_select(input,j+1,r,k-length);
}

//allows for pruning of ngrams if dict size is
//to large
void trimUnderCutoff(gramDict* dict, int lower_cutoff,int reducer)
{
  for (gramDict::const_iterator current = dict->begin();
   current != dict->end();++current)
  {
    if (current->second <= lower_cutoff)
      dict->erase(current->first);
    else
      (*dict)[current->first] -= reducer;//fixed bug from ++(*dict) 05/16/2016
  }

}


void trimToTopN(gramDict* dict,int n,int lower_cutoff)
{
 //gramDict::iterator current(dict->begin());
 //first step to drastically reduce size of dictionary
 //also does stopwords
  for (gramDict::const_iterator current = dict->begin();
   current != dict->end();++current)
  {
    if (current->second <= lower_cutoff)
    {
      dict->erase(current->first);
      //std::cout<<"something is done\n";
    }
    else if (OUTER_STOPWORDS.count(current->first)>0)
      dict->erase(current->first);
  }
  //write values to array for values that are not cut off

  int dsize = dict->size();
  long * values;
  values = new long[dsize];
  int i = 0;
  //std::cout<<"STEP 2\n";
  gramDict::iterator current2(dict->begin());
  //std::cout<<"STEP 2.5\n";
  while (current2 != dict->end())
  {
    values[i++]=current2->second;
    //std::cout<<i<<'.';
    ++current2;
  }
  //print(values,dsize);
  if (dsize>n)
  {
    //std::cout<<"STEP 3\n";
    negatizeArray(values,dsize);
    quick_select(values, 0 , i, n);
    negatizeArray(values,n);
    gramDict::iterator current3(dict->begin());
    long minval = values[n-1];
    //std::cout<<"MINVAL: "<<minval<<'\n';

    while (current3 != dict->end())
    {
      if (current3->second < minval)
        dict->erase(current3->first);
      ++current3;
    }

  }
  //std::cout<<"-----";
  //print(values,n);

}


//takes a semi-large string and creates a string vector from it
void parse_page_text(gramDict *dict,std::string page_text, int trimCutoff)
{
  //std::cout<<"parsing page text\n";
  std::string current_string = "";
  for (size_t i = 0; i<page_text.size();++i)
  {
    char pti = page_text[i];
    if (pti==32)
    {
      if (OUTER_STOPWORDS.count(current_string)==0)
      {
        //std::cout<<"wololo\n";
        ++(*dict)[current_string];
      }
      current_string="";
    }
    else
      current_string+=pti;
  }
  //std::cout<<dict->size()<<'\n';
}

std::string chainStringArray(std::string* stringArray, int N)
{
  std::string result = "";
  for (int i = 0; i<N;i++)
    if (i==0)
      result+=stringArray[i];
    else
    {
      result += ' ';
      result +=stringArray[i];
    }
  return result;

}

bool checkNGram(std::string *word_bank,int N)
{
  for (int i = 0; i < N; i++)
    if ((i==0||i==N-1)&&OUTER_STOPWORDS.count(word_bank[i])>0)
      return false;
    else if ((i>0 && i <N-1) &&INNER_STOPWORDS.count(word_bank[i])>0)
      return false;
  return true;

}

//used to process n-grams
//under construction
void parse_page_text_ngram(gramDict *dict,
 std::string page_text,int N,int trimCutoff)
{
  //if a negative N is specified
  //this will run the algorithm for all positive integer n <= (-N)
  //this makes the program about Nx faster otherwise
  if (N<0)
  {
    parse_page_text(dict,page_text,trimCutoff);
    for (int tN = 2; tN < -N+1;tN++)
      parse_page_text_ngram(dict,page_text,tN,trimCutoff);
    return;
  }
  std::string current_string = "";
  std::string * word_bank;
  word_bank = new std::string[N];
  //std::cout<<"CREATED WORD BANK\n";
  int wordsUsed = 0;
  for (size_t i = 0; i<page_text.size();++i)
  {
    char pti = page_text[i];
    if (pti==32)
    {
      if (true)//changed because errors were possible before
      {
        //std::cout<<"ADDING WORD\n";
        wordsUsed++;
        if (wordsUsed>N)
          wordsUsed = N;
        for (int j = 0; j<wordsUsed-1;j++)
          word_bank[j]=word_bank[j+1];
        word_bank[N-1] = current_string;
        if (wordsUsed==N && checkNGram(word_bank,N))
          ++(*dict)[chainStringArray(word_bank,N)];
      }
      current_string="";
    }
    else
      current_string+=pti;
  }
  //std::cout<<"FINISHED LOOP\n";
  //reduces dictionary size if there are too many entries
  if (dict->size() > NGRAM_DICTIONARY_MAX_SIZE)
  {
    trimUnderCutoff(dict,5,1);
    std::cout<<"REDUCED DICT SIZE\n";
  }
  //clean up word bank memory
  for (int i = 0; i<N; i++)
    word_bank[i].clear();

}

void read_dictionary(gramDict * dict)
{
  gramDict::const_iterator current((*dict).begin());
  while (current != (*dict).end())
  {
    std::cout<< current->first << ": " << current->second<<'\n';
    ++current;
  }
}

tfidfDict tfIdf(gramDict * mainDict,gramDict * documentDict)
{
  tfidfDict newDict;
  for (gramDict::const_iterator current = documentDict->begin();
   current != documentDict->end();current++)
    newDict[current->first] = ((double) current->second)/((double) (*mainDict)[current->first]);
  return newDict;
}

//does not assume independent elements
//moves elements to first dictionary
void combine_dictionaries(gramDict* dict1, gramDict* dict2)
{
  for (gramDict::const_iterator current = dict2->begin();
   current != dict2->end();++current)
    (*dict1)[current->first]+=current->second;

}

//assumes that elements of dictionaries are unique
//this will be used with different N values for ngram dictionaries
//moves elements into first dictionary
void merge_dictionaries(gramDict* dict1, gramDict* dict2)
{
  for (gramDict::const_iterator current = dict2->begin();
   current != dict2->end();++current)
  {
      dict1->insert(std::pair<std::string, int>(current->first, current->second));
  }
}

std::string getLastNLetters(std::string str, int N)
{
  int n = str.length();
  if (n<=N)
    return str;
  return str.substr(n-N,n-1);

}

std::string removeLastNLetters(std::string str, int N)
{
  int n = str.length();
  if (n<=N)
    return "";
  else
    return str.substr(0,n-N);

}

//"ing", "e\ing
void mergeGerund(gramDict * dict)
{
  std::string gerund = "ing";
  for (gramDict::iterator current = dict->begin();
     current != dict->end();++current)
  {
    std::string text = current->first;
    int n = text.length();
    if (n < 6)
      continue;
    std::string endText = getLastNLetters(text,3);
    if (strcmp(endText.c_str(),gerund.c_str())==0)
    {
      std::string beginText = removeLastNLetters(text,3);
      if (dict->count(beginText) > 0)
      {
        (*dict)[beginText]+=current->second;
        dict->erase(current->first);
      }
      else//check for double letters, e.g., "wi*nn*ing"| also e-removed gerunds, like scraping
      {
        const char * ctext = text.c_str();
        if (ctext[n-4]==ctext[n-5])
        {
          beginText = removeLastNLetters(beginText,1);
          if (dict->count(beginText) > 0)
          {
            dict->at(beginText)+=current->second;
            dict->erase(current->first);
          }
        }
        else
        {
          std::string theoText = removeLastNLetters(text,3)+"e";
          if (dict->count(theoText)>0)
          {
            dict->at(theoText)+=current->second;
            dict->erase(current->first);
          }

        }
      }
    }
  }
}

//detects "plural" elements of dictionaries and combines them together
void mergePlural(gramDict * dict)
{
  std::string plural1 = "s";
  std::string plural2 = "es";
  std::string plural3 = "ies";
  for (gramDict::iterator current = dict->begin();
   current != dict->end();++current)
  {
    std::string text = current->first;
    if (text.length() < 3)
      continue;
    std::string endText = getLastNLetters(text,2);
    std::string endText3 = getLastNLetters(text,3);
    if (text.length() > 5 && strcmp(plural3.c_str(),endText3.c_str())==0)
    {
      std::string theoText = removeLastNLetters(text,3)+"y";
      if (dict->count(theoText)> 0)
      {
        //std::cout<<"Y NOT?\n";
        dict->at(theoText)+=current->second;
        dict->erase(current->first);
        continue;
      }
    }
    else if (strcmp(endText.c_str(),plural2.c_str())==0)
    {
      std::string smallText = removeLastNLetters(text,2);
      if (dict->count(smallText)>0)
      {
        //std::cout<"ES OK?\n";
        dict->at(smallText)+=current->second;
        dict->erase(current->first);
        continue;
      }
    }
    if (strcmp(getLastNLetters(text,1).c_str(),plural1.c_str())==0)
    {
      std::string smallText = removeLastNLetters(text,1);
      if (dict->count(smallText)>0)
      {
        //std::cout<<"SARCASM\n";
        dict->at(smallText)+=current->second;
        dict->erase(current->first);
      }
    }
  }
}

void basicNLP(gramDict * dict)
{
  mergePlural(dict);
  mergeGerund(dict);
}

void outputDictionary(gramDict * dict,std::string filename)
{
  std::fstream outfile;
  outfile.open(filename.c_str(),std::fstream::out);
  std::cout<<filename<<'\n';
  for (gramDict::const_iterator current = dict->begin();
   current != dict->end();++current)
   {
    outfile<<current->first << ",";
    outfile << current->second<<'\n';
  }
  outfile.close();
}

//used to create file for indexed files
void convertCountToIndex(gramDict * dict)
{
  int i = 0;
  for (gramDict::const_iterator current = dict->begin();
   current != dict->end();++current)
  {
    dict->at(current->first) = i;
    ++i;
  }
}

void writeKeywordDataFile(std::vector<gramDict> * dictVectorP,gramDict * mainDict,std::string filename, bool includeCount)
{
  std::fstream outfile;
  outfile.open(filename.c_str(),std::fstream::out);
  int i = 0;
  for (std::vector<gramDict>::const_iterator it = dictVectorP->begin();
   it != dictVectorP->end();++it)
  {
    for (gramDict::const_iterator current = it->begin();current != it->end();
     ++current)
    {
      outfile<<i<<',';
      if (includeCount)
        outfile<<mainDict->at(current->first)<<','<<current->second<<'\n';
      else
       outfile<<mainDict->at(current->first)<<'\n';
    }
    ++i;
  }
  outfile.close();
  return;
}
