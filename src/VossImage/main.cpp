
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "pch.h"
#include "MathFunctions.h"
#include "VossHeightmap.h"
#include "Image.h"
#include "PngImage.h"
#include "IHeightmapRenderer.h"
#include "HeightmapRender.h"

static const size_t g_LandscapeSize = 7; // 2^7 = 128x128 heightmap

struct ParamsHelper
{
    bool showHelp;
    std::string outputFile;
    int imageWidth, imageHeight;
    int worldPosX, worldPosY;

    ParamsHelper()
        : showHelp(false)
        , imageWidth(1024)
        , imageHeight(768)
        , worldPosX(10)
        , worldPosY(10)
    {
    }
};

void Usage(char cmdline[])
{
    std::cout
        << "Usage: " << cmdline << " [options] <output file name>" << std::endl
        << "Options:" << std::endl
        << "\t-h,--help\t\tShow this help message" << std::endl
        << "\t-r,--resolution WxH\tOutput image resolution. Default is 1024x768" << std::endl
        << "\t-w,--worldpos X,Y\tHeightmap X and Y coords. Default is 10,10" << std::endl
        << std::endl;
}

bool ParseArguments(int argc, char* argv[], ParamsHelper& helper)
{
    int i;
    for (i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            helper.showHelp = true;
            return true;
        }
        else if ((arg == "-r") || (arg == "--resolution")) {
            if (i + 1 < argc) {
                // Make sure we aren't at the end of argv!
                // Increment 'i' so we don't get the argument as the next argv[i].
                std::string str = argv[++i];
                std::regex re("^([0-9]+)[xX\\*]([0-9]+)$");
                std::smatch match;
                if (std::regex_match(str, match, re) && match.size() > 1) {
                    helper.imageWidth = std::stoi(match[1]);
                    helper.imageHeight = std::stoi(match[2]);
                }
                else {
                    std::cerr << "'" << str << "' argument is invalid for the resolution." << std::endl;
                    return false;
                }
            }
            else {
                // Uh-oh, there was no argument to the destination option.
                std::cerr << arg << " option requires one argument." << std::endl;
                return false;
            }  
        }
        else if ((arg == "-w") || (arg == "--worldpos")) {
            if (i + 1 < argc) {
                std::string str = argv[++i];
                std::regex re("^(-?[0-9]+)\\,(-?[0-9]+)$");
                std::smatch match;
                if (std::regex_match(str, match, re) && match.size() > 1) {
                    helper.worldPosX = std::stoi(match[1]);
                    helper.worldPosY = std::stoi(match[2]);
                }
                else {
                    std::cerr << "'" << str << "' argument is invalid for the world position." << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << arg << " option requires one argument." << std::endl;
                return false;
            }  
        }
    }
    // Last argument is output file name
    helper.outputFile.assign(argv[i]);

    return true;
}

/*****************************************************************************
 * main
 ****************************************************************************/
int main(int argc, char* argv[])
{
    if (argc < 2) {
        Usage(argv[0]);
        return 1;
    }

    ParamsHelper params;
    if (!ParseArguments(argc, argv, params)) {
        std::cerr << "Failed to parse parameters" << std::endl;
        return 1;
    }
    if (params.showHelp) {
        Usage(argv[0]);
        return 0;
    }
    
    VossHeightmap map(g_LandscapeSize);
    PngImage image(params.imageWidth, params.imageHeight);
    HeightmapRender render(&map, &image);

    map.generate(params.worldPosX, params.worldPosY);
    render.draw();
    image.saveToFile(params.outputFile);
    
    std::cout << "Image Formed:\t" << params.imageWidth << "x" << params.imageHeight << std::endl;
    std::cout << "World Position:\t" << params.worldPosX << "," << params.worldPosY << std::endl;
    std::cout << "Output File:\t" << params.outputFile << std::endl << std::endl;

    return 0;
}

