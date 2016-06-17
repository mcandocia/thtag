#ifndef TEXT_MANAGE_H
#define TEXT_MANAGE_H
#include <string>
#include <cctype>
#include <iostream>
#include <podofo/podofo.h>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <cstdlib>

typedef std::map<std::string,int> gramDict;

typedef std::map<std::string, double> tfidfDict;

void printStringVector(std::vector<std::string> strv);

bool nonAscii(char c);

bool isAlpha(char c);

void print(std::string &str);

void printr(std::string str);

bool string_in_vector_c(const std::string &str,const std::vector<std::string> &arr);

bool string_in_vector(const std::string &str, std::vector<std::string> &arr);

std::string removeNonAscii(std::string& str);

std::string removeNonAlpha(std::string& str);

void negatizeArray(long * input, int arraySize);

void initializeStopwords();

void read_set(std::unordered_set<std::string> *sset);

void print(long * input,int max_length);

int partition(long* input, int p, int r);

int quick_select(long* input, int p, int r, int k);

void trimUnderCutoff(gramDict* dict, int lower_cutoff,int reducer = 1);

void trimToTopN(gramDict* dict,int n,int lower_cutoff = 5);

void parse_page_text(gramDict *dict,std::string page_text,int trimCutoff=150);

std::string chainStringArray(std::string* stringArray, int N);

bool checkNGram(std::string *word_bank,int N);

void parse_page_text_ngram(gramDict *dict,
 std::string page_text,int N = 2, int trimCutoff = 100);

void read_dictionary(gramDict * dict);

//divides the occurences of terms in a document
//by their overall appearance frequency
tfidfDict tfIdf(gramDict * mainDict,gramDict * documentDict);

//this will probably be preferred over merge_dictionaries
void combine_dictionaries(gramDict * dict1, gramDict* dict2);

void merge_dictionaries(gramDict * dict1, gramDict* dict2);

std::string getLastNLetters(std::string str, int N);

std::string removeLastNLetters(std::string str, int N);

void mergeGerund(gramDict * dict);

void mergePlural(gramDict * dict);

void basicNLP(gramDict * dict);

void outputDictionary(gramDict * dict,std::string filename);

void convertCountToIndex(gramDict * dict);

void writeKeywordDataFile(std::vector<gramDict> * dictVectorP,gramDict * mainDict,std::string filename,bool includeCount = false);

#endif TEXT_MANAGE_H
