/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: ChatPanel
//
// Created on: Wed May  2 21:07:18 2007
//

#include <wx/intl.h>
#include <wx/datetime.h>
#include "chatpanel.h"
#include "springlobbyapp.h"

BEGIN_EVENT_TABLE(ChatPanel, wxPanel)

  EVT_TEXT_ENTER ( CHAT_TEXT, ChatPanel::OnSay )
  EVT_BUTTON     ( CHAT_SEND, ChatPanel::OnSay )

END_EVENT_TABLE()


//! @brief ChatPanel constructor.
//!
//! @param parent the parent wxWindow.
//! @param show_nick_list set to true if the nick list should be created.
//! @todo Fix nick list
//! @todo Enlarge the say input.
ChatPanel::ChatPanel( wxWindow* parent, bool show_nick_list ) : wxPanel( parent, -1 )
{
  // Setting default values
  m_show_nick_list = show_nick_list;
  m_chan_name = "";
  
  // Creating sizers
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_chat_sizer = new wxBoxSizer( wxVERTICAL );
  m_say_sizer = new wxBoxSizer( wxHORIZONTAL );
  
  if ( m_show_nick_list ) {
    
    m_splitter = new wxSplitterWindow( this , -1, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_nick_panel = new wxPanel( m_splitter, -1 );
    m_chat_panel = new wxPanel( m_splitter, -1 );
    
    m_nick_sizer = new wxBoxSizer( wxVERTICAL );
    
    m_nicklist = new wxListCtrl( m_nick_panel, -1, wxDefaultPosition, wxDefaultSize, 
                                wxLC_REPORT | wxLC_VIRTUAL | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
    
    m_nick_filter = new wxComboBox( m_nick_panel, -1, _("Show all"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    
    m_nick_sizer->Add( m_nicklist, 1, wxEXPAND );
    m_nick_sizer->Add( m_nick_filter, 0, wxEXPAND );
    
    m_nick_panel->SetSizer( m_nick_sizer );
    
  } else {
    
    m_chat_panel = this;
    m_nick_sizer = NULL;
    m_nicklist = NULL;
    m_nick_filter = NULL;
    m_splitter = NULL;
    
  }
  
  // Creating ui elements
  m_chatlog_text = new wxTextCtrl( m_chat_panel, -1, _(""), wxDefaultPosition, wxDefaultSize, 
                             wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );

  m_say_text = new wxTextCtrl( m_chat_panel, CHAT_TEXT, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
  m_say_button = new wxButton( m_chat_panel, CHAT_SEND, _T("Send") );
  
  // Adding elements to sizers
  m_say_sizer->Add( m_say_text, 1, wxEXPAND );
  m_say_sizer->Add( m_say_button );
  
  m_chat_sizer->Add( m_chatlog_text, 1, wxEXPAND );
  m_chat_sizer->Add( m_say_sizer, 0, wxEXPAND );
  
  if ( m_show_nick_list ) {
    m_chat_panel->SetSizer( m_chat_sizer );
    
    m_splitter->SplitVertically( m_chat_panel, m_nick_panel, 100 );
    
    m_main_sizer->Add( m_splitter, 1, wxEXPAND );
    
  } else {
    m_main_sizer->Add( m_chat_sizer, 4, wxEXPAND );
  }
  
  // Assign sizer to panel
  SetSizer( m_main_sizer );
  
  
  if ( m_show_nick_list ) {
    wxSize s = m_splitter->GetSize();
    m_splitter->SetSashPosition( s.GetWidth() - 180, true );
  }
}


//! @brief ChatPanel destructor.
ChatPanel::~ChatPanel()
{

}


void ChatPanel::OnSay( wxCommandEvent& event )
{
  Say( m_say_text->GetValue() );
  m_say_text->SetValue( _("") );
}

//! @brief Output a message said in the channel.
//!
//! @param who nick of the person who said something.
//! @param message the message to be outputted.
//! @todo Fix nicer format of chat messages.
void ChatPanel::Said( wxString who, wxString message )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxBLACK));
  LogTime();
  m_chatlog_text->AppendText( _(" ") + who + _(": ")+ message + _("\n") );
}


void ChatPanel::DidAction( wxString who, wxString action )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxBLACK));
  LogTime();
  m_chatlog_text->AppendText( _(" ") + who + _(" ") + action + _(".\n") );
}


//! @brief Output motd sent by server
//!
//! @param message The MOTD message to output
void ChatPanel::Motd( wxString message )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxBLUE));
  m_chatlog_text->AppendText( _(" ** motd ** ")+ message + _("\n") );
}


void ChatPanel::UnknownCommand( wxString command, wxString params )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxRED));
  m_chatlog_text->AppendText( _T(" !! Command: \"") + command + _T("\" params: \"") + params + _("\".\n") );
}


void ChatPanel::Joined( wxString who )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxGREEN));
  m_chatlog_text->AppendText( _(" ** ")+ who + _T(" joined the channel.\n") );
}


void ChatPanel::Parted( wxString who, wxString message )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxRED));
  m_chatlog_text->AppendText( _(" ** ")+ who + _T(" left the channel ( ") + message + _(" ).\n") );
}

void ChatPanel::SetTopic( wxString who, wxString message )
{
  m_chatlog_text->SetDefaultStyle(wxTextAttr(*wxBLUE));
  m_chatlog_text->AppendText( _(" ** Channel topic: ")+ message + _T("\n * Set by ") + who + _(".\n") );  
}

//! @brief Set name of the chat/channel.
//!
//! @param chan_name the name of the channel.
//! @note The name is used as a reference when talking to the Server class.
void ChatPanel::SetChannelName( const string chan_name )
{
  m_chan_name = chan_name;
}


//! @brief Get name of the chat/channel.
//!
//! @return Name of the chat/channel.
string ChatPanel::GetChannelName()
{
  return m_chan_name;
}


//! @brief Check if ChatPanel controls a server output window.
//!
//! @return true if ChatPanel controls a server output window else false.
bool ChatPanel::IsServerPanel()
{
  return (m_chan_name == string(SERVER_CHAT_NAME));
}


void ChatPanel::Say( wxString message )
{
  cout << "** ChatPanel::Say()" << endl;
  Server* serv = app().Serv();
  assert( serv != NULL );
  serv->SayChannel( m_chan_name, STL_STRING(message) );
}

void ChatPanel::Part()
{
  cout << "** ChatPanel::Part()" << endl;
  Server* serv = app().Serv();
  assert( serv != NULL );
  app().RemoveChannelPanel( m_chan_name );
  serv->PartChannel( m_chan_name );
}

void ChatPanel::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  m_chatlog_text->AppendText( _("[") + now.Format( _("%H:%M") ) + _("]") );

}

