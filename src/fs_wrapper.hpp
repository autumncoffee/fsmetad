#pragma once

#ifdef RELEASE_FILESYSTEM
#include <filesystem>

namespace stdfs = std::filesystem;

#else
#include <experimental/filesystem>

namespace stdfs = std::experimental::filesystem;
#endif
