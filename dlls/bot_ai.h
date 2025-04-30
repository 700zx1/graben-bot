#ifndef BOT_AI_H
#define BOT_AI_H

#include "bot.h"
#include <curl/curl.h>
#include <json/json.h>
#include <vector>

// Initialize AI system
void AI_Init(void);

// Cleanup AI resources
void AI_Cleanup(void);

#endif // BOT_AI_H
