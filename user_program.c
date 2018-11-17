/************************************
 * This is the framework for those
 * who wish to write their own C
 * code for the basic badge
 *
 * Take a look at user_program_temp.c (not included in project, but
 * available in src directory) to see how to use IIC routines
 ************************************/


#include "badge_user.h"
#include "hw.h"

#define MAX_NOTE_DURATION   800
#define MAX_RINGTONE_LENGTH 64
#define MAX_TONE_STRING_LENGTH 6
#define MAX_RINGTONE_STRING_LENGTH (MAX_RINGTONE_LENGTH * MAX_TONE_STRING_LENGTH)
#define TONE_BASE_OFFSET 36
#define TONE_OFFSET_ERROR 13

// Ringtone "language" reference from the 3310 docs
// https://www.nokia.com/en_int/phones/sites/default/files/user-guides/3310_usersguide_en.pdf

uint8_t get_tone_offset(char tone)
    {
    char *offsets = "c d ef g a b";
    uint8_t i;
    for (i = 0; i < 12; i++)
        {
        if (offsets[i] == tone)
            {
            return i;
            }
        }

    return TONE_OFFSET_ERROR;
    }

void parse_ringtone(char *ringtone_string, uint8_t* tones, uint* durations, int* tones_count)
    {
    uint8_t current_tone = TONE_BASE_OFFSET;
    uint8_t decoder_state = 0; // 0: note_length; 1: note_tone; 2: note_octave
    int i = 0; // string index

    uint8_t current_int;
    uint8_t octave_offset;
    uint8_t tone_offset;
    char current;
    char next;

    *tones_count = 0; // note array index

    while (ringtone_string[i] != '\0')
        {
        current = ringtone_string[i];
        next = ringtone_string[i+1];
        current_int = (uint8_t)current - (uint8_t)'0';

        if(current == ' ')
            {
            decoder_state = 0;
            current_tone = TONE_BASE_OFFSET;
            }
        else if (decoder_state == 0)
            {
                if (next >= '0' && next <= '9')
                    {
                    char duration_denom[2];
                    duration_denom[0] = current;
                    duration_denom[1] = next;
                    duration_denom[2] = '\0';
                    current_int = atoi(duration_denom);
                    i++;
                    }

                durations[*tones_count] = MAX_NOTE_DURATION / current_int;

                if (next == '.')
                    {
                    // add half the duration
                    durations[*tones_count] = durations[*tones_count] + (durations[*tones_count] / 2);
                    i++;
                    }
                decoder_state = 1;
            }
        else if (decoder_state == 1)
            {
            if (current == '#')
                {
                current_tone += 1;
                }
            else if (current >= 'a' && current <= 'z') // TODO: lowercase only for now
                {
                tone_offset = get_tone_offset(current);
                current_tone += tone_offset;
                decoder_state = 2;
                }
            }
        else if (decoder_state == 2)
            {
            octave_offset = 12 * (current_int - 1);
            current_tone += octave_offset;
            tones[*tones_count] = current_tone;
            (*tones_count)++;
            }
        i++;
        }
    }

void play_ringtone(uint8_t *tones, uint *durations, int length)
    {
    int i = 0;
    for(i = 0; i < length; i++)
        {
        sound_play_notes(tones[i], tones[i], tones[i], durations[i]);
        }
    }

void copy_ringtone(char *src, char *dest)
    {
    int i = 0;
    while(src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    }

void capture_ringtone(char *ringtone)
    {
    char* nokia = "8e4 8d4 4#f3 4#g3 8#c4 8#b3 4d3 4e3 8b3 8a3 4#c3 4e3 2.a3";
    uint8_t key_captured;
    int8_t ret;
    int i = 0;
    clr_buffer();
    video_set_color(3,8);	//Set color before writing
	video_gotoxy(0,0);
    while(1)
        {
        ret = stdio_get(&key_captured);
        if (ret != 0)
            {
            if (key_captured == 0xA)
                {
                ringtone[i] = '\0';
                return;
                }
            else if (key_captured == 'n')
                {
                copy_ringtone(nokia, ringtone);
                return;
                }
            else
                {
                ringtone[i] = key_captured;
                i++;
                stdio_c(key_captured);
                }
            }
        }
    }

//#include "hwz.h"
//#define BASE_FLASH_ADDR 0x0x040000 // 0x040000-0x07FFFF (flash empty space - see badge_settings.h)
//void save_ringtone (uint8_t *data, uint8_t slot)
//	{
//	uint32_t addr = BASE_FLASH_ADDR + slot * MAX_RINGTONE_STRING_LENGTH;
//    while (*data != '\0')
//        {
//        fl_write(addr++, *data++);
//        while ((fl_rdsr())&0x01);
//        }
//	}
//
//uint8_t load_ringtone (uint8_t * data, uint8_t slot)
//	{
//    uint32_t addr = BASE_FLASH_ADDR + slot * MAX_RINGTONE_STRING_LENGTH;
//    while (*data != '\0')
//        {
//        fl_write(addr++, *data++);
//        while ((fl_rdsr())&0x01);
//        }
//	}

void user_program_init(void)
	{
    return;
    }

void user_program_loop(void)
    {
    uint8_t tones[MAX_RINGTONE_LENGTH];
    uint durations[MAX_RINGTONE_LENGTH];
    int ringtone_length = 0;
    char ringtone[MAX_RINGTONE_LENGTH * MAX_TONE_STRING_LENGTH + 1];

    capture_ringtone(ringtone);
    parse_ringtone(ringtone, tones, durations, &ringtone_length);
    play_ringtone(tones, durations, ringtone_length);
    }