#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main (int argc, char* const argv[])
{
    const auto result = Catch::Session ().run (argc, argv);
    system ("pause");
    return result;
}
