#include <wrequest.hh>
#include <iostream>

int main() {
    std::clog << req::send(req::server{L"http://info.cern.ch/"}, req::request{L"GET"}) << '\n';
}
