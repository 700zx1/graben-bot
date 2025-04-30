#include <curl/curl.h>

extern "C" {
    void __declspec(dllexport) BotCleanupAI(void)
    {
        if (ai_curl) {
            curl_easy_cleanup(ai_curl);
            curl_global_cleanup();
        }
    }
}
