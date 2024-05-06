
#include "app.hpp"
#include "fmt/base.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>

auto main(int argc, char *argv[]) -> int {
    if (argc != 2) {
        fmt::println(stderr, "Provide path to shaders directory");
        std::abort();
    }
    App{}.run(std::filesystem::path{argv[1]});  // NOLINT
}
