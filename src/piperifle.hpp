#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define PIPERIFLE_EXPORT __declspec(dllexport)
#else
  #define PIPERIFLE_EXPORT
#endif

PIPERIFLE_EXPORT void piperifle();
