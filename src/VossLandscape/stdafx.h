#pragma once

#ifdef _MSC_VER
# ifndef _USE_MATH_DEFINES
#  define _USE_MATH_DEFINES
# endif
#endif

#include <regex>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

