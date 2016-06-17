#ifndef PDF_MANAGE_H
#define PDF_MANAGE_H

#include <podofo/podofo.h>
#include <algorithm>
#include "text_manage.h"

const std::vector<std::string> valid_tokens{"\"","'","Tj","TJ"};

void writeIntChars(std::string str);

//temporary solution for weird "fi" substitutions
std::string ligatureSub(std::string str);

void read_pdf(std::string filename, gramDict * mainDict, int N = 1,int trimCutoff=150);

#endif PDF_MANAGE_H
