#include "html_manage.h"



void navigate_html(xmlNode * a_node,
 xmlDocPtr doc,gramDict* dict, int N)
{
  xmlNode *cur_node = nullptr;
  for (cur_node = a_node; cur_node;cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      //printf("node type: Element, name: %s\n",
//      cur_node->name);
      xmlChar * key;
      key = xmlNodeListGetRawString(doc,cur_node,1);
      if (key!=nullptr)
      {
        std::string str;
        str += (char*) key;
        str=removeNonAlpha(str);
        if (str.length()>1)
        {
          //std::cout<<str<<"\n===\n";
          if (N==1)
            parse_page_text(dict,str);
          else
            parse_page_text_ngram(dict,str,N);
        }
      }
      xmlFree(key);
    }
    navigate_html(cur_node->children,doc,dict,N);
  }
}

void read_html(std::string filename,gramDict * mainDict, int N,int trimCutoff)
{
  xmlDocPtr doc;
  xmlNodePtr cur;
  gramDict dictionary;
  doc = xmlParseFile(filename.c_str());

  if (doc==nullptr)
  {
    fprintf(stderr,"Document not parsed successfully. \n");
    return;
  }
  cur = xmlDocGetRootElement(doc);

  if (cur==nullptr)
  {
    fprintf(stderr,"empty document\n");
    xmlFreeDoc(doc);
    return;
  }
  navigate_html(cur,doc,&dictionary,N);
  xmlFreeDoc(doc);
  xmlCleanupParser();
  trimToTopN(&dictionary,trimCutoff,5);
  //read_dictionary(&dictionary);
  combine_dictionaries(mainDict,&dictionary);
}
