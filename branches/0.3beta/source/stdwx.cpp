#include "stdwx.h"

// TODO: remove after wxWidgets 2.9 release
// http://trac.wxwidgets.org/ticket/8370
wxArrayString wxSplit(const wxString& str, const wxChar sep, const wxChar escape) 
{ 
    if (escape == wxT('\0')) 
        // we don't need to honour the escape character 
        return wxStringTokenize(str, sep, wxTOKEN_RET_EMPTY_ALL /* return all tokens, even empty ones */); 

    wxArrayString ret; 
    wxString curr; 
    wxChar prev = wxT('\0'); 

    for (const wxChar *p = str.c_str(); *p != wxT('\0'); p++) 
    { 
        if ( *p == sep ) 
        { 
            if ( prev == escape ) 
            { 
                // remove the escape character and don't consider this 
                // occurrence of 'sep' as a real separator 
                curr.Last() = sep; 
            } 
            else 
            { 
                // add only non-empty tokens 
                ret.Add(curr); 
                curr.Empty(); 
            } 
        } 
        else 
            curr << *p; 


        prev = *p; 
    } 


    // add the last token 
    ret.Add(curr); 

    return ret; 

} 

// TODO: remove as soon as wxWidgets 2.9 released 
// http://trac.wxwidgets.org/ticket/9810
bool wxLaunchDefaultApplication(const wxString &document)
{
    wxString verb = wxT("open"); 
    int result = (int)ShellExecute(NULL, verb, document, NULL, NULL, SW_SHOWDEFAULT); 

    return result > 32;
}

int random(int max)
{
    if (max <= 0)
        return 0;

    int result = 0;
    int i = max;
    while (RAND_MAX < i )
    {
        srand(time(NULL));
        result += rand();
        i -= RAND_MAX;
    }
    srand(time(NULL));
    result += rand() % i;
        
    return result;
}
