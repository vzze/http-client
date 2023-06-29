#include <wrequest.hh>

std::string req::send(const server sv, const request re) {
#ifdef _WIN32
    BOOL results = FALSE;
    HINTERNET session = NULL, connect = NULL, request = NULL;

    struct hinternet_raii {
        HINTERNET handle1, handle2, handle3;

        ~hinternet_raii() {
            if(handle1) WinHttpCloseHandle(handle1);
            if(handle2) WinHttpCloseHandle(handle2);
            if(handle3) WinHttpCloseHandle(handle3);
        }
    } handler{session, connect, request};

    session = WinHttpOpen(
        L"W Req/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if(!session)
        throw std::runtime_error{"Could not create session."};

    WCHAR puny_code[255];

    IdnToAscii(0, sv.name.c_str(), -1, puny_code, 255);

    const INTERNET_PORT connection_type = (sv.is_https) ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_PORT;

    connect = WinHttpConnect(
        session,
        puny_code,
        connection_type,
        0
    );

    if(!connect)
        throw std::runtime_error{"Could not connect."};

    const DWORD secure_flags = (sv.is_https) ? WINHTTP_FLAG_SECURE : 0;

    request = WinHttpOpenRequest(
        connect,
        re.type.c_str(),
        sv.page.c_str(),
        L"HTTP/1.0",
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        secure_flags
    );

    if(!request)
        throw std::runtime_error{"Could not open request."};

    if(re.type == L"GET")
        results = WinHttpSendRequest(
            request,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0
        );
    else
        results = WinHttpSendRequest(
            request,
            L"Content-Type: application/x-www-form-urlencoded\r\n",
            (DWORD)-1,
            (LPVOID)(re.payload.c_str()),
            static_cast<DWORD>(re.payload.length()),
            static_cast<DWORD>(re.payload.length()),
            0
        );

    if(results) results = WinHttpReceiveResponse(request, NULL);
    else {
        if(GetLastError() != ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED)
            return {};

        auto my_store = CertOpenSystemStore(0, TEXT("MY"));

        if(!my_store)
            throw std::runtime_error{"Could not open certificate store."};

        auto cert_context = CertFindCertificateInStore(
            my_store,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            0,
            CERT_FIND_SUBJECT_STR,
            "Cert_subject_1",
            NULL
        );

        struct cert_raii {
            HCERTSTORE store;
            PCCERT_CONTEXT ctx;

            ~cert_raii() {
                if(ctx) CertFreeCertificateContext(ctx);
                if(store) CertCloseStore(store, 0);
            }
        } handler{my_store, cert_context};

        if(!cert_context)
            throw std::runtime_error{"Could not find desired certificate."};


        WinHttpSetOption(
            request,
            WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
            (LPVOID)(cert_context),
            sizeof(CERT_CONTEXT)
        );

        if(re.type == L"GET")
            results = WinHttpSendRequest(
                request,
                WINHTTP_NO_ADDITIONAL_HEADERS,
                0,
                WINHTTP_NO_REQUEST_DATA,
                0,
                0,
                0
            );
        else
            results = WinHttpSendRequest(
                request,
                L"Content-Type: application/x-www-form-urlencoded\r\n",
                (DWORD)-1,
                (LPVOID)(re.payload.c_str()),
                static_cast<DWORD>(re.payload.length()),
                static_cast<DWORD>(re.payload.length()),
                0
            );

        if(results) results = WinHttpReceiveResponse(request, NULL);
    }

    if(!results) return {};

    std::string res{};

    DWORD size = 0;
    DWORD downloaded = 0;

    do {
        if(!WinHttpQueryDataAvailable(request, &size))
            throw std::runtime_error{"Could not query data."};

        auto out_buffer = std::make_unique<char[]>(size + 1);

        ZeroMemory(out_buffer.get(), size + 1);

        if(!WinHttpReadData(request, static_cast<LPVOID>(out_buffer.get()), size, &downloaded))
            throw std::runtime_error{"Could not read data."};

        res += out_buffer.get();
    } while(size > 0);

    return res;
#else

#endif
}
