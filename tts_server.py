from flask import Flask, request, Response
from gtts import gTTS
import io

app = Flask(__name__)

@app.route('/tts', methods=['POST'])
def text_to_speech():
    data = request.json
    text = data.get('text', '')
    
    if text:
        tts = gTTS(text=text, lang='en')
        audio = io.BytesIO()
        tts.write_to_fp(audio)
        audio.seek(0)
        
        return Response(
            audio,
            mimetype="audio/mp3",
            headers={"Content-Disposition": "attachment;filename=speech.mp3"}
        )
    
    return Response("No text provided", status=400)

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)
