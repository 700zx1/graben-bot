# GraveBot - Half Life 1 Bot with TTS

This project implements a Half Life 1 bot with text-to-speech capabilities using a Python backend.

## Dependencies

### System Dependencies

1. OpenAL
2. libcurl
3. jsoncpp

### Installation on Ubuntu/Debian

```bash
# Install system dependencies
sudo apt-get install libopenal-dev libcurl4-openssl-dev libjsoncpp-dev

# Install Python dependencies
pip install -r requirements.txt
```

### Building the Plugin

1. Install CMake if not already installed:
```bash
sudo apt-get install cmake
```

2. Create and enter build directory:
```bash
mkdir build
cd build
```

3. Run CMake and build:
```bash
cmake ..
make
```

The compiled plugin will be in the `build/bin` directory.

## Running the System

1. Start the Python TTS server:
```bash
python tts_server.py
```

2. Start Half Life with the bot plugin.

The bot will now automatically convert chat messages to speech using the TTS server.
