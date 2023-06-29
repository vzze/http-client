#include <wrequest.hh>

req::server::server(const std::wstring & link) {
    is_https = link.find(L"http://") != 0;

    const auto pos = link.find(L"://");

    std::wstring::size_type page_pos;

    if(pos == std::wstring::npos) {
        page_pos = link.find(L"/");
        name = link.substr(0, page_pos);
    } else {
        page_pos = link.find(L"/", pos + 3);
        name = link.substr(pos + 3, page_pos - pos - 3);
    }

    if(page_pos == std::string::npos)
        page = L"/";
    else
        page = link.substr(page_pos);
}
