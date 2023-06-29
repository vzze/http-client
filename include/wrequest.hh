#ifndef WREQUEST_HH
#define WREQUEST_HH

#include <stdexcept>
#include <exception>
#include <string>
#include <memory>

#ifdef _WIN32

#include <windows.h>
#include <winhttp.h>
#include <winnls.h>
#include <wincrypt.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "normaliz.lib")
#pragma comment(lib, "crypt32.lib")

#else

#endif

namespace req {
    struct server {
        bool is_https;
        std::wstring name;
        std::wstring page;

        server(const std::wstring & link);
    };

    struct request {
        std::wstring type;
        std::string payload;

        request(const std::wstring &, const std::string & = "");
    };

    std::string send(const server, const request);
}

#endif
