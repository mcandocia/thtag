#ifndef HTML_MANAGE_H
#define HTML_MANAGE_H
#include "text_manage.h"
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

void navigate_html(xmlNode * a_node,
 xmlDocPtr doc,gramDict* dict, int N);

void read_html(std::string filename,gramDict * mainDict, int N = 1,int trimCutoff=150);

#endif HTML_MANAGE_H
