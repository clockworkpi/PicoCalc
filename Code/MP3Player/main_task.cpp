#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "main_task.h"
#include "logo50.h"
#include "icon_enter.h"
#include "icon_esc.h"
#include "icon_nav.h"
#include "icon_p.h"

main_task::main_task()
        : task("Main", MAIN_STACKSIZE),
          _cs(SD_CS_PIN),
          _spi(SD_SPI0, SD_MISO_PIN, SD_MOSI_PIN, SD_SCLK_PIN, _cs),
          _sd(_spi),
          _next(NEXT_BUTTON),
          _part(PART_BUTTON),
          _ps(PICO_PS),
          _fs(_sd),
          _lcd_rst(LCD_RST_PIN),
          _lcd_dc(LCD_DC_PIN),
          _lcd_cs(LCD_CS_PIN),
          _lcd_spi(LCD_SPI1, LCD_MISO_PIN, LCD_MOSI_PIN, LCD_SCK_PIN, _lcd_cs),
          _lcd(_lcd_spi,_lcd_rst,_lcd_dc,ili9488_drv::PICO320),
          _gui(_lcd),
          _sel_index(0),
          _page_index(0),
          _last_page_index(0),
          _last_sel_index(0),
          update_sel(0),
          update_required(1),
          playing(0),
          last_play_pos(0),
          play_pos(0),
          num_files(0),
          pause_flag(0),
          play_flag(0){

    _next.gpioMode(GPIO::INPUT | GPIO::PULLUP);
    _part.gpioMode(GPIO::INPUT | GPIO::PULLUP);
    _ps.gpioMode(GPIO::OUTPUT | GPIO::INIT_HIGH);
    _gui.FontSelect(&FONT_8X14);
    _gui.SetForecolor(C_GAINSBORO);

    for(auto & menu_item : menu_items){
        menu_item.fsize = 0;
        menu_item.fname = nullptr;
        menu_item.x = 0;
        menu_item.y = 0;
    }
}

void main_task::draw_string(int x,int y,const char*str,uint8_t fnt = 1){

    if(fnt == 0) {
        _gui.FontSelect(&FONT_6X8);
    }
    if(fnt == 1) {
        _gui.FontSelect(&FONT_8X14);
    }
    if(fnt == 2) {
        _gui.FontSelect(&FONT_12X20);
    }

    _gui.SetForecolor(C_GAINSBORO);
    _gui.SetBackcolor(C_BLACK);
    _gui.PutString(x,y,str);
}
void main_task::draw_highlight_string(int x,int y,const char*str){
    _gui.FontSelect(&FONT_8X14);
    _gui.SetForecolor(C_BLACK);
    _gui.SetBackcolor(C_GAINSBORO);
    _gui.PutString(x,y,str);
}

void main_task::draw_char(int x,int y ,char c) {
    _gui.FontSelect(&FONT_8X14);
    _gui.PutChar(c,x,y,C_GAINSBORO,C_BLACK, true);
}

void main_task::draw_big_char(int x,int y ,char c) {
    _gui.FontSelect(&FONT_12X20);
    _gui.PutChar(c,x,y,C_GAINSBORO,C_BLACK, true);
}
void main_task::draw_bar(int x1, int x2,UG_COLOR c) {
    uint8_t  offsetx = 10;
   _gui.FillFrame(x1+offsetx,280,x2+offsetx,285,c);
}


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 280
#define FONT_WIDTH 12
#define FONT_HEIGHT 20
#define MAX_CHARS_PER_LINE (SCREEN_WIDTH / FONT_WIDTH)
#define MAX_LINES 2

void main_task::draw_wrap_text(const char *text) {
    int line_count = 0;
    const char *ptr = text;


    char lines[MAX_LINES][MAX_CHARS_PER_LINE + 1] = {{0}};
    while (*ptr && line_count < MAX_LINES) {
        int i = 0;
        while (*ptr && i < MAX_CHARS_PER_LINE) {
            lines[line_count][i++] = *ptr++;
        }
        lines[line_count][i] = '\0';
        line_count++;
    }


    int text_height = line_count * FONT_HEIGHT;
    int y_offset = (SCREEN_HEIGHT - text_height) / 2;


    for (int line = 0; line < line_count; line++) {
        int char_count = strlen(lines[line]);
        int x_offset = (SCREEN_WIDTH - char_count * FONT_WIDTH) / 2;
        for (int j = 0; j < char_count; j++) {
            draw_big_char(x_offset + j * FONT_WIDTH, y_offset + line * FONT_HEIGHT, lines[line][j]);
        }
    }
}

int main_task::enum_files() {
    char tmp[16];
    _gui.FontSelect(&FONT_8X14);
    _gui.ConsoleSetForecolor(C_GAINSBORO);
    _gui.ConsoleSetBackcolor(C_BLACK);
    _gui.ConsolePutString("Init FatFS");
    FatFs::FRESULT res = _fs.mount(0);
    if(res == FatFs::FR_OK){
        _gui.ConsolePutString("[OK]");
    }else{
        sprintf(tmp,"[NG] code=0x%x",res);
        _gui.ConsolePutString(tmp);
        return -1;
    }
    _gui.ConsolePutString("\n");
    _gui.ConsolePutString("File List");

    num_files = 0;
    res = _fs.findfirst(&_dir, &_finfo, "", "*.mp3");
    // Loop over all MP3 files
    while (res == FatFs::FR_OK && _finfo.fname[0] && strlen(_finfo.fname) > 4 ) {
        printf("%s\n",_finfo.fname);
        menu_items[num_files].fname = (char*)malloc(strlen(_finfo.fname) + 1);
        strcpy(menu_items[num_files].fname, _finfo.fname);
        menu_items[num_files].fsize = _finfo.fsize;

        res = _fs.findnext(&_dir, &_finfo);
        num_files++;
        if(num_files>=MAX_FILES){
            break;
        }
    }
    if (res != FatFs::FR_OK) {
        sprintf(tmp, "[NG] code=0x%x", (int)res);
        _gui.ConsolePutString( tmp);
        return -1;
    }
    _fs.closedir(&_dir);
    _gui.ConsolePutString("[OK]");
    return num_files;
}

//In ffconf.h,
// set
// #define FF_MAX_LFN		33
// #define FF_LFN_BUF		33
/*
void truncate_string(const char *input, char *output, size_t max_length) {
    size_t input_len = strlen(input);

    if (input_len <= max_length) {
        strcpy(output, input);
    } else {
        strncpy(output, input, max_length - 3);
        output[max_length - 3] = '\0';
        strcat(output, "...");
    }
}
*/
#define HEADER_HEIGHT 20
int main_task::select_mp3() {
    char tmp[34];
    uint16_t start_x = 11;
    uint16_t start_y= HEADER_HEIGHT+3;
    /*               header
     * ------------------------------------------
     * 3px
     * [item]
     * 3px
     * [item]
     * 3px
     * [item]
     * 3px
     */

    for (int i = _page_index; i < _page_index+ITEMS_PER_PAGE;i++) {
        if(i >= num_files) break;
        //truncate_string(file_list[_page_index+i],tmp,33);
        menu_items[i].x = start_x;
        menu_items[i].y = start_y;

        draw_string(menu_items[i].x, menu_items[i].y, menu_items[i].fname);
        start_y+=23;
    }

    return 0;
}
void main_task::draw_cursor() {
    char buf[12];
    //sprintf(buf, "%02d", _sel_index + 1);
    //draw_string(0,_lcd.getSizeY()-20,buf);
    printf("%d %d %d\n",_last_sel_index,_sel_index,_page_index);
    uint16_t x,y;

    if(update_sel){
        x = menu_items[_last_sel_index % ITEMS_PER_PAGE].x;
        y = menu_items[_last_sel_index % ITEMS_PER_PAGE].y;

        _gui.FillFrame(10, y - 2, _lcd.getSizeX() - 10, y + 15, C_BLACK);
        draw_string(x, y, menu_items[_last_sel_index].fname);
    }
    x = menu_items[_sel_index % ITEMS_PER_PAGE].x;
    y = menu_items[_sel_index % ITEMS_PER_PAGE].y;

    _gui.FillFrame(10,y-2,_lcd.getSizeX()-10,y+15,C_GAINSBORO);
    //draw_string(0, (_sel_index % ITEMS_PER_PAGE) * 20+20, "=>");
    draw_highlight_string(11, y, menu_items[_sel_index].fname);

}

void main_task::clear_screen() {
    _lcd.clearScreen(C_BLACK);
}
void main_task::clear_menu() {
    //clear menu section area, not whole screen
    _lcd.fillArea(0,20,_lcd.getSizeX()-1,_lcd.getSizeY()-20,C_BLACK);
}
void main_task::boot_menu() {
    _page_index = (_sel_index / ITEMS_PER_PAGE) * ITEMS_PER_PAGE;
    if(_page_index!= _last_page_index){
        clear_menu();
        _last_page_index = _page_index;
        update_required = 1;
        update_sel = 0;
    }

    if(update_required) {
        clear_menu();
        draw_header("Playlist");
        select_mp3();
        draw_cursor();
        draw_footer(1);
        update_required = 0;
        last_play_pos = 0;
        play_pos = 0;

    }

    if(update_sel){
        draw_cursor();
        update_sel = 0;
    }
}

void main_task::menu_up() {
    _last_sel_index = _sel_index;
    if (_sel_index == 0) {
        _sel_index = num_files - 1;
    } else {
        _sel_index--;
    }
    update_sel = 1;
}
void main_task::menu_down() {
    _last_sel_index = _sel_index;
    if (_sel_index == num_files - 1) {
        _sel_index = 0;
    } else {
        _sel_index++;
    }
    update_sel = 1;
}

void main_task::menu_start(sd_reader_task&sd_reader,mp3_decoder_task &decoder,pcm_pwm_rp2040_drv &pcm_drv) {
    FatFs::FRESULT res = FatFs::FR_OK;
    printf("start play: %s\n",menu_items[_sel_index].fname);
    res = _fs.open(&_file, menu_items[_sel_index].fname, FA_OPEN_EXISTING | FA_READ);
    assert(res == FatFs::FR_OK);
    sd_reader.start(&_fs, &_file);
    decoder.reset();
    decoder.sign_up();
    update_required = 1;
    playing =1;
    last_play_pos = 0;
    play_pos = 0;
    pause_flag = 0;

}

//play through the list start from sel_index;
void main_task::menu_start_all(sd_reader_task &sd_reader, mp3_decoder_task &decoder, pcm_pwm_rp2040_drv &pcm_drv) {
    FatFs::FRESULT res = FatFs::FR_OK;
    printf("start play all: %d\n",_sel_index);
    res = _fs.open(&_file, menu_items[_sel_index].fname, FA_OPEN_EXISTING | FA_READ);
    assert(res == FatFs::FR_OK);
    sd_reader.start(&_fs, &_file);
    decoder.reset();
    decoder.sign_up();
    update_required = 1;
    playing =1;
    last_play_pos = 0;
    play_pos = 0;
    pause_flag = 0;
    play_flag = 1;
}

void main_task::draw_logo(){
    uint16_t start_x,start_y;

    start_x = (_lcd.getSizeX() - logo_width)/2;
    start_y = (_lcd.getSizeY() - logo_height)/2;

    unsigned char *data = (unsigned char *)header_data;
    for (unsigned int y = 0; y < logo_height; ++y) {
        for (unsigned int x = 0; x < logo_width; ++x) {
            unsigned char pixel[3];
            HEADER_PIXEL(data, pixel);

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];

            uint32_t color = (r << 16) | (g << 8) | b; // RGB888
            _lcd.drawPixel(start_x+x,start_y+y, color);
        }
    }
}
//header Height 20
void main_task::draw_header(char*title) {
    _gui.FontSelect(&FONT_8X14);
    _gui.SetForecolor(C_GAINSBORO);
    _gui.SetBackcolor(C_BLACK);
    _gui.PutString(11,(20-14)/2,"                    ");
    _gui.PutString(11,(20-14)/2,title);
    _lcd.drawHLine(10,19,_lcd.getSizeX()-10,C_LIGHT_GRAY);
}

void main_task::draw_footer_esc() {
    uint16_t start_x,start_y;

    start_x = 261;
    start_y = _lcd.getSizeY() - esc_height-5;

    unsigned char *data = (unsigned char *)esc_header_data;
    for (unsigned int y = 0; y < esc_height; ++y) {
        for (unsigned int x = 0; x < esc_width; ++x) {
            unsigned char pixel[3];
            ESC_PIXEL(data, pixel);

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];

            uint32_t color = (r << 16) | (g << 8) | b; // RGB888
            _lcd.drawPixel(start_x+x,start_y+y, color);
        }
    }
    draw_string(285,start_y+3,"Stop",0);
}

void main_task::draw_footer_enter() {
    uint16_t start_x,start_y;

    start_x = 260;
    start_y = _lcd.getSizeY() - enter_height-5;

    unsigned char *data = (unsigned char *)enter_header_data;
    for (unsigned int y = 0; y < enter_height; ++y) {
        for (unsigned int x = 0; x < enter_width; ++x) {
            unsigned char pixel[3];
            ENTER_PIXEL(data, pixel);

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];

            uint32_t color = (r << 16) | (g << 8) | b; // RGB888
            _lcd.drawPixel(start_x+x,start_y+y, color);
        }
    }
    draw_string(285,start_y+3,"Play",0);
}

void main_task::draw_footer_nav() {

    uint16_t start_x,start_y;

    start_x = 10;
    start_y = _lcd.getSizeY() - nav_height-6;

    unsigned char *data = (unsigned char *)nav_header_data;
    for (unsigned int y = 0; y < nav_height; ++y) {
        for (unsigned int x = 0; x < nav_width; ++x) {
            unsigned char pixel[3];
            NAV_PIXEL(data, pixel);

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];

            uint32_t color = (r << 16) | (g << 8) | b; // RGB888
            _lcd.drawPixel(start_x+x,start_y+y, color);
        }
    }
    draw_string(23,start_y+3,"Nav.",0);

}

void main_task::draw_footer_pause() {
    uint16_t start_x,start_y;

    start_x = 210;
    start_y = _lcd.getSizeY() - icon_p_height-5;

    unsigned char *data = (unsigned char *)icon_p_header_data;
    for (unsigned int y = 0; y < icon_p_height; ++y) {
        for (unsigned int x = 0; x < icon_p_width; ++x) {
            unsigned char pixel[3];
            ICON_P_PIXEL(data, pixel);

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];

            uint32_t color = (r << 16) | (g << 8) | b; // RGB888
            _lcd.drawPixel(start_x+x,start_y+y, color);
        }
    }
    draw_string(225,start_y+3,"Pause",0);
}

void main_task::draw_footer_pagenumber() {
    // page1/4

    char tmp[12];
    int total_page = (num_files / ITEMS_PER_PAGE)+1;
    int cur_page = (_sel_index / ITEMS_PER_PAGE)+1;
    if(total_page > 1) {
        sprintf(tmp, "%d/%d", cur_page, total_page);
        draw_string(150, _lcd.getSizeY() - nav_height-2, tmp, 0);
    }
}

void main_task::draw_footer(uint8_t stat) {
    //stat 1 == menu_list   ==> nav icon, Enter icon
    //stat 2 == playing music ==>  only Esc icon
    _gui.FillFrame(0,_lcd.getSizeY()-20,_lcd.getSizeX()-1,_lcd.getSizeY()-1,C_BLACK);
    if(stat == 1) {
        draw_footer_nav();
        draw_footer_enter();
        draw_footer_pagenumber();
    }
    if(stat == 2) {
        draw_footer_pause();
        draw_footer_esc();
    }

    _lcd.drawHLine(10,_lcd.getSizeY()-20,_lcd.getSizeX()-10,C_LIGHT_GRAY);
}

void main_task::draw_playing() {

    char * song_name = menu_items[_sel_index].fname;
    draw_wrap_text(song_name);

}


void main_task::run() {
    char tmp[34];
    i2c_kbd _kbd;

    pcm_pwm_rp2040_drv pcm_drv(AUDIO_LEFT, AUDIO_RIGHT);
    sd_reader_task sd_reader;
    mp3_decoder_task decoder(pcm_drv, sd_reader);

    uint8_t keycheck = 0;
    uint8_t keyread = 0;
    uint8_t key_stat = 0;//press,release, or hold
    int kbd_ret = 0;
    int c;
    static int ctrlheld = 0;
    ///const char spinner[] = {'/', '-', '|', '\\'};
    uint8_t spin_i = 0;
    uint8_t spin_timer = 0;

    int play_pos_diff = 0;

    _lcd.clearScreen(C_BLACK);
    draw_logo();
    task::sleep_ms(1000);
    clear_screen();

    num_files = enum_files();
    if (num_files <= 0) {
        printf("num_files <=0\n");
        return;
    }
    clear_screen();
    draw_header("Playlist");
    while (true){
        if(playing){
            if (sd_reader.isLinkedIn() &&  decoder.isLinkedIn()) {
                //printf("playing now\n");

                if(update_required) {
                    clear_menu();
                    //truncate_string(fname_list[_sel_index],tmp,33);
                    if(play_flag == 1) {
                        draw_header("Now Playing All");
                    }else{
                        draw_header("Now Playing");
                    }
                    draw_playing();
                    draw_footer(2);
                    update_required = 0;
                }
                if(spin_timer >= 100) {
                    //(bitrate*8/)filesize
                    //draw bar use last_play_pos
                    //draw_bar(0,last_play_pos,C_BLACK);
                    play_pos = decoder.get_position(menu_items[_sel_index].fsize,_lcd.getSizeY()-20);
                    ///printf("play_pos %d\n",play_pos);
                    //draw bar use play_pos
                    play_pos_diff = play_pos - last_play_pos;
                    if(play_pos_diff < 0) {// VBR
                        draw_bar(play_pos,last_play_pos,C_BLACK);
                    }else{ //>= 0
                        //draw_bar(last_play_pos,play_pos,C_BLACK);
                        draw_bar(play_pos,play_pos+play_pos_diff,C_GAINSBORO);
                    }
                    uint32_t seconds = decoder.get_total_seconds();

                    int minutes = seconds / 60;
                    int remaining_seconds = seconds % 60;

                    // "MM:SS"
                    snprintf(tmp, 6, "%02d:%02d", minutes, remaining_seconds);
                    draw_string(10,288,tmp,0);
                    ///draw_char(20, 300, spinner[spin_i % 4]);
                    spin_i++;
                    spin_timer = 0;
                    last_play_pos = play_pos;
                }
                spin_timer++;
            }else{
                printf("not playing...auto exit\n");
                _fs.close(&_file);
                playing =0;
                update_required =1;
                if(play_flag == 1) {
                    menu_down();
                    menu_start_all(sd_reader,decoder,pcm_drv);
                }
            }
        }else{
            boot_menu();
        }

        if(keycheck == 0) {
            if(keyread == 0) {
                kbd_ret = _kbd.write_i2c_kbd();
                keyread = 1;
            }else{
                kbd_ret = _kbd.read_i2c_kbd();
                keyread = 0;
            }
            keycheck = 16;
        }

        if(kbd_ret){
            if (kbd_ret == 0xA503)ctrlheld = 0;
            else if (kbd_ret == 0xA502) {
                ctrlheld = 1;
            } else if ((kbd_ret & 0xff) == 1) {//pressed
                key_stat = 1;
            } else if ((kbd_ret & 0xff) == 3) {
                key_stat = 3;
            }
            c = kbd_ret >> 8;
            int realc = -1;
            switch (c) {
                case 0xA1:
                case 0xA2:
                case 0xA3:
                case 0xA4:
                case 0xA5:
                    realc = -1;//skip shift alt ctrl keys
                    break;
                default:
                    realc = c;
                    break;
            }
            c = realc;
            if (c >= 'a' && c <= 'z' && ctrlheld)c = c - 'a' + 1;
            switch (c) {
                case 0xb5://UP
                    if(key_stat == 1 && !playing) {
                        menu_up();
                    }
                    break;
                case 0xb6://DOWN
                    if(key_stat == 1 && !playing) {
                        menu_down();
                    }
                    break;
                case 0xb4://LEFT
                    break;
                case 0xb7://RIGHT
                    break;
                case 0x0A://ENTER
                    if(key_stat == 1 && !playing) {
                        menu_start(sd_reader, decoder, pcm_drv);
                    }
                    break;
                case 'a':
                    if(key_stat == 1 && !playing) {
                        menu_start_all(sd_reader,decoder,pcm_drv);
                    }
                    break;
                case 0xB1://ESC
                    if(key_stat == 1 && playing) {
                        playing = 0;
                        if(decoder.getState()==task::SUSPENDED){//maybe is suspended
                            decoder.resume();
                        }
                        sd_reader.force_eof();
                        printf("stop playing\n");
                        pause_flag = 0;
                        update_required = 1;
                        play_flag = 0;
                    }
                    break;
                case 'p':
                    if(key_stat == 1 && playing) {
                        if(pause_flag == 0) {
                            pause_flag = 1;
                            decoder.suspend();
                            draw_header("Paused");
                        }else{
                            pause_flag = 0;
                            decoder.resume();
                            draw_header("Now Playing");
                        }
                        printf("pause playing mp3\n");
                    }
                    break;
                default:
                    break;
            }
            kbd_ret = 0;
        }

        task::sleep_ms(1);
        if(keycheck) keycheck--;
    }

}
