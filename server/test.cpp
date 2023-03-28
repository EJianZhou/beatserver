#include <chrono>
#include <functional>
#include <thread>
#include <iostream>
using namespace std;

void myFunction() {
    cout << "B" << endl; 
}
void f() {
    cout << "A" << endl;
}

int main() {

    std::function<void()> func = std::bind(&myFunction);

    while (true) {
        f();

        std::chrono::steady_clock::time_point timePoint =
            std::chrono::steady_clock::now() + std::chrono::minutes(1);

        std::this_thread::sleep_until(timePoint);

        func();
    }
    return 0;
}
