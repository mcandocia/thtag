#include <wx/wx.h>
#include <wx/string.h>

class Toolbar : public wxFrame
{
public:
  Toolbar(const wxString& title);

  void OnQuit(wxCommandEvent& event);


};


