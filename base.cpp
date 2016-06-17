#include "wx/wx.h"
#include <iostream>
#include <cstdlib>
#include <wx/string.h>

int main(int argc, char **argv)
{
  std::cout << "Test Program" << std::endl;
  wxString str1 = wxT("Linux");
  wxString str2 = wxT("Operating");
  wxString str3 = wxT("System");

  wxString str = str1 + wxT(" ") + str2 + wxT(" ") + str3;

  wxPuts(str);
}
