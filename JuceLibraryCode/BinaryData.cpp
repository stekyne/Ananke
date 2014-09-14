/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== TODOlist.txt ==================
static const unsigned char temp_binary_data_0[] =
"Audio Widgets TODO list\r\n"
"------------------------\r\n"
"\r\n"
"This project will serve as a front end interface to generate control data \r\n"
"for audio related projects. Audio Widgets will generate the control data \r\n"
"used for synthesis. For example, envelopes to control cutoff/resonance.\r\n"
"\r\n"
"Widgets to add:\r\n"
"[] Wrapper component with inputs and outputs shown\r\n"
"[] ADSR envelopes\r\n"
"[] LFO ";

const char* TODOlist_txt = (const char*) temp_binary_data_0;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x6db7dd95:  numBytes = 374; return TODOlist_txt;
        default: break;
    }

    numBytes = 0;
    return 0;
}

const char* namedResourceList[] =
{
    "TODOlist_txt"
};

}
