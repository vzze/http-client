#include <wrequest.hh>
#include <iostream>

int main() {
    std::clog << req::send(req::server{L"https://www.youtube.com/"}, req::request{L"GET"}) << '\n';
}
