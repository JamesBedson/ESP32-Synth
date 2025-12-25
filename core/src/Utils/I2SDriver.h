#pragma once
#include "driver/i2s.h"
#include "Constants.h"

#define I2S_PORT I2S_NUM_0


class I2SDriver {

    public:
        I2SDriver() 
        {
            audioConfig = 
            {
                .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX), // ESP Function --> audio master + transmitter
                .sample_rate = Constants::SAMPLE_RATE,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // priority level
                .dma_buf_count = 8,
                .dma_buf_len = 64, // 8 x 64 = 512 samples of audio
                .use_apll = true
            };

            pin_config = 
            {
                .bck_io_num = 26,
                .ws_io_num = 25,
                .data_out_num = 22,
                .data_in_num = -1
            };
        }

        void begin() 
        {
            i2s_driver_install(I2S_PORT, &audioConfig, 0, NULL);
            i2s_set_pin(I2S_PORT, &pin_config);
            i2s_zero_dma_buffer(I2S_PORT); //initialises buffer with zeros.
        }

    bool write(const void* sampleBuffer, size_t numBytes)
    {
        size_t bytesWritten = 0;

        esp_err_t result = i2s_write(
            I2S_PORT,
            sampleBuffer,
            numBytes,
            &bytesWritten,
            portMAX_DELAY // block until buffer has space
        );

        return (result == ESP_OK && bytesWritten == numBytes);
    }

    private:
        i2s_config_t audioConfig;
        i2s_pin_config_t pin_config;
};