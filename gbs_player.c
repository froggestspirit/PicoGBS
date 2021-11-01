#include <stdio.h> 
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
 
// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN_L 28  // you can change this to whatever you like
#define AUDIO_PIN_R 27  // you can change this to whatever you like

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 0x1000  // (Needs to be a multiple of 2)
#define BUFFER_SIZE_HALF (BUFFER_SIZE >> 1)
#define DEFAULT_LENGTH 90  // Default song length in seconds
#define MUTE_THRESHOLD (SAMPLE_RATE * 4)  // How long a song should stay silent before ending

float soundChannelPos[4];
int16_t soundChannel4Bit;
const int16_t *PU1Table;
const int16_t *PU2Table;
const uint8_t *PU4Table;
uint16_t PU4TableLen;
uint32_t gbFrame, apuFrame;
uint8_t apuCycle;
int8_t output[BUFFER_SIZE];
uint16_t readPos, fillPos;
uint8_t song, maxSongs;

float fadeout;
uint16_t songTime, secFrame;
uint32_t mutedTime;

#include "tables.h"
#include "lfsr.h"
#include "peanut_gb.h"

#include "gbs.h"

static struct gb_s gb;


void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN_L));
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN_R));
	if(fadeout == 1.0f){
		pwm_set_gpio_level(AUDIO_PIN_L, output[readPos++] + 0x80);
		pwm_set_gpio_level(AUDIO_PIN_R, output[readPos++] + 0x80);
	}else{
		pwm_set_gpio_level(AUDIO_PIN_L, (output[readPos++] * fadeout) + 0x80);
		pwm_set_gpio_level(AUDIO_PIN_R, (output[readPos++] * fadeout) + 0x80);
	}
	if(readPos >= BUFFER_SIZE) readPos -= BUFFER_SIZE;
}


void play_song(uint8_t song){
	gb_init(&gb, song);
	fadeout = 1.0f;
	songTime = 0;
	secFrame = 0;
	mutedTime = 0;
	readPos = 0;
	fillPos = 0;
	for(uint32_t i = 0; i < BUFFER_SIZE; i++) output[i] = 0;
	soundChannelPos [0] = 0;
	soundChannelPos [1] = 0.01;
	soundChannelPos [2] = 0;
	soundChannelPos [3] = 0;

	gbFrame = SAMPLE_RATE;
	apuFrame = SAMPLE_RATE;
	apuCycle = 0;
}


int main(void) {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
    stdio_init_all();
    set_sys_clock_khz(132000, true); 
    gpio_set_function(AUDIO_PIN_L, GPIO_FUNC_PWM);
    gpio_set_function(AUDIO_PIN_R, GPIO_FUNC_PWM);

	for(int i = 0; i < 0x70; i++) gb.rom[i] = gbs[i];
	maxSongs = gb.rom[0x04];
	song = gb.rom[0x05] - 1;
	gb.load_address = gb.rom[0x06] + (gb.rom[0x07] << 8);
	gb.init_address = gb.rom[0x08] + (gb.rom[0x09] << 8);
	gb.play_address = gb.rom[0x0A] + (gb.rom[0x0B] << 8);
	gb.stack_pointer = gb.rom[0x0C] + (gb.rom[0x0D] << 8);
	gb.timer_modulo = gb.rom[0x0E];
	gb.timer_control = gb.rom[0x0F];
	for(int i = 0; i < gb.load_address; i++) gb.rom[i] = 0x00;
	for(int i = 0; i < 0x19E81; i++) gb.rom[i + gb.load_address] = gbs[i + 0x70];


    int audio_pin_slice_l = pwm_gpio_to_slice_num(AUDIO_PIN_L);
    int audio_pin_slice_r = pwm_gpio_to_slice_num(AUDIO_PIN_R);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice_l);
    pwm_clear_irq(audio_pin_slice_r);
    pwm_set_irq_enabled(audio_pin_slice_l, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176, 000, 000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv( & config, 12.0f); 
    pwm_config_set_wrap( & config, 250); 
    pwm_init(audio_pin_slice_l, & config, true);
    pwm_init(audio_pin_slice_r, & config, true);

    pwm_set_gpio_level(AUDIO_PIN_L, 0);
    pwm_set_gpio_level(AUDIO_PIN_R, 0);

	play_song(song);

    while(1) {
		if((fillPos >= readPos && fillPos - readPos < BUFFER_SIZE_HALF) || (fillPos < readPos && (fillPos + BUFFER_SIZE) - readPos < BUFFER_SIZE_HALF)){
			secFrame++;
			if(secFrame >= SAMPLE_RATE){
				secFrame -= SAMPLE_RATE;
				if(++songTime == DEFAULT_LENGTH){
					fadeout = 0.999f;
				}
			}

			gbFrame += 60;
			if(gbFrame >= SAMPLE_RATE){
				gbFrame -= SAMPLE_RATE;
				if(fadeout < 1.0f){
					fadeout -= 0.001f;
					if(fadeout <= 0){
						if(++song >= maxSongs) song -= maxSongs;
						play_song(song);
					}
				}
				gb.gb_frame = 0;
				while(!gb.gb_frame) __gb_step_cpu(&gb);
				
				switch(gb.hram[0x11] & 0xC0){
					case 0x00:
						PU1Table = PU0;
					break;
					case 0x40:
						PU1Table = PU1;
					break;
					case 0x80:
						PU1Table = PU2;
					break;
					case 0xC0:
						PU1Table = PU3;
					break;
				}

				switch(gb.hram[0x16] & 0xC0){
					case 0x00:
						PU2Table = PU0;
					break;
					case 0x40:
						PU2Table = PU1;
					break;
					case 0x80:
						PU2Table = PU2;
					break;
					case 0xC0:
						PU2Table = PU3;
					break;
				}

				switch(gb.hram[0x22] & 0x08){
					case 0x00:
						PU4Table = lfsr15;
						PU4TableLen = 0x7FFF;
					break;
					case 0x08:
						PU4Table = lfsr7;
						PU4TableLen = 0x7F;
					break;
				}
			}

			apuFrame += 512;
			if(apuFrame >= SAMPLE_RATE){
				apuFrame -= SAMPLE_RATE;
				apuCycle++;

				if((apuCycle & 1) == 0){  // Length
					if(gb.audio.ch1Len){
						if(--gb.audio.ch1Len == 0 && gb.audio.ch1LenOn){
							gb.hram[0x26] &= 0xFE;
						}
					}
					
					if(gb.audio.ch2Len){
						if(--gb.audio.ch2Len == 0 && gb.audio.ch2LenOn){
							gb.hram[0x26] &= 0xFD;
						}
					}
					
					if(gb.audio.ch3Len){
						if(--gb.audio.ch3Len == 0 && gb.audio.ch3LenOn){
							gb.hram[0x26] &= 0xFB;
						}
					}
					
					if(gb.audio.ch4Len){
						if(--gb.audio.ch4Len == 0 && gb.audio.ch4LenOn){
							gb.hram[0x26] &= 0xF7;
						}
					}
				}

				if((apuCycle & 7) == 7){  // Envelope
					if(gb.audio.ch1EnvCounter){
						if(--gb.audio.ch1EnvCounter == 0){
							if(gb.audio.ch1Vol && !gb.audio.ch1EnvDir){
								gb.audio.ch1Vol--;
								gb.audio.ch1EnvCounter = gb.audio.ch1EnvCounterI;
							}else if(gb.audio.ch1Vol < 0x0F && gb.audio.ch1EnvDir){
								gb.audio.ch1Vol++;
								gb.audio.ch1EnvCounter = gb.audio.ch1EnvCounterI;
							}
						}
					}
					
					if(gb.audio.ch2EnvCounter){
						if(--gb.audio.ch2EnvCounter == 0){
							if(gb.audio.ch2Vol && !gb.audio.ch2EnvDir){
								gb.audio.ch2Vol--;
								gb.audio.ch2EnvCounter = gb.audio.ch2EnvCounterI;
							}else if(gb.audio.ch2Vol < 0x0F && gb.audio.ch2EnvDir){
								gb.audio.ch2Vol++;
								gb.audio.ch2EnvCounter = gb.audio.ch2EnvCounterI;
							}
						}
					}
					
					if(gb.audio.ch4EnvCounter){
						if(--gb.audio.ch4EnvCounter == 0){
							if(gb.audio.ch4Vol && !gb.audio.ch4EnvDir){
								gb.audio.ch4Vol--;
								gb.audio.ch4EnvCounter = gb.audio.ch4EnvCounterI;
							}else if(gb.audio.ch4Vol < 0x0F && gb.audio.ch4EnvDir){
								gb.audio.ch4Vol++;
								gb.audio.ch4EnvCounter = gb.audio.ch4EnvCounterI;
							}
						}
					}
				}

				if((apuCycle & 3) == 2){  // Sweep
					if(gb.audio.ch1SweepCounterI && gb.audio.ch1SweepShift){
						if(--gb.audio.ch1SweepCounter == 0){
							gb.audio.ch1Freq = gb.hram[0x13] + ((gb.hram[0x14] & 7) << 8);
							if(gb.audio.ch1SweepDir){
								gb.audio.ch1Freq -= gb.audio.ch1Freq >> gb.audio.ch1SweepShift;
								if(gb.audio.ch1Freq & 0xF800) gb.audio.ch1Freq = 0;
							}else{
								gb.audio.ch1Freq += gb.audio.ch1Freq >> gb.audio.ch1SweepShift;
								if(gb.audio.ch1Freq & 0xF800){
									gb.audio.ch1Freq = 0;
									gb.audio.ch1EnvCounter = 0;
									gb.audio.ch1Vol = 0;
								}
							}
							gb.hram[0x13] = gb.audio.ch1Freq & 0xFF;
							gb.hram[0x14] &= 0xF8;
							gb.hram[0x14] += (gb.audio.ch1Freq >> 8) & 0x07;
							gb.audio.ch1SweepCounter = gb.audio.ch1SweepCounterI;
						}
					}
				}
			}
			//Sound generation loop
			soundChannelPos[0] += freqTable[gb.hram[0x13]+((gb.hram[0x14]&7)<<8)] / (SAMPLE_RATE / 32);
			soundChannelPos[1] += freqTable[gb.hram[0x18]+((gb.hram[0x19]&7)<<8)] / (SAMPLE_RATE / 32);
			soundChannelPos[2] += freqTable[gb.hram[0x1D]+((gb.hram[0x1E]&7)<<8)] / (SAMPLE_RATE / 32);
			soundChannelPos[3] += freqTableNSE[gb.hram[0x22]] / (SAMPLE_RATE);
			while(soundChannelPos[0] >= 32) soundChannelPos[0] -= 32;
			while(soundChannelPos[1] >= 32) soundChannelPos[1] -= 32;
			while(soundChannelPos[2] >= 32) soundChannelPos[2] -= 32;
			while(soundChannelPos[3] >= PU4TableLen) soundChannelPos[3] = 0;
			output[fillPos++] = 0;
			output[fillPos--] = 0;
			if(gb.hram[0x26] & 0x80){
				soundChannel4Bit = 7 - (int)(soundChannelPos[3]) & 7;
				if((gb.hram[0x25] & 0x01) && (gb.audio.ch1DAC) && (gb.hram[0x26] & 0x01)) output[fillPos] += gb.audio.ch1Vol * PU1Table[(int)(soundChannelPos[0])];
				if((gb.hram[0x25] & 0x02) && (gb.audio.ch2DAC) && (gb.hram[0x26] & 0x02)) output[fillPos] += gb.audio.ch2Vol * PU2Table[(int)(soundChannelPos[1])];
				if((gb.hram[0x25] & 0x04) && (gb.hram[0x1A] & 0x80) && (gb.hram[0x26] & 0x04)) output[fillPos] += gb.audio.WAVRAM[(int)(soundChannelPos[2])] >> gb.audio.ch3Vol;
				if((gb.hram[0x25] & 0x08) && (gb.audio.ch4DAC) && (gb.hram[0x26] & 0x08)) output[fillPos] += gb.audio.ch4Vol * (((PU4Table[(int)(soundChannelPos[3]/8)] >> soundChannel4Bit) & 1) ? 1 : -1);
				fillPos++;
				if((gb.hram[0x25] & 0x10) && (gb.audio.ch1DAC) && (gb.hram[0x26] & 0x01)) output[fillPos] += gb.audio.ch1Vol * PU1Table[(int)(soundChannelPos[0])];
				if((gb.hram[0x25] & 0x20) && (gb.audio.ch2DAC) && (gb.hram[0x26] & 0x02)) output[fillPos] += gb.audio.ch2Vol * PU2Table[(int)(soundChannelPos[1])];
				if((gb.hram[0x25] & 0x40) && (gb.hram[0x1A] & 0x80) && (gb.hram[0x26] & 0x04)) output[fillPos] += gb.audio.WAVRAM[(int)(soundChannelPos[2])] >> gb.audio.ch3Vol;
				if((gb.hram[0x25] & 0x80) && (gb.audio.ch4DAC) && (gb.hram[0x26] & 0x08)) output[fillPos] += gb.audio.ch4Vol * (((PU4Table[(int)(soundChannelPos[3]/8)] >> soundChannel4Bit) & 1) ? 1 : -1);
			}else{
				fillPos++;
			}
			if((output[fillPos] | output[fillPos - 1]) == 0){
				if(++mutedTime >= MUTE_THRESHOLD) fadeout = 0;  // Setting fadeout to 0 will trigger the next song on the next gbframe
			}else{
				mutedTime = 0;
			}
			if(++gb.audio.idleTimer >= MUTE_THRESHOLD) fadeout = 0;  // Setting fadeout to 0 will trigger the next song on the next gbframe
			if(++fillPos >= BUFFER_SIZE) fillPos -= BUFFER_SIZE;
		}else{
        __wfi(); // Wait for Interrupt
		}
    }
}
