///////////////////////////////////////////////////
// MP3 player for RPi Pico and GPIO audio output //
///////////////////////////////////////////////////
//
// This program will read MP3 files from a SD card and
// play them on the RP2040 launchpad. The MP3 decoder is
// implemented in software, so only some kind of PCM
// output is necessary, which is provided by a driver
// which outputs the PCM data as PWM GPIO signals.
//
// The main task will loop over all *.mp3 files and play
// them. The next-button can be used to skip to the next
// song. The paart-button is used to select the partition
// of the SD card during startup.
//
// For every new song, the SD reader and MP3 decoder
// tasks are started again.
//
// Make sure you connect the SD-card PCB to the correct
// SPI interface (see pin assignments in config.h), and
// provide some MP3 files without too high quality
// (128 kbps works fine!)

#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "config.h"
#include "gpio_rp2040.h"
#include "spi_rp2040.h"
#include "sd_spi_drv.h"
#include "pcm_pwm_rp2040_drv.h"
#include "task.h"
#include "sd_reader_task.h"
#include "mp3_decoder_task.h"
#include "ff.h"
#include <cassert>
#include "uGUI.h"
#include "uGUI_colors.h"
#include "font_6x8.h"
#include "font_8x14.h"
#include "font_12x20.h"
#include "ili9488_drv.h"
#include "i2ckbd.h"

typedef struct menu_item {
    uint16_t x;
    uint16_t y;
    char *fname; ///pointer to a malloced char buf
    int fsize;
}MENU_ITEM;


class main_task : public task
{
public:
    main_task();
    void run() override;
    void draw_string(int,int,const char*,uint8_t);
    void draw_highlight_string(int,int,const char*);
    void draw_char(int x,int y ,char c);
    void draw_big_char(int x,int y ,char c);
    void draw_bar(int x1,int x2,UG_COLOR c);
    int enum_files();
    int select_mp3();
    void draw_cursor();
    void boot_menu();
    void menu_up();
    void menu_down();
    void menu_start(sd_reader_task&sd_reader,mp3_decoder_task &decoder,pcm_pwm_rp2040_drv &pcm_drv);
    void menu_start_all(sd_reader_task&sd_reader,mp3_decoder_task &decoder,pcm_pwm_rp2040_drv &pcm_drv);
    void draw_logo();
    void clear_screen();
    void clear_menu();
    void draw_header(char*);
    void draw_footer(uint8_t);
    void draw_footer_enter();
    void draw_footer_esc();
    void draw_footer_nav();
    void draw_footer_pause();
    void draw_footer_pagenumber();
    void draw_playing();
    void draw_wrap_text(const char *);
private:
    gpio_rp2040     _cs;    // CS Line of SD card SPI interface
    spi_rp2040      _spi;   // SPI interface used for the SD card reader
    sd_spi_drv      _sd;    // SD card low level driver

    gpio_rp2040     _next;  // Next button
    gpio_rp2040     _part;  // Partition button
    gpio_rp2040     _ps;    // Pico Power save pin

    FatFs           _fs;
    FatFs::DIR      _dir{};
    FatFs::FILEINFO _finfo{};
    FatFs::FILE     _file{};  // MP3 file on SD

    gpio_rp2040     _lcd_rst;
    gpio_rp2040     _lcd_dc;
    gpio_rp2040     _lcd_cs;
    spi_rp2040 _lcd_spi;
    ili9488_drv _lcd;
    uGUI       _gui;
    int _sel_index;
    int _page_index;
    int _last_page_index;
    int _last_sel_index;
    uint8_t update_sel;
    uint8_t update_required;
    uint8_t playing;
    uint16_t last_play_pos;
    uint16_t play_pos;
    MENU_ITEM  menu_items[MAX_FILES]{};
    uint16_t num_files;
    uint8_t pause_flag ;
    uint8_t play_flag;//default:0, play_all = 1,play_shuffle = 2
};

#endif // MAIN_TASK_H
