#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#include <wx/string.h>
#include <wx/stackwalk.h>

#if wxUSE_STACKWALKER

class StackTrace : public wxStackWalker
{
  public:
    StackTrace() {}
    ~StackTrace() {}

    wxString GetStackTrace() { return StackTraceString; }
    wxString GetStackTraceHash();

  private:

    void OnStackFrame( const wxStackFrame& frame );

    wxString StackTraceString;
    wxString PartToHash;

};

StackTrace& stacktrace();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
