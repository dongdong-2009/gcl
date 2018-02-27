#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ccxx/ccxx.h>

#include <ccxx/qtcommon.h>









gzstreambuf* gzstreambuf::open( const char* name, int open_mode) {
    if ( is_open())
        return (gzstreambuf*)0;
    mode = open_mode;
    // no append nor read/write mode
    if ((mode & std::ios::ate) || (mode & std::ios::app)
        || ((mode & std::ios::in) && (mode & std::ios::out)))
        return (gzstreambuf*)0;
    char  fmode[10];
    char* fmodeptr = fmode;
    if ( mode & std::ios::in)
        *fmodeptr++ = 'r';
    else if ( mode & std::ios::out)
        *fmodeptr++ = 'w';
    *fmodeptr++ = 'b';
    *fmodeptr = '\0';
    file = gzopen( name, fmode);
    if (file == 0)
        return (gzstreambuf*)0;
    opened = 1;
    return this;
}

gzstreambuf * gzstreambuf::close() {
    if ( is_open()) {
        sync();
        opened = 0;
        if ( gzclose( file) == Z_OK)
            return this;
    }
    return (gzstreambuf*)0;
}

int gzstreambuf::underflow() { // used for input buffer only
    if ( gptr() && ( gptr() < egptr()))
        return * reinterpret_cast<unsigned char *>( gptr());

    if ( ! (mode & std::ios::in) || ! opened)
        return EOF;
    // Josuttis' implementation of inbuf
    int n_putback = gptr() - eback();
    if ( n_putback > 4)
        n_putback = 4;
    memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread( file, buffer+4, bufferSize-4);
    if (num <= 0) // ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg( buffer + (4 - n_putback),   // beginning of putback area
          buffer + 4,                 // read position
          buffer + 4 + num);          // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *>( gptr());
}

int gzstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr() - pbase();
    if ( gzwrite( file, pbase(), w) != w)
        return EOF;
    pbump( -w);
    return w;
}

int gzstreambuf::overflow( int c) { // used for output buffer only
    if ( ! ( mode & std::ios::out) || ! opened)
        return EOF;
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if ( flush_buffer() == EOF)
        return EOF;
    return c;
}

int gzstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if ( pptr() && pptr() > pbase()) {
        if ( flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------
// class gzstreambase:
// --------------------------------------

gzstreambase::gzstreambase( const char* name, int mode) {
    init( &buf);
    open( name, mode);
}

gzstreambase::~gzstreambase() {
    buf.close();
}

void gzstreambase::open( const char* name, int open_mode) {
    if ( ! buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
}

void gzstreambase::close() {
    if ( buf.is_open())
        if ( ! buf.close())
            clear( rdstate() | std::ios::badbit);
}





using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->setWindowTitle(QtCommonString::gbkToQString(CxApplication::applicationFileName()));
}



int64 fn_compress_path(const std::string & sPath, const std::string &sOutFilePath)
{
    int64 r = 0;
    const char * sOutFile = sOutFilePath.c_str();

    ogzstream    out2;
    out2.open( sOutFile);
    if ( ! out2.good())
    {
        std::cerr << "ERROR: Opening file `" << sOutFile << "' failed.\n";
        return r;
    }
    out2.close();
    if ( ! out2.good())
    {
        std::cerr << "ERROR: Closing file `" << sOutFile << "' failed.\n";
        return r;
    }
    // now use the shorter way with the constructor to open the same file
    ogzstream  out( sOutFile);
    if ( ! out.good())
    {
        std::cerr << "ERROR: Opening file `" << sOutFile << "' failed.\n";
        return r;
    }

    vector<CxFilePathInfo> pathInfos;
    CxFileSystem::scanDir(sPath, pathInfos);
    for (size_t i = 0; i < pathInfos.size(); ++i)
    {
        const CxFilePathInfo & filePathInfo = pathInfos.at(i);

        const char * sInFile = filePathInfo.fileName.c_str();
        std::ifstream in;
        in.open( sInFile , ios::in | ios::binary );

        char c = in.get();
        while (in.good())
        {
            out << c;
            c = in.get();
        }

        in.close();
        if ( ! in.eof())
        {
            std::cerr << "ERROR: Reading file `" << sInFile << "' failed.\n";
            return r;
        }
    }

    out.close();
    if ( ! out.good())
    {
        std::cerr << "ERROR: Writing file `" << sOutFile << "' failed.\n";
        return r;
    }
    return r;
}

void MainWindow::on_pushButton_2_clicked()
{
    char sInFile[] = "c:/0.vsd";
    char sOutFile[] = "c:/0.zip";

    std::cerr << "Usage: c:/0.vsd <in-file> <out-file>\n";

    // check alternate way of opening file
    ogzstream    out2;
    out2.open( sOutFile);
    if ( ! out2.good()) {
        std::cerr << "ERROR: Opening file `" << sOutFile << "' failed.\n";
    return;
    }
    out2.close();
    if ( ! out2.good()) {
        std::cerr << "ERROR: Closing file `" << sOutFile << "' failed.\n";
    return;
    }
    // now use the shorter way with the constructor to open the same file
    ogzstream  out( sOutFile);
    if ( ! out.good()) {
        std::cerr << "ERROR: Opening file `" << sOutFile << "' failed.\n";
    return;
    }
    std::ifstream in;
    in.open( sInFile , ios::in | ios::binary );

    char c = in.get();
    while (in.good()) {
        out << c;
        c = in.get();
    }
    in.close();
    out.close();
    if ( ! in.eof()) {
        std::cerr << "ERROR: Reading file `" << sInFile << "' failed.\n";
    return;
    }
    if ( ! out.good()) {
        std::cerr << "ERROR: Writing file `" << sOutFile << "' failed.\n";
    return;
    }
    return;
}
