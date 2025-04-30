#include <curl/curl.h>

extern "C" {
    void __declspec(dllexport) BotCleanupTTS(void)
    {
        if (curl) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
        }
    }
}
