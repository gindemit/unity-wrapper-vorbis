#if _MSC_VER // this is defined when compiling with Visual Studio
#define EXPORT_API __declspec(dllexport) // Visual Studio needs annotating exported functions with this
#else
#define EXPORT_API // XCode does not need annotating exported functions, so define is empty
#endif

long EXPORT_API EncodePcmDataToFile(
    const char* filePath,
    const float* samples,
    const long samplesLength,
    const short channels,
    const long frequency,
    const float base_quality);