#include <fcgi_config.h>
#include <fcgiapp.h>

#include <ccxx/cxfile.h>

int main(void)
{
    FCGX_Init();
    int socket = FCGX_OpenSocket(":9000", 0);
    char* remote_addr;

    FCGX_Request request;
    if (FCGX_InitRequest(&request, socket, 0) != 0)
        return 1;

//    CxTime::currentMsepoch();
//    std::string s;
//    CxFile::load("c:/1.jpg", s);

    while (1) {
        FCGX_Accept_r(&request);
        remote_addr = FCGX_GetParam("REMOTE_ADDR", request.envp);

//        FCGX_PutS("Content-type: text/plain\n\n", request.out);
//        FCGX_PutS(remote_addr, request.out);
//        FCGX_PutS("\n", request.out);

//        FCGX_PutS("Content-Type: image/jpeg\n\n", request.out);

        std::string s;
    //    CxFile::load("c:/1.jpg", s);
        CxFile::load("c:/1.xml", s);

        FCGX_PutS("Content-Type: xml\n\n", request.out);
//        FCGX_PutS("Content-Type: text/xml\n\n", request.out);
        FCGX_PutStr(s.data(), s.size(), request.out);
        FCGX_PutS("\n", request.out);

        FCGX_Finish_r(&request);
    }
}
