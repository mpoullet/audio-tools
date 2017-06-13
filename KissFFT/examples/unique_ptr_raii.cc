// Taken from: https://www.codeproject.com/Articles/820931/Using-std-unique-ptr-RAII-with-malloc-and-free

#include <memory>

int main() {
    auto Data = std::unique_ptr<double, decltype(free) *> {
        reinterpret_cast<double *>(malloc(sizeof(double) * 50)),
        free
    };
    
    return 0;
}
