///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdwx.h"

#include "SendPMDialogBase.h"

///////////////////////////////////////////////////////////////////////////

SendPMDialogBase::SendPMDialogBase( wxWindow* parent, 
                                   wxWindowID id, 
                                   const wxString& title, 
                                   const wxPoint& pos, 
                                   const wxSize& size, 
                                   long style ) : 
    wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,120 ), wxDefaultSize );
	
	wxBoxSizer* bSizerGlobal;
	bSizerGlobal = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->AddGrowableRow( 0 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticPM = new wxStaticText( this, wxID_ANY, _("PM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticPM->Wrap( -1 );
	fgSizerMain->Add( m_staticPM, 0, wxALL|wxEXPAND, 5 );
	
	m_PMtext = new wxTextCtrl( this, wxID_PM_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizerMain->Add( m_PMtext, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_staticNick = new wxStaticText( this, wxID_ANY, _("Nick"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticNick->Wrap( -1 );
	fgSizerMain->Add( m_staticNick, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizerNickSend;
	fgSizerNickSend = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizerNickSend->AddGrowableCol( 0 );
	fgSizerNickSend->SetFlexibleDirection( wxBOTH );
	fgSizerNickSend->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
    m_textNick = new wxTextCtrl( this, wxID_COMBO_NICK, _("Specify nickname"), wxDefaultPosition, wxDefaultSize); 
	fgSizerNickSend->Add( m_textNick, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonSend = new wxButton( this, wxID_BUTTON_SEND, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerNickSend->Add( m_buttonSend, 0, wxALL, 5 );
	
	fgSizerMain->Add( fgSizerNickSend, 1, wxEXPAND, 5 );
	
	bSizerGlobal->Add( fgSizerMain, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizerGlobal );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( SendPMDialogBase::OnCloseDialog ) );
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SendPMDialogBase::OnInitDialog ) );
	m_textNick->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SendPMDialogBase::OnTextComboNick ), NULL, this );
	m_textNick->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SendPMDialogBase::OnSendClick ), NULL, this );
	m_buttonSend->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SendPMDialogBase::OnSendClick ), NULL, this );
}

SendPMDialogBase::~SendPMDialogBase()
{
    // Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( SendPMDialogBase::OnCloseDialog ) );
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( SendPMDialogBase::OnInitDialog ) );
	m_textNick->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SendPMDialogBase::OnTextComboNick ), NULL, this );
	m_textNick->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( SendPMDialogBase::OnSendClick ), NULL, this );
	m_buttonSend->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SendPMDialogBase::OnSendClick ), NULL, this );
}
