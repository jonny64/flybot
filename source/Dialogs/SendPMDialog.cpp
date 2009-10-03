#include "stdwx.h"
#include "SendPMDialog.h"

SendPMDialog::SendPMDialog( wxWindow* parent )
:
SendPMDialogBase( parent )
{

}

void SendPMDialog::OnCloseDialog( wxCloseEvent& event )
{
    event.Skip();
}

void SendPMDialog::OnInitDialog( wxInitDialogEvent& WXUNUSED(event) )
{
	// TODO: Implement OnInitDialog
}

void SendPMDialog::OnTextComboNick( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: Implement OnTextComboNick
}

void SendPMDialog::OnSendClick( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: Implement OnSendClick
}
