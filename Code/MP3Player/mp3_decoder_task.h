#ifndef MP3_DECODER_TASK_H
#define MP3_DECODER_TASK_H

#include <cstdint>
#include "config.h"
#include "pcm_audio_interface.h"
#include "sd_reader_task.h"
#include "gpio_rp2040.h"
#include "task.h"
#include "mad.h"

class mp3_decoder_task : public task
{
public:
    mp3_decoder_task(pcm_audio_interface & pcm_if, sd_reader_task & sd);

    void run() override;
    void reset();
    uint16_t  get_position(unsigned long fsize,int max_pos);
    uint32_t get_total_seconds();
private:
    static enum mad_flow input (void *data, struct mad_stream *stream);
    static enum mad_flow header(void *data, struct mad_header const *);
    static enum mad_flow output(void *data, mad_header const *header, mad_pcm *pcm);
    static enum mad_flow error (void *data, mad_stream *stream, mad_frame *frame);
    static int16_t       scale (mad_fixed_t sample);

    pcm_audio_interface &   _pcm_if;
    sd_reader_task  &       _sd_reader;
    mad_decoder             _decoder{};
    gpio_rp2040             _led;
    uint8_t                 _mp3_buf[MP3_BUF_SIZE]{};
    uint32_t                _pcm_rate;
    mad_timer_t             _timer;
    unsigned long           _bitrate;
    unsigned long           _total_time;
};

#endif // MP3_DECODER_TASK_H
