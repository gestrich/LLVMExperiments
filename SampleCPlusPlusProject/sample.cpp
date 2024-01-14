#include <iostream>
using namespace std;

class SimpleClass {
public:
    SimpleClass() {}

    void doSomething() {
        // Some functionality here
            cout << "Hello World!";
    }
};

int main() {
    SimpleClass example;
    example.doSomething();
    return 0;
}