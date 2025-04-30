#include "bot.h"
#include <curl/curl.h>
#include <json/json.h>

// Global variables for AI
static CURL *ai_curl;
static CURLcode ai_res;
static const char* ai_server_url = "http://127.0.0.1:5001/ai/decision";

// Structure to hold AI decisions
struct AI_Decision {
    bool attack;
    bool defend;
    bool move;
    Vector target_position;
    float priority;
};

// Function to gather bot state
Json::Value GatherBotState(bot_t *pBot) {
    Json::Value state;
    edict_t *pEdict = pBot->pEdict;
    
    // Position
    state["position"]["x"] = pEdict->v.origin.x;
    state["position"]["y"] = pEdict->v.origin.y;
    state["position"]["z"] = pEdict->v.origin.z;
    
    // Health
    state["health"] = pEdict->v.health;
    
    // Ammo
    Json::Value ammo;
    for (int i = 0; i < MAX_WEAPONS; i++) {
        if (pBot->ammo[i] > 0) {
            ammo[weapon_defs[i].name] = pBot->ammo[i];
        }
    }
    state["ammo"] = ammo;
    
    // Team
    state["team"] = pBot->bot_team;
    
    // Game mode
    state["game_mode"] = (mod_id == CTF_DLL) ? "ctf" : 
                         (mod_id == SI_DLL) ? "sci" : 
                         "deathmatch";
    
    // Current state
    state["state"] = pBot->b_engaging_enemy ? "attacking" : 
                     (pBot->waypoint_goal != -1) ? "moving" : 
                     "idle";
    
    return state;
}

// Function to get AI decision
AI_Decision GetAIDecision(bot_t *pBot) {
    AI_Decision decision = {false, false, false, Vector(0,0,0), 0.0};
    
    if (!ai_curl) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        ai_curl = curl_easy_init();
    }
    
    if (ai_curl) {
        Json::Value state = GatherBotState(pBot);
        Json::FastWriter writer;
        std::string json = writer.write(state);
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(ai_curl, CURLOPT_URL, ai_server_url);
        curl_easy_setopt(ai_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(ai_curl, CURLOPT_POSTFIELDS, json.c_str());
        
        ai_res = curl_easy_perform(ai_curl);
        
        if (ai_res == CURLE_OK) {
            Json::Value response;
            Json::Reader reader;
            std::string response_str;
            
            // Get response from curl
            curl_easy_setopt(ai_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(ai_curl, CURLOPT_WRITEDATA, &response_str);
            
            if (reader.parse(response_str, response)) {
                decision.attack = (response["action"].asString() == "attack");
                decision.defend = (response["action"].asString() == "defend");
                decision.move = (response["action"].asString() == "move");
                
                if (response.isMember("target") && response["target"].isMember("position")) {
                    Json::Value target = response["target"]["position"];
                    decision.target_position.x = target["x"].asFloat();
                    decision.target_position.y = target["y"].asFloat();
                    decision.target_position.z = target["z"].asFloat();
                }
                
                decision.priority = response["priority"].asFloat();
            }
        }
        
        curl_slist_free_all(headers);
    }
    
    return decision;
}

// Function to apply AI decision
void ApplyAIDecision(bot_t *pBot, const AI_Decision &decision) {
    edict_t *pEdict = pBot->pEdict;
    
    // Reset all actions
    pEdict->v.button = 0;
    pBot->f_move_speed = 0.0f;
    pBot->f_strafe_speed = 0.0f;
    
    // Apply AI decision
    if (decision.attack) {
        // Attack behavior
        if (pBot->pBotEnemy) {
            BotShootAtEnemy(pBot);
        } else {
            // Find new enemy if none exists
            if (b_botdontshoot == 0) {
                pBot->pBotEnemy = BotFindEnemy(pBot);
            }
        }
    } else if (decision.defend) {
        // Defend behavior
        pBot->b_engaging_enemy = FALSE;
        pBot->pBotEnemy = nullptr;
    } else if (decision.move) {
        // Move behavior
        pBot->f_look_for_waypoint_time = gpGlobals->time;
        pBot->waypoint_goal = -1;
        
        // Set new goal position
        pBot->v_goal = decision.target_position;
        pBot->f_goal_proximity = 50.0f;  // Adjust as needed
    }
}
