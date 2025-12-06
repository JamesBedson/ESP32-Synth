#include <Arduino.h>
#include "Utils/I2SDriver.h"
#include "Sequencer/Pattern.h"
#include "Sequencer/Sequencer.h"
#include "Sequencer/PlaybackEngine.h"

#define STACK_DEPTH 4096

TaskHandle_t audioTaskHandle = NULL;

I2SDriver i2sDriver;
Pattern<Constants::MAX_STEP_SIZE> pattern(4);
Sequencer sequencer(180.0f);
Synth synth;

PlaybackEngine engine(
	sequencer, 
	pattern, 
	synth
);

void audioCallback(void* param)
{

	const float bufferMs = (Constants::BUFFER_SIZE * 1000.0f) / Constants::SAMPLE_RATE;
	int16_t buffer[Constants::BUFFER_SIZE * Constants::CHANNELS];

	sequencer.start();

	while (true)
	{
		engine.update(bufferMs);

		synth.renderAudio(buffer, Constants::BUFFER_SIZE);

		i2sDriver.write(buffer, sizeof(buffer));
	};
}

void setup() 
{
	Serial.begin(115200);

	i2sDriver.begin();

	for (int i = 0; i < 16; i++) 
    {
        Step& s = pattern.getStep(i);
        s.setGate(true);
        s.setNote(60 + (i % 4) * 2);
    }
	// Audio callback
	xTaskCreatePinnedToCore(
		audioCallback,
		"Audio Callback",
		STACK_DEPTH,
		NULL,
		3,
		&audioTaskHandle,
		1
	);
}

void loop() 
{

}
