from flask import Flask, request, jsonify
import json
import requests

app = Flask(__name__)

# OpenAI API configuration
OPENAI_API_KEY = "YOUR_API_KEY"  # Replace with your actual API key
OPENAI_API_URL = "https://api.openai.com/v1/chat/completions"

# Bot state structure
class BotState:
    def __init__(self):
        self.position = {"x": 0, "y": 0, "z": 0}
        self.health = 100
        self.ammo = {}
        self.enemies = []
        self.items = []
        self.team = 0
        self.game_mode = "deathmatch"
        self.state = "idle"

@app.route('/ai/decision', methods=['POST'])
def get_ai_decision():
    try:
        data = request.json
        bot_state = BotState()
        
        # Update bot state from request
        bot_state.position = data.get('position', {'x': 0, 'y': 0, 'z': 0})
        bot_state.health = data.get('health', 100)
        bot_state.ammo = data.get('ammo', {})
        bot_state.enemies = data.get('enemies', [])
        bot_state.items = data.get('items', [])
        bot_state.team = data.get('team', 0)
        bot_state.game_mode = data.get('game_mode', 'deathmatch')
        bot_state.state = data.get('state', 'idle')

        # Prepare prompt for AI
        prompt = f"""
        You are a Half Life 1 bot AI. Analyze the current game state and provide a tactical decision.
        
        Current State:
        Position: ({bot_state.position['x']}, {bot_state.position['y']}, {bot_state.position['z']})
        Health: {bot_state.health}%
        Ammo: {json.dumps(bot_state.ammo)}
        Team: {bot_state.team}
        Game Mode: {bot_state.game_mode}
        Current State: {bot_state.state}
        
        Nearby Items: {json.dumps(bot_state.items)}
        Enemies: {json.dumps(bot_state.enemies)}
        
        Provide a tactical decision including:
        1. Action to take (move, attack, defend, etc.)
        2. Target of the action
        3. Reasoning behind the decision
        4. Priority level (0.0 - 1.0)
        """

        # Call OpenAI API
        headers = {
            'Authorization': f'Bearer {OPENAI_API_KEY}',
            'Content-Type': 'application/json'
        }
        
        data = {
            'model': 'gpt-3.5-turbo',
            'messages': [
                {'role': 'system', 'content': 'You are a tactical AI for Half Life 1 bots.'},
                {'role': 'user', 'content': prompt}
            ],
            'temperature': 0.7,
            'max_tokens': 200
        }

        response = requests.post(OPENAI_API_URL, headers=headers, json=data)
        response.raise_for_status()
        
        # Parse AI response
        ai_response = response.json()
        decision = ai_response['choices'][0]['message']['content']

        # Return decision
        return jsonify({
            'action': decision.get('action', 'unknown'),
            'target': decision.get('target', {}),
            'reason': decision.get('reason', 'No reason provided'),
            'priority': float(decision.get('priority', 0.5))
        })

    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5001)
