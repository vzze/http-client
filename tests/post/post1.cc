#include <wrequest.hh>
#include <iostream>

int main() {
    std::cout << req::send(req::server{L"http://httpbin.org/post"}, req::request(L"POST", "req=none")) << '\n';
}
