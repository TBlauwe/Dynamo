#include <dynamo/dynamo.hpp>

int main(int argc, char** argv) {
    Dynamo dynamo;
    dynamo.run();
    dynamo.shutdown();
    return 0;
}