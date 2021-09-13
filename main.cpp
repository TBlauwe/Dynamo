#include "persona/persona.hpp"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char** argv) {
    doctest::Context ctx;
    ctx.setOption("abort-after", 5);  // default - stop after 5 failed asserts
    ctx.applyCommandLine(argc, argv); // apply command line - argc / argv
    ctx.setOption("no-breaks", true); // override - don't break in the debugger
    int res = ctx.run();              // run test cases unless with --no-run
    if (ctx.shouldExit())              // query flags (and --exit) rely on this
        return res;

    Dynamo dynamo;
    dynamo.run();
    dynamo.shutdown();
    return 0;
}

int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }
