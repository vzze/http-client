#include <wrequest.hh>
#include <iostream>

int main() {
    std::clog << req::send(req::server{L"https://raw.githubusercontent.com/vzze/fell/main/src/CMakeLists.txt"}, req::request{L"GET"}) << '\n';
}
