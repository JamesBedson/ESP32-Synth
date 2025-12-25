#include <Arduino.h>
#include "Utils/I2SDriver.h"
#include "Sequencer/Pattern.h"
#include "Sequencer/Sequencer.h"
#include "Sequencer/PlaybackEngine.h"
#include "Web/WebServerManager.h"
#include "Utils/AudioParameterTree.h"
#include "MessageBridge.h"
#include "Web/WebMessageHandler.h"
#include "Utils/AudioBuffer.h"
#include "Utils/AudioOutputConverter.h"

#define STACK_DEPTH 4096

TaskHandle_t audioTaskHandle = NULL;
I2SDriver i2sDriver;
Pattern<Constants::MAX_STEP_SIZE> pattern(4);
Sequencer sequencer(180.0f);
Synth synth;
PlaybackEngine engine(
	sequencer,
	pattern,
	synth);

WebMessageHandler handler;
WebServerManager webServer(handler);

AudioParameterTree tree;
MessageBridge bridge(tree);

AudioFloatBuffer floatBuf(Constants::BUFFER_SIZE, Constants::CHANNELS);
int16_t intBuf[Constants::BUFFER_SIZE * Constants::CHANNELS];

void audioCallback(void *param)
{
	const float bufferMs = (Constants::BUFFER_SIZE * 1000.0f) / Constants::SAMPLE_RATE;

	while (true)
	{
		floatBuf.clear();

		engine.update(bufferMs);
		synth.renderAudio(
			floatBuf.data(), 
			Constants::BUFFER_SIZE, 
			Constants::CHANNELS
		);

		AudioOutputConverter::floatToInt16(
			floatBuf.data(),
			intBuf,
			Constants::BUFFER_SIZE * Constants::CHANNELS);

		//Serial.println(intBuf[0]);
		i2sDriver.write(intBuf, sizeof(intBuf));
	};
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Booting...");

	bridge.connectToWebHandler(handler);
	tree.add(&synth.amplitude);

	sequencer.start();

	webServer.begin();
	i2sDriver.begin();

	for (int i = 0; i < 16; i++)
	{
		Step &s = pattern.getStep(i);
		s.setGate(true);
		s.setNote(60 + (i % 4) * 2);
	}

	// tree.add(&synth.frequency);
	//  Audio callback
	xTaskCreatePinnedToCore(
		audioCallback,
		"Audio Callback",
		STACK_DEPTH,
		NULL,
		3,
		&audioTaskHandle,
		1);
}

void loop()
{
	bridge.processPendingMessages();
	delay(1);
}
