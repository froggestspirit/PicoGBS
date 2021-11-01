/**
 * MIT License
 *
 * Copyright (c) 2018 Mahyar Koshkouei
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Please note that at least three parts of source code within this project was
 * taken from the SameBoy project at https://github.com/LIJI32/SameBoy/ which at
 * the time of this writing is released under the MIT License. Occurrences of
 * this code is marked as being taken from SameBoy with a comment.
 * SameBoy, and code marked as being taken from SameBoy,
 * is Copyright (c) 2015-2019 Lior Halphon.
 */

#pragma once

/* Interrupt masks */
#define VBLANK_INTR    0x01
#define LCDC_INTR    0x02
#define TIMER_INTR    0x04
#define SERIAL_INTR    0x08
#define CONTROL_INTR    0x10
#define ANY_INTR    0x1F

/* Memory section sizes for DMG */
#define SRAM_SIZE    0x8000
#define WRAM_SIZE    0x2000
#define HRAM_SIZE    0x0100

/* Memory addresses */
#define ROM_0_ADDR      0x0000
#define ROM_N_ADDR      0x4000
#define CART_RAM_ADDR   0xA000
#define WRAM_0_ADDR     0xC000
#define ECHO_ADDR       0xE000
#define OAM_ADDR        0xFE00
#define IO_ADDR         0xFF00
#define HRAM_ADDR       0xFF80
#define INTR_EN_ADDR    0xFFFF

/* Cart section sizes */
#define ROM_BANK_SIZE   0x4000

/* DIV Register is incremented at rate of 16384Hz.
 * 4194304 / 16384 = 256 clock cycles for one increment. */
#define DIV_CYCLES          256

/* Sound timers */
/* TODO: Possibility of combining these APU timers. Using the faster 256Hz
 * timer, then deriving the other two counters from that. */
#define APU_LEN_CYCLES        16384    /* Length counter 256Hz */
#define APU_SWP_CYCLES        32768    /* Sweep counter 128Hz */
#define APU_ENV_CYCLES        65536    /* Volume Envelope counter 64Hz */

/* Serial clock locked to 8192Hz on DMG.
 * 4194304 / (8192 / 8) = 4096 clock cycles for sending 1 byte. */
#define SERIAL_CYCLES        4096

/* Calculating VSYNC. */
#define DMG_CLOCK_FREQ        4194304.0
#define SCREEN_REFRESH_CYCLES    70224.0
#define VERTICAL_SYNC        (DMG_CLOCK_FREQ/SCREEN_REFRESH_CYCLES)

/* SERIAL SC register masks. */
#define SERIAL_SC_TX_START    0x80
#define SERIAL_SC_CLOCK_SRC    0x01

/* STAT register masks */
#define STAT_LYC_INTR       0x40
#define STAT_MODE_2_INTR    0x20
#define STAT_MODE_1_INTR    0x10
#define STAT_MODE_0_INTR    0x08
#define STAT_LYC_COINC      0x04
#define STAT_MODE           0x03
#define STAT_USER_BITS      0xF8

/* LCDC control masks */
#define LCDC_ENABLE         0x80
#define LCDC_WINDOW_MAP     0x40
#define LCDC_WINDOW_ENABLE  0x20
#define LCDC_TILE_SELECT    0x10
#define LCDC_BG_MAP         0x08
#define LCDC_OBJ_SIZE       0x04
#define LCDC_OBJ_ENABLE     0x02
#define LCDC_BG_ENABLE      0x01

/* LCD characteristics */
#define LCD_LINE_CYCLES     456
#define LCD_MODE_0_CYCLES   0
#define LCD_MODE_2_CYCLES   204
#define LCD_MODE_3_CYCLES   284
#define LCD_VERT_LINES      154
#define LCD_WIDTH           160
#define LCD_HEIGHT          144

/* Interrupt jump addresses */
#define VBLANK_INTR_ADDR    0x0040
#define LCDC_INTR_ADDR      0x0048
#define TIMER_INTR_ADDR     0x0050
#define SERIAL_INTR_ADDR    0x0058
#define CONTROL_INTR_ADDR   0x0060

/* SPRITE controls */
#define NUM_SPRITES         0x28
#define MAX_SPRITES_LINE    0x0A
#define OBJ_PRIORITY        0x80
#define OBJ_FLIP_Y          0x40
#define OBJ_FLIP_X          0x20
#define OBJ_PALETTE         0x10
#define OBJ_BANK            0x08

#define ROM_HEADER_CHECKSUM_LOC    0x014D

#ifndef MIN
    #define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif

struct cpu_registers_s
{
    /* Combine A and F registers. */
    union
    {
        struct
        {
            /* Define specific bits of Flag register. */
            union
            {
                struct
                {
                    uint8_t unused : 4;
                    uint8_t c : 1; /* Carry flag. */
                    uint8_t h : 1; /* Half carry flag. */
                    uint8_t n : 1; /* Add/sub flag. */
                    uint8_t z : 1; /* Zero flag. */
                } f_bits;
                uint8_t f;
            };
            uint8_t a;
        };
        uint16_t af;
    };

    union
    {
        struct
        {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    union
    {
        struct
        {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    union
    {
        struct
        {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp; /* Stack pointer */
    uint16_t pc; /* Program counter */
};

struct count_s
{
    uint_fast16_t lcd_count;    /* LCD Timing */
    uint_fast16_t div_count;    /* Divider Register Counter */
    uint_fast16_t tima_count;    /* Timer Counter */

  int16_t apu_len_count;  /* Length counter */
  int32_t apu_swp_count;  /* Sweep counter */
  int32_t apu_env_count;  /* Volume envelope counter */
  int8_t apu_wav_count; /* Count which wav sample is set to be mixed. */
};

struct gb_registers_s
{
    /* TODO: Sort variables in address order. */
    /* Timing */
    uint8_t TIMA, TMA, DIV;
    union
    {
        struct
        {
            uint8_t tac_rate : 2;    /* Input clock select */
            uint8_t tac_enable : 1;    /* Timer enable */
            uint8_t unused : 5;
        };
        uint8_t TAC;
    };

    /* LCD */
    uint8_t LCDC;
    uint8_t STAT;
    uint8_t SCY;
    uint8_t SCX;
    uint8_t LY;
    uint8_t LYC;
    uint8_t DMA;
    uint8_t BGP;
    uint8_t OBP0;
    uint8_t OBP1;
    uint8_t WY;
    uint8_t WX;

    /* Joypad info. */
    uint8_t P1;

    /* Serial data. */
    uint8_t SB;
    uint8_t SC;

    /* Interrupt flag. */
    uint8_t IF;

    /* Interrupt enable. */
    uint8_t IE;
};

enum  LCD{
  LCD_HBLANK = 0,
  LCD_VBLANK = 1,
  LCD_SEARCH_OAM = 2,
  LCD_TRANSFER = 3
};

/**
 * Emulator context.
 *
 * Only values within the `direct` struct may be modified directly by the
 * front-end implementation. Other variables must not be modified.
 */
struct gb_s
{
  struct
  {
    unsigned int  gb_halt : 1;
    unsigned int  gb_ime : 1;
    unsigned int  gb_frame : 1; /* New frame drawn. */
    enum LCD lcd_mode : 2;
  };

    uint8_t instance;
    uint8_t selected_rom_bank;
    /* WRAM and VRAM bank selection not available. */
    uint8_t cart_ram_bank;
    uint16_t cart_ram_bank_offset; //offset to subtract from the address to point to the right SRAM bank
    uint8_t enable_cart_ram;
    /* Cartridge ROM/RAM mode select. */
    uint8_t cart_mode_select;
    struct cpu_registers_s cpu_reg;
    struct gb_registers_s gb_reg;
    struct count_s counter;

    uint8_t rom[0x20000];
    uint8_t sram[SRAM_SIZE];
    uint8_t wram[WRAM_SIZE];
    uint8_t hram[HRAM_SIZE];

    uint16_t load_address;
    uint16_t init_address;
    uint16_t play_address;
    uint16_t stack_pointer;
    uint8_t timer_modulo;
    uint8_t timer_control;

    /* Audio */
    struct{
    uint16_t ch1Freq;
    uint8_t ch1SweepCounter;
    uint8_t ch1SweepCounterI;
    bool ch1SweepDir;
    uint8_t ch1SweepShift;
    uint8_t ch1Vol;
    uint8_t ch2Vol;
    uint8_t ch3Vol;
    uint8_t ch4Vol;
    uint8_t ch1VolI;
    uint8_t ch2VolI;
    uint8_t ch3VolI;
    uint8_t ch4VolI;
    uint8_t ch1Len;
    uint8_t ch2Len;
    uint8_t ch3Len;
    uint8_t ch4Len;
    uint8_t ch1LenI;
    uint8_t ch2LenI;
    uint8_t ch3LenI;
    uint8_t ch4LenI;
    bool ch1LenOn;
    bool ch2LenOn;
    bool ch3LenOn;
    bool ch4LenOn;
    uint8_t ch1EnvCounter;
    uint8_t ch2EnvCounter;
    uint8_t ch4EnvCounter;
    uint8_t ch1EnvCounterI;
    uint8_t ch2EnvCounterI;
    uint8_t ch4EnvCounterI;
    bool ch1EnvDir;
    bool ch2EnvDir;
    bool ch4EnvDir;
    bool ch1DAC;
    bool ch2DAC;
    bool ch4DAC;
    uint16_t WAVRAM[32];
    uint32_t idleTimer;
    } audio;
};


/**
 * Internal function used to read bytes.
 */
uint8_t __gb_read(struct gb_s *gb, const uint_fast16_t addr){
    switch(addr >> 12){
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    	return gb->rom[addr];
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x7:
    	return gb->rom[addr + ((gb->selected_rom_bank - 1) << 14)];
    case 0x8:
    case 0x9:
        return 0;
    case 0xA:
    case 0xB:
        return gb->sram[addr - gb->cart_ram_bank_offset];

    case 0xC:
    case 0xD:
        return gb->wram[addr - WRAM_0_ADDR];

    case 0xE:
        return gb->wram[addr - ECHO_ADDR];

    case 0xF:
        if(addr < OAM_ADDR)
            return gb->wram[addr - ECHO_ADDR];

        /* Unusable memory area */
        if(addr < IO_ADDR)
            return 0xFF;

        /* HRAM */
        if(HRAM_ADDR <= addr && addr < INTR_EN_ADDR)
            return gb->hram[addr - IO_ADDR];

        if((addr >= 0xFF10) && (addr <= 0xFF3F)){
            return gb->hram[addr - IO_ADDR] & APU_READ_MASK[addr - IO_ADDR];
        }

        /* IO and Interrupts. */
        switch(addr & 0xFF){
        /* IO Registers */
        case 0x00:
            return 0xC0 | gb->gb_reg.P1;

        case 0x01:
            return gb->gb_reg.SB;

        case 0x02:
            return gb->gb_reg.SC;

        /* Timer Registers */
        case 0x04:
            return gb->gb_reg.DIV;

        case 0x05:
            return gb->gb_reg.TIMA;

        case 0x06:
            return gb->gb_reg.TMA;

        case 0x07:
            return gb->gb_reg.TAC;

        /* Interrupt Flag Register */
        case 0x0F:
            return gb->gb_reg.IF;

        /* LCD Registers */
        case 0x40:
            return gb->gb_reg.LCDC;

        case 0x41:
            return (gb->gb_reg.STAT & STAT_USER_BITS) |
                   (gb->gb_reg.LCDC & LCDC_ENABLE ? gb->lcd_mode : LCD_VBLANK);

        case 0x42:
            return gb->gb_reg.SCY;

        case 0x43:
            return gb->gb_reg.SCX;

        case 0x44:
            return gb->gb_reg.LY;

        case 0x45:
            return gb->gb_reg.LYC;

        /* DMA Register */
        case 0x46:
            return gb->gb_reg.DMA;

        /* DMG Palette Registers */
        case 0x47:
            return gb->gb_reg.BGP;

        case 0x48:
            return gb->gb_reg.OBP0;

        case 0x49:
            return gb->gb_reg.OBP1;

        /* Window Position Registers */
        case 0x4A:
            return gb->gb_reg.WY;

    case 0x4B:
      return gb->gb_reg.WX;

        /* Interrupt Enable Register */
        case 0xFF:
            return gb->gb_reg.IE;

        /* Unused registers return 1 */
        default:
            return 0xFF;
        }
    }
    return 0xFF;
}

/**
 * Internal function used to write bytes.
 */
void __gb_write(struct gb_s *gb, const uint_fast16_t addr, const uint8_t val){
    switch(addr >> 12){
    case 0x0:
    case 0x1:
        gb->enable_cart_ram = ((val & 0x0F) == 0x0A);
        return;

    case 0x2:
    case 0x3:
        gb->selected_rom_bank = (val & 0x1F) | (gb->selected_rom_bank & 0x60);

        if((gb->selected_rom_bank & 0x1F) == 0x00)
            gb->selected_rom_bank++;
        gb->selected_rom_bank = gb->selected_rom_bank;
        return;

    case 0x4:
    case 0x5:
        gb->cart_ram_bank = (val & 3);
        gb->cart_ram_bank_offset = 0xA000 - (gb->cart_ram_bank << 13);
        gb->selected_rom_bank = ((val & 3) << 5) | (gb->selected_rom_bank & 0x1F);
        gb->selected_rom_bank = gb->selected_rom_bank;
        return;

    case 0x6:
    case 0x7:
        gb->cart_mode_select = (val & 1);
        return;

    case 0x8:
    case 0x9:
        return;

    case 0xA:
    case 0xB:
        if(gb->enable_cart_ram) gb->sram[addr - gb->cart_ram_bank_offset] = val;

        return;

    case 0xC:
    case 0xD:
        gb->wram[addr - WRAM_0_ADDR] = val;
        return;

    case 0xE:
        gb->wram[addr - ECHO_ADDR] = val;
        return;

    case 0xF:
        if(addr < OAM_ADDR){
            gb->wram[addr - ECHO_ADDR] = val;
            return;
        }

        /* Unusable memory area. */
        if(addr < IO_ADDR)
            return;

        if(HRAM_ADDR <= addr && addr < INTR_EN_ADDR){
            gb->hram[addr - IO_ADDR] = val;
            return;
        }

        if((addr >= 0xFF10) && (addr <= 0xFF2F)){
            if(gb->hram[addr - IO_ADDR] != val) gb->audio.idleTimer = 0;
            switch(addr & 0xFF){

                case 0x10://ch1 sweep
                    gb->hram[addr - IO_ADDR] = val;
                    gb->audio.ch1SweepDir = (val & 0x08) >> 3;
                    gb->audio.ch1SweepCounter = gb->audio.ch1SweepCounterI = (val & 0x70) >> 4;
                    gb->audio.ch1SweepShift = (val & 0x07);
                break;

                case 0x11://ch1 duty/length
                    gb->hram[addr - IO_ADDR] = val; 
                    gb->audio.ch1Len = gb->audio.ch1LenI = 64 - (val & 0x3F);
                break;

                case 0x16://ch2 duty/length
                    gb->hram[addr - IO_ADDR] = val; 
                    gb->audio.ch2Len = gb->audio.ch2LenI = 64 - (val & 0x3F);
                break;

                case 0x1B://ch3 length
                    gb->hram[addr - IO_ADDR] = val; 
                    gb->audio.ch3Len = gb->audio.ch3LenI = 256 - val;
                break;

                case 0x20://ch4 length
                    gb->hram[addr - IO_ADDR] = val; 
                    gb->audio.ch4Len = gb->audio.ch4LenI = 64 - (val & 0x3F);
                break;

                case 0x12://ch1 envelope
                    gb->hram[addr - IO_ADDR] = val;
                    gb->audio.ch1DAC = (val & 0xF8) > 0;
                    gb->audio.ch1Vol = gb->audio.ch1VolI = (val & 0xF0) >> 4;
                    gb->audio.ch1EnvDir = (val & 0x08) >> 3;
                    gb->audio.ch1EnvCounter = gb->audio.ch1EnvCounterI = (val & 0x07);
                break;

                case 0x17://ch2 envelope
                    gb->hram[addr - IO_ADDR] = val;
                    gb->audio.ch2DAC = (val & 0xF8) > 0;
                    gb->audio.ch2Vol = gb->audio.ch2VolI = (val & 0xF0) >> 4;
                    gb->audio.ch2EnvDir = (val & 0x08) >> 3;
                    gb->audio.ch2EnvCounter = gb->audio.ch2EnvCounterI = (val & 0x07);
                break;

                case 0x1C://ch3 Volume (on hardware, this bitshifts the wav samples. The method here is quicker, sounds better, but is less accurate compared to hardware)
                    gb->hram[addr - IO_ADDR] = val;
                    switch((val & 0x60)){
                        case 0x00://mute
                            gb->audio.ch3Vol = gb->audio.ch3VolI = 8;
                        break;
                        case 0x20://full
                            gb->audio.ch3Vol = gb->audio.ch3VolI = 0;
                        break;
                        case 0x40://half
                            gb->audio.ch3Vol = gb->audio.ch3VolI = 2;
                        break;
                        case 0x60://quarter
                            gb->audio.ch3Vol = gb->audio.ch3VolI = 3;
                        break;
                    }

                break;

                case 0x21://ch4 envelope
                    gb->hram[addr - IO_ADDR] = val;
                    gb->audio.ch4DAC = (val & 0xF8) > 0;
                    gb->audio.ch4Vol = gb->audio.ch4VolI = (val & 0xF0) >> 4;
                    gb->audio.ch4EnvDir = (val & 0x08) >> 3;
                    gb->audio.ch4EnvCounter = gb->audio.ch4EnvCounterI = (val & 0x07);
                break;

                case 0x14://ch1 retrigger sound
                    gb->hram[addr - IO_ADDR] = val;
                    if(val&0x80){
                        gb->audio.ch1Vol = gb->audio.ch1VolI;
                        if(gb->audio.ch1DAC) gb->hram[0x26] |= 0x01;
                        gb->audio.ch1SweepCounter = gb->audio.ch1SweepCounterI;
                        gb->audio.ch1EnvCounter = gb->audio.ch1EnvCounterI;
                        gb->audio.ch1Len = gb->audio.ch1LenI;
                    }
                    if(val&0x40){
                        gb->audio.ch1LenOn = 1;
                    }else{
                        gb->audio.ch1LenOn = 0;
                    }
                break;

                case 0x19://ch2 retrigger sound
                    gb->hram[addr - IO_ADDR] = val;
                    if(val&0x80){
                        gb->audio.ch2Vol = gb->audio.ch2VolI;
                        if(gb->audio.ch2DAC) gb->hram[0x26] |= 0x02;
                        gb->audio.ch2EnvCounter = gb->audio.ch2EnvCounterI;
                        gb->audio.ch2Len = gb->audio.ch2LenI;
                    }
                    if(val&0x40){
                        gb->audio.ch2LenOn = 1;
                    }else{
                        gb->audio.ch2LenOn = 0;
                    }
                break;

                case 0x1E://ch3 retrigger sound
                    gb->hram[addr - IO_ADDR] = val;
                    if(val&0x80){
                        gb->audio.ch3Vol = gb->audio.ch3VolI;
                        if(gb->hram[0x1A] & 0x80) gb->hram[0x26] |= 0x04;
                        gb->audio.ch3Len = gb->audio.ch3LenI;
                    }
                    if(val&0x40){
                        gb->audio.ch3LenOn = 1;
                    }else{
                        gb->audio.ch3LenOn = 0;
                    }
                break;

                case 0x23://ch4 retrigger sound
                    gb->hram[addr - IO_ADDR] = val;
                    if(val&0x80){
                        gb->audio.ch4Vol = gb->audio.ch4VolI;
                        //if(gb->audio.ch4DAC) 
                        gb->hram[0x26] |= 0x08;
                        gb->audio.ch4EnvCounter = gb->audio.ch4EnvCounterI;
                        gb->audio.ch4Len = gb->audio.ch4LenI;
                    }
                    if(val&0x40){
                        gb->audio.ch4LenOn = 1;
                    }else{
                        gb->audio.ch4LenOn = 0;
                    }
                break;

                default: gb->hram[addr - IO_ADDR] = (val & APU_WRITE_MASK[addr - IO_ADDR]) + (gb->hram[addr - IO_ADDR] & (APU_WRITE_MASK[addr - IO_ADDR]^0xFF)); break;
                }
            return;
        }

        if((addr >= 0xFF30) && (addr <= 0xFF3F)){
            gb->audio.WAVRAM[((addr & 0x0F) << 1)] = -15 + ((val & 0xF0) >> 3);
            gb->audio.WAVRAM[((addr & 0x0F) << 1) + 1] = -15 + ((val & 0x0F) << 1);
        return;
        }


        /* IO and Interrupts. */
        switch(addr & 0xFF){

        /* Timer Registers */
        case 0x04:
            gb->gb_reg.DIV = 0x00;
            gb->counter.tima_count = 0x00;
            return;

        case 0x05:
            gb->gb_reg.TIMA = val;
            return;

        case 0x06:
            gb->gb_reg.TMA = val;
            return;

        case 0x07:
            gb->gb_reg.TAC = val;
            return;

        /* Interrupt Flag Register */
        case 0x0F:
            gb->gb_reg.IF = (val | 0b11100000);
            return;

        /* LCD Registers */
        case 0x40:
            gb->gb_reg.LCDC = val;

            /* LY fixed to 0 when LCD turned off. */
            if((gb->gb_reg.LCDC & LCDC_ENABLE) == 0){
                /* Do not turn off LCD outside of VBLANK. This may
                 * happen due to poor timing in this emulator. */
                if(gb->lcd_mode != LCD_VBLANK){
                    gb->gb_reg.LCDC |= LCDC_ENABLE;
                    return;
                }

                gb->gb_reg.STAT = (gb->gb_reg.STAT & ~0x03) | LCD_VBLANK;
                gb->gb_reg.LY = 0;
                gb->counter.lcd_count = 0;
            }

            return;

        case 0x41:
            gb->gb_reg.STAT = (val & 0b01111000);
            return;

        case 0x42:
            gb->gb_reg.SCY = val;
            return;

        case 0x43:
            gb->gb_reg.SCX = val;
            return;

        /* LY (0xFF44) is read only. */
        case 0x45:
            gb->gb_reg.LYC = val;
            return;
      
        /* Interrupt Enable Register */
        case 0xFF:
            gb->gb_reg.IE = val;
            return;
        }
    }
}


uint8_t __gb_execute_cb(struct gb_s *gb){
  uint8_t inst_cycles;
    uint8_t cbop = __gb_read(gb, gb->cpu_reg.pc++);
    uint8_t r = (cbop & 0x7);
    uint8_t b = (cbop >> 3) & 0x7;
    uint8_t d = (cbop >> 3) & 0x1;
    uint8_t val;
    uint8_t writeback = 1;

  inst_cycles = 8;
    /* Add an additional 8 cycles to these sets of instructions. */
    switch(cbop & 0xC7){
    case 0x06:
    case 0x86:
  case 0xC6:
      inst_cycles += 8;
  break;
  case 0x46:
      inst_cycles += 4;
  break;
    }

    switch(r){
    case 0:
        val = gb->cpu_reg.b;
        break;

    case 1:
        val = gb->cpu_reg.c;
        break;

    case 2:
        val = gb->cpu_reg.d;
        break;

    case 3:
        val = gb->cpu_reg.e;
        break;

    case 4:
        val = gb->cpu_reg.h;
        break;

    case 5:
        val = gb->cpu_reg.l;
        break;

    case 6:
        val = __gb_read(gb, gb->cpu_reg.hl);
        break;

    /* Only values 0-7 are possible here, so we make the final case
     * default to satisfy -Wmaybe-uninitialized warning. */
    default:
        val = gb->cpu_reg.a;
        break;
    }

    /* TODO: Find out WTF this is doing. */
    switch(cbop >> 6){
    case 0x0:
        cbop = (cbop >> 4) & 0x3;

        switch(cbop){
        case 0x0: /* RdC R */
        case 0x1: /* Rd R */
            if(d) /* RRC R / RR R */
            {
                uint8_t temp = val;
                val = (val >> 1);
                val |= cbop ? (gb->cpu_reg.f_bits.c << 7) : (temp << 7);
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
                gb->cpu_reg.f_bits.c = (temp & 0x01);
            }
            else /* RLC R / RL R */
            {
                uint8_t temp = val;
                val = (val << 1);
                val |= cbop ? gb->cpu_reg.f_bits.c : (temp >> 7);
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
                gb->cpu_reg.f_bits.c = (temp >> 7);
            }

            break;

        case 0x2:
            if(d) /* SRA R */
            {
                gb->cpu_reg.f_bits.c = val & 0x01;
                val = (val >> 1) | (val & 0x80);
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
            }
            else /* SLA R */
            {
                gb->cpu_reg.f_bits.c = (val >> 7);
                val = val << 1;
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
            }

            break;

        case 0x3:
            if(d) /* SRL R */
            {
                gb->cpu_reg.f_bits.c = val & 0x01;
                val = val >> 1;
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
            }
            else /* SWAP R */
            {
                uint8_t temp = (val >> 4) & 0x0F;
                temp |= (val << 4) & 0xF0;
                val = temp;
                gb->cpu_reg.f_bits.z = (val == 0x00);
                gb->cpu_reg.f_bits.n = 0;
                gb->cpu_reg.f_bits.h = 0;
                gb->cpu_reg.f_bits.c = 0;
            }

            break;
        }

        break;

    case 0x1: /* BIT B, R */
        gb->cpu_reg.f_bits.z = !((val >> b) & 0x1);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        writeback = 0;
        break;

    case 0x2: /* RES B, R */
        val &= (0xFE << b) | (0xFF >> (8 - b));
        break;

    case 0x3: /* SET B, R */
        val |= (0x1 << b);
        break;
    }

    if(writeback){
        switch(r){
        case 0:
            gb->cpu_reg.b = val;
            break;

        case 1:
            gb->cpu_reg.c = val;
            break;

        case 2:
            gb->cpu_reg.d = val;
            break;

        case 3:
            gb->cpu_reg.e = val;
            break;

        case 4:
            gb->cpu_reg.h = val;
            break;

        case 5:
            gb->cpu_reg.l = val;
            break;

        case 6:
            __gb_write(gb, gb->cpu_reg.hl, val);
            break;

        case 7:
            gb->cpu_reg.a = val;
            break;
        }
    }
 return inst_cycles;
}

/**
 * Internal function used to step the CPU.
 */
void __gb_step_cpu(struct gb_s *gb){
    if(gb->cpu_reg.pc < 0x0010){  // Hack to help handle GBS
        gb->cpu_reg.pc = 0;
        gb->gb_halt = 1;
        gb->gb_ime = 1;
        gb->counter.lcd_count = LCD_LINE_CYCLES + 1;
        gb->gb_reg.LY = LCD_HEIGHT - 1;
    }
    uint8_t opcode, inst_cycles;
    static const uint8_t op_cycles[0x100] =
    {
        /* *INDENT-OFF* */
        /*0 1 2  3  4  5  6  7  8  9  A  B  C  D  E  F    */
        4,12, 8, 8, 4, 4, 8, 4,20, 8, 8, 8, 4, 4, 8, 4,    /* 0x00 */
        4,12, 8, 8, 4, 4, 8, 4,12, 8, 8, 8, 4, 4, 8, 4,    /* 0x10 */
        8,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,    /* 0x20 */
        8,12, 8, 8,12,12,12, 4, 8, 8, 8, 8, 4, 4, 8, 4,    /* 0x30 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x40 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x50 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x60 */
        8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x70 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x80 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0x90 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0xA0 */
        4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    /* 0xB0 */
        8,12,12,16,12,16, 8,16, 8,16,12, 8,12,24, 8,16,    /* 0xC0 */
        8,12,12, 0,12,16, 8,16, 8,16,12, 0,12, 0, 8,16,    /* 0xD0 */
        12,12,8, 0, 0,16, 8,16,16, 4,16, 0, 0, 0, 8,16,    /* 0xE0 */
        12,12,8, 4, 0,16, 8,16,12, 8,16, 4, 0, 0, 8,16    /* 0xF0 */
        /* *INDENT-ON* */
    };

    /* Handle interrupts */
    if((gb->gb_ime || gb->gb_halt) &&
            (gb->gb_reg.IF & gb->gb_reg.IE & ANY_INTR)){
        gb->gb_halt = 0;

        if(gb->gb_ime){
            /* Disable interrupts */
            gb->gb_ime = 0;

            /* Push Program Counter */
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);

            /* Call interrupt handler if required. */
            if(gb->gb_reg.IF & gb->gb_reg.IE & VBLANK_INTR){
                gb->cpu_reg.pc = gb->play_address;
                gb->gb_reg.IF ^= VBLANK_INTR;
            }
            else if(gb->gb_reg.IF & gb->gb_reg.IE & LCDC_INTR){
                gb->cpu_reg.pc = gb->play_address;
                gb->gb_reg.IF ^= LCDC_INTR;
            }
            else if(gb->gb_reg.IF & gb->gb_reg.IE & TIMER_INTR){
                gb->cpu_reg.pc = gb->play_address;
                gb->gb_reg.IF ^= TIMER_INTR;
            }
            else if(gb->gb_reg.IF & gb->gb_reg.IE & SERIAL_INTR){
                gb->cpu_reg.pc = gb->play_address;
                gb->gb_reg.IF ^= SERIAL_INTR;
            }
            else if(gb->gb_reg.IF & gb->gb_reg.IE & CONTROL_INTR){
                gb->cpu_reg.pc = gb->play_address;
                gb->gb_reg.IF ^= CONTROL_INTR;
            }
        }
    }

    /* Obtain opcode */
    opcode = (gb->gb_halt ? 0x00 : __gb_read(gb, gb->cpu_reg.pc++));
    inst_cycles = op_cycles[opcode];

    /* Execute opcode */
    switch(opcode){
    case 0x00: /* NOP */
        break;

    case 0x01: /* LD BC, imm */
        gb->cpu_reg.c = __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.b = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x02: /* LD (BC), A */
        __gb_write(gb, gb->cpu_reg.bc, gb->cpu_reg.a);
        break;

    case 0x03: /* INC BC */
        gb->cpu_reg.bc++;
        break;

    case 0x04: /* INC B */
        gb->cpu_reg.b++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.b == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.b & 0x0F) == 0x00);
        break;

    case 0x05: /* DEC B */
        gb->cpu_reg.b--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.b == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.b & 0x0F) == 0x0F);
        break;

    case 0x06: /* LD B, imm */
        gb->cpu_reg.b = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x07: /* RLCA */
        gb->cpu_reg.a = (gb->cpu_reg.a << 1) | (gb->cpu_reg.a >> 7);
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = (gb->cpu_reg.a & 0x01);
        break;

    case 0x08: /* LD (imm), SP */
    {
        uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
        temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
        __gb_write(gb, temp++, gb->cpu_reg.sp & 0xFF);
        __gb_write(gb, temp, gb->cpu_reg.sp >> 8);
        break;
    }

    case 0x09: /* ADD HL, BC */
    {
        uint_fast32_t temp = gb->cpu_reg.hl + gb->cpu_reg.bc;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (temp ^ gb->cpu_reg.hl ^ gb->cpu_reg.bc) & 0x1000 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFFFF0000) ? 1 : 0;
        gb->cpu_reg.hl = (temp & 0x0000FFFF);
        break;
    }

    case 0x0A: /* LD A, (BC) */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.bc);
        break;

    case 0x0B: /* DEC BC */
        gb->cpu_reg.bc--;
        break;

    case 0x0C: /* INC C */
        gb->cpu_reg.c++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.c == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.c & 0x0F) == 0x00);
        break;

    case 0x0D: /* DEC C */
        gb->cpu_reg.c--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.c == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.c & 0x0F) == 0x0F);
        break;

    case 0x0E: /* LD C, imm */
        gb->cpu_reg.c = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x0F: /* RRCA */
        gb->cpu_reg.f_bits.c = gb->cpu_reg.a & 0x01;
        gb->cpu_reg.a = (gb->cpu_reg.a >> 1) | (gb->cpu_reg.a << 7);
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        break;

    case 0x10: /* STOP */
        //gb->gb_halt = 1;
        break;

    case 0x11: /* LD DE, imm */
        gb->cpu_reg.e = __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.d = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x12: /* LD (DE), A */
        __gb_write(gb, gb->cpu_reg.de, gb->cpu_reg.a);
        break;

    case 0x13: /* INC DE */
        gb->cpu_reg.de++;
        break;

    case 0x14: /* INC D */
        gb->cpu_reg.d++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.d == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.d & 0x0F) == 0x00);
        break;

    case 0x15: /* DEC D */
        gb->cpu_reg.d--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.d == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.d & 0x0F) == 0x0F);
        break;

    case 0x16: /* LD D, imm */
        gb->cpu_reg.d = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x17: /* RLA */
    {
        uint8_t temp = gb->cpu_reg.a;
        gb->cpu_reg.a = (gb->cpu_reg.a << 1) | gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = (temp >> 7) & 0x01;
        break;
    }

    case 0x18: /* JR imm */
    {
        int8_t temp = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.pc += temp;
        break;
    }

    case 0x19: /* ADD HL, DE */
    {
        uint_fast32_t temp = gb->cpu_reg.hl + gb->cpu_reg.de;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (temp ^ gb->cpu_reg.hl ^ gb->cpu_reg.de) & 0x1000 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFFFF0000) ? 1 : 0;
        gb->cpu_reg.hl = (temp & 0x0000FFFF);
        break;
    }

    case 0x1A: /* LD A, (DE) */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.de);
        break;

    case 0x1B: /* DEC DE */
        gb->cpu_reg.de--;
        break;

    case 0x1C: /* INC E */
        gb->cpu_reg.e++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.e == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.e & 0x0F) == 0x00);
        break;

    case 0x1D: /* DEC E */
        gb->cpu_reg.e--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.e == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.e & 0x0F) == 0x0F);
        break;

    case 0x1E: /* LD E, imm */
        gb->cpu_reg.e = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x1F: /* RRA */
    {
        uint8_t temp = gb->cpu_reg.a;
        gb->cpu_reg.a = gb->cpu_reg.a >> 1 | (gb->cpu_reg.f_bits.c << 7);
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = temp & 0x1;
        break;
    }

    case 0x20: /* JP NZ, imm */
        if(!gb->cpu_reg.f_bits.z){
            int8_t temp = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
            gb->cpu_reg.pc += temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc++;

        break;

    case 0x21: /* LD HL, imm */
        gb->cpu_reg.l = __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.h = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x22: /* LDI (HL), A */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.a);
        gb->cpu_reg.hl++;
        break;

    case 0x23: /* INC HL */
        gb->cpu_reg.hl++;
        break;

    case 0x24: /* INC H */
        gb->cpu_reg.h++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.h == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.h & 0x0F) == 0x00);
        break;

    case 0x25: /* DEC H */
        gb->cpu_reg.h--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.h == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.h & 0x0F) == 0x0F);
        break;

    case 0x26: /* LD H, imm */
        gb->cpu_reg.h = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x27: /* DAA */
    {
        uint16_t a = gb->cpu_reg.a;

        if(gb->cpu_reg.f_bits.n){
            if(gb->cpu_reg.f_bits.h)
                a = (a - 0x06) & 0xFF;

            if(gb->cpu_reg.f_bits.c)
                a -= 0x60;
        }
        else
        {
            if(gb->cpu_reg.f_bits.h || (a & 0x0F) > 9)
                a += 0x06;

            if(gb->cpu_reg.f_bits.c || a > 0x9F)
                a += 0x60;
        }

        if((a & 0x100) == 0x100)
            gb->cpu_reg.f_bits.c = 1;

        gb->cpu_reg.a = a;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0);
        gb->cpu_reg.f_bits.h = 0;

        break;
    }

    case 0x28: /* JP Z, imm */
        if(gb->cpu_reg.f_bits.z){
            int8_t temp = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
            gb->cpu_reg.pc += temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc++;

        break;

    case 0x29: /* ADD HL, HL */
    {
        uint_fast32_t temp = gb->cpu_reg.hl + gb->cpu_reg.hl;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = (temp & 0x1000) ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFFFF0000) ? 1 : 0;
        gb->cpu_reg.hl = (temp & 0x0000FFFF);
        break;
    }

    case 0x2A: /* LD A, (HL+) */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.hl++);
        break;

    case 0x2B: /* DEC HL */
        gb->cpu_reg.hl--;
        break;

    case 0x2C: /* INC L */
        gb->cpu_reg.l++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.l == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.l & 0x0F) == 0x00);
        break;

    case 0x2D: /* DEC L */
        gb->cpu_reg.l--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.l == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.l & 0x0F) == 0x0F);
        break;

    case 0x2E: /* LD L, imm */
        gb->cpu_reg.l = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x2F: /* CPL */
        gb->cpu_reg.a = ~gb->cpu_reg.a;
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = 1;
        break;

    case 0x30: /* JP NC, imm */
        if(!gb->cpu_reg.f_bits.c){
            int8_t temp = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
            gb->cpu_reg.pc += temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc++;

        break;

    case 0x31: /* LD SP, imm */
        gb->cpu_reg.sp = __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.sp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
        break;

    case 0x32: /* LD (HL), A */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.a);
        gb->cpu_reg.hl--;
        break;

    case 0x33: /* INC SP */
        gb->cpu_reg.sp++;
        break;

    case 0x34: /* INC (HL) */
    {
        uint8_t temp = __gb_read(gb, gb->cpu_reg.hl) + 1;
        gb->cpu_reg.f_bits.z = (temp == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((temp & 0x0F) == 0x00);
        __gb_write(gb, gb->cpu_reg.hl, temp);
        break;
    }

    case 0x35: /* DEC (HL) */
    {
        uint8_t temp = __gb_read(gb, gb->cpu_reg.hl) - 1;
        gb->cpu_reg.f_bits.z = (temp == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((temp & 0x0F) == 0x0F);
        __gb_write(gb, gb->cpu_reg.hl, temp);
        break;
    }

    case 0x36: /* LD (HL), imm */
        __gb_write(gb, gb->cpu_reg.hl, __gb_read(gb, gb->cpu_reg.pc++));
        break;

    case 0x37: /* SCF */
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 1;
        break;

    case 0x38: /* JP C, imm */
        if(gb->cpu_reg.f_bits.c){
            int8_t temp = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
            gb->cpu_reg.pc += temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc++;

        break;

    case 0x39: /* ADD HL, SP */
    {
        uint_fast32_t temp = gb->cpu_reg.hl + gb->cpu_reg.sp;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            ((gb->cpu_reg.hl & 0xFFF) + (gb->cpu_reg.sp & 0xFFF)) & 0x1000 ? 1 : 0;
        gb->cpu_reg.f_bits.c = temp & 0x10000 ? 1 : 0;
        gb->cpu_reg.hl = (uint16_t)temp;
        break;
    }

    case 0x3A: /* LD A, (HL--) */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.hl--);
        break;

    case 0x3B: /* DEC SP */
        gb->cpu_reg.sp--;
        break;

    case 0x3C: /* INC A */
        gb->cpu_reg.a++;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.a & 0x0F) == 0x00);
        break;

    case 0x3D: /* DEC A */
        gb->cpu_reg.a--;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.a & 0x0F) == 0x0F);
        break;

    case 0x3E: /* LD A, imm */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.pc++);
        break;

    case 0x3F: /* CCF */
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = ~gb->cpu_reg.f_bits.c;
        break;

    case 0x40: /* LD B, B */
        break;

    case 0x41: /* LD B, C */
        gb->cpu_reg.b = gb->cpu_reg.c;
        break;

    case 0x42: /* LD B, D */
        gb->cpu_reg.b = gb->cpu_reg.d;
        break;

    case 0x43: /* LD B, E */
        gb->cpu_reg.b = gb->cpu_reg.e;
        break;

    case 0x44: /* LD B, H */
        gb->cpu_reg.b = gb->cpu_reg.h;
        break;

    case 0x45: /* LD B, L */
        gb->cpu_reg.b = gb->cpu_reg.l;
        break;

    case 0x46: /* LD B, (HL) */
        gb->cpu_reg.b = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x47: /* LD B, A */
        gb->cpu_reg.b = gb->cpu_reg.a;
        break;

    case 0x48: /* LD C, B */
        gb->cpu_reg.c = gb->cpu_reg.b;
        break;

    case 0x49: /* LD C, C */
        break;

    case 0x4A: /* LD C, D */
        gb->cpu_reg.c = gb->cpu_reg.d;
        break;

    case 0x4B: /* LD C, E */
        gb->cpu_reg.c = gb->cpu_reg.e;
        break;

    case 0x4C: /* LD C, H */
        gb->cpu_reg.c = gb->cpu_reg.h;
        break;

    case 0x4D: /* LD C, L */
        gb->cpu_reg.c = gb->cpu_reg.l;
        break;

    case 0x4E: /* LD C, (HL) */
        gb->cpu_reg.c = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x4F: /* LD C, A */
        gb->cpu_reg.c = gb->cpu_reg.a;
        break;

    case 0x50: /* LD D, B */
        gb->cpu_reg.d = gb->cpu_reg.b;
        break;

    case 0x51: /* LD D, C */
        gb->cpu_reg.d = gb->cpu_reg.c;
        break;

    case 0x52: /* LD D, D */
        break;

    case 0x53: /* LD D, E */
        gb->cpu_reg.d = gb->cpu_reg.e;
        break;

    case 0x54: /* LD D, H */
        gb->cpu_reg.d = gb->cpu_reg.h;
        break;

    case 0x55: /* LD D, L */
        gb->cpu_reg.d = gb->cpu_reg.l;
        break;

    case 0x56: /* LD D, (HL) */
        gb->cpu_reg.d = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x57: /* LD D, A */
        gb->cpu_reg.d = gb->cpu_reg.a;
        break;

    case 0x58: /* LD E, B */
        gb->cpu_reg.e = gb->cpu_reg.b;
        break;

    case 0x59: /* LD E, C */
        gb->cpu_reg.e = gb->cpu_reg.c;
        break;

    case 0x5A: /* LD E, D */
        gb->cpu_reg.e = gb->cpu_reg.d;
        break;

    case 0x5B: /* LD E, E */
        break;

    case 0x5C: /* LD E, H */
        gb->cpu_reg.e = gb->cpu_reg.h;
        break;

    case 0x5D: /* LD E, L */
        gb->cpu_reg.e = gb->cpu_reg.l;
        break;

    case 0x5E: /* LD E, (HL) */
        gb->cpu_reg.e = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x5F: /* LD E, A */
        gb->cpu_reg.e = gb->cpu_reg.a;
        break;

    case 0x60: /* LD H, B */
        gb->cpu_reg.h = gb->cpu_reg.b;
        break;

    case 0x61: /* LD H, C */
        gb->cpu_reg.h = gb->cpu_reg.c;
        break;

    case 0x62: /* LD H, D */
        gb->cpu_reg.h = gb->cpu_reg.d;
        break;

    case 0x63: /* LD H, E */
        gb->cpu_reg.h = gb->cpu_reg.e;
        break;

    case 0x64: /* LD H, H */
        break;

    case 0x65: /* LD H, L */
        gb->cpu_reg.h = gb->cpu_reg.l;
        break;

    case 0x66: /* LD H, (HL) */
        gb->cpu_reg.h = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x67: /* LD H, A */
        gb->cpu_reg.h = gb->cpu_reg.a;
        break;

    case 0x68: /* LD L, B */
        gb->cpu_reg.l = gb->cpu_reg.b;
        break;

    case 0x69: /* LD L, C */
        gb->cpu_reg.l = gb->cpu_reg.c;
        break;

    case 0x6A: /* LD L, D */
        gb->cpu_reg.l = gb->cpu_reg.d;
        break;

    case 0x6B: /* LD L, E */
        gb->cpu_reg.l = gb->cpu_reg.e;
        break;

    case 0x6C: /* LD L, H */
        gb->cpu_reg.l = gb->cpu_reg.h;
        break;

    case 0x6D: /* LD L, L */
        break;

    case 0x6E: /* LD L, (HL) */
        gb->cpu_reg.l = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x6F: /* LD L, A */
        gb->cpu_reg.l = gb->cpu_reg.a;
        break;

    case 0x70: /* LD (HL), B */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.b);
        break;

    case 0x71: /* LD (HL), C */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.c);
        break;

    case 0x72: /* LD (HL), D */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.d);
        break;

    case 0x73: /* LD (HL), E */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.e);
        break;

    case 0x74: /* LD (HL), H */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.h);
        break;

    case 0x75: /* LD (HL), L */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.l);
        break;

    case 0x76: /* HALT */
        /* TODO: Emulate HALT bug? */
        gb->gb_halt = 1;
        break;

    case 0x77: /* LD (HL), A */
        __gb_write(gb, gb->cpu_reg.hl, gb->cpu_reg.a);
        break;

    case 0x78: /* LD A, B */
        gb->cpu_reg.a = gb->cpu_reg.b;
        break;

    case 0x79: /* LD A, C */
        gb->cpu_reg.a = gb->cpu_reg.c;
        break;

    case 0x7A: /* LD A, D */
        gb->cpu_reg.a = gb->cpu_reg.d;
        break;

    case 0x7B: /* LD A, E */
        gb->cpu_reg.a = gb->cpu_reg.e;
        break;

    case 0x7C: /* LD A, H */
        gb->cpu_reg.a = gb->cpu_reg.h;
        break;

    case 0x7D: /* LD A, L */
        gb->cpu_reg.a = gb->cpu_reg.l;
        break;

    case 0x7E: /* LD A, (HL) */
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.hl);
        break;

    case 0x7F: /* LD A, A */
        break;

    case 0x80: /* ADD A, B */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.b ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x81: /* ADD A, C */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.c ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x82: /* ADD A, D */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.d ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x83: /* ADD A, E */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.e ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x84: /* ADD A, H */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.h ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x85: /* ADD A, L */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.l ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x86: /* ADD A, (HL) */
    {
        uint8_t hl = __gb_read(gb, gb->cpu_reg.hl);
        uint16_t temp = gb->cpu_reg.a + hl;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ hl ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x87: /* ADD A, A */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.a;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = temp & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x88: /* ADC A, B */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.b + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.b ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x89: /* ADC A, C */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.c + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.c ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8A: /* ADC A, D */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.d + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.d ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8B: /* ADC A, E */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.e + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.e ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8C: /* ADC A, H */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.h + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.h ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8D: /* ADC A, L */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.l + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.l ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8E: /* ADC A, (HL) */
    {
        uint8_t val = __gb_read(gb, gb->cpu_reg.hl);
        uint16_t temp = gb->cpu_reg.a + val + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ val ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x8F: /* ADC A, A */
    {
        uint16_t temp = gb->cpu_reg.a + gb->cpu_reg.a + gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        /* TODO: Optimisation here? */
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.a ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x90: /* SUB B */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.b ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x91: /* SUB C */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.c ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x92: /* SUB D */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.d ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x93: /* SUB E */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.e ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x94: /* SUB H */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.h ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x95: /* SUB L */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.l ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x96: /* SUB (HL) */
    {
        uint8_t val = __gb_read(gb, gb->cpu_reg.hl);
        uint16_t temp = gb->cpu_reg.a - val;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ val ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x97: /* SUB A */
        gb->cpu_reg.a = 0;
        gb->cpu_reg.f_bits.z = 1;
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0x98: /* SBC A, B */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.b - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.b ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x99: /* SBC A, C */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.c - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.c ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9A: /* SBC A, D */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.d - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.d ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9B: /* SBC A, E */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.e - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.e ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9C: /* SBC A, H */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.h - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.h ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9D: /* SBC A, L */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.l - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.l ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9E: /* SBC A, (HL) */
    {
        uint8_t val = __gb_read(gb, gb->cpu_reg.hl);
        uint16_t temp = gb->cpu_reg.a - val - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ val ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0x9F: /* SBC A, A */
        gb->cpu_reg.a = gb->cpu_reg.f_bits.c ? 0xFF : 0x00;
        gb->cpu_reg.f_bits.z = gb->cpu_reg.f_bits.c ? 0x00 : 0x01;
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = gb->cpu_reg.f_bits.c;
        break;

    case 0xA0: /* AND B */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA1: /* AND C */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA2: /* AND D */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA3: /* AND E */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA4: /* AND H */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA5: /* AND L */
        gb->cpu_reg.a = gb->cpu_reg.a & gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA6: /* AND (HL) */
        gb->cpu_reg.a = gb->cpu_reg.a & __gb_read(gb, gb->cpu_reg.hl);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA7: /* AND A */
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA8: /* XOR B */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xA9: /* XOR C */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAA: /* XOR D */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAB: /* XOR E */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAC: /* XOR H */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAD: /* XOR L */
        gb->cpu_reg.a = gb->cpu_reg.a ^ gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAE: /* XOR (HL) */
        gb->cpu_reg.a = gb->cpu_reg.a ^ __gb_read(gb, gb->cpu_reg.hl);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xAF: /* XOR A */
        gb->cpu_reg.a = 0x00;
        gb->cpu_reg.f_bits.z = 1;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB0: /* OR B */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB1: /* OR C */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB2: /* OR D */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB3: /* OR E */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB4: /* OR H */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB5: /* OR L */
        gb->cpu_reg.a = gb->cpu_reg.a | gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB6: /* OR (HL) */
        gb->cpu_reg.a = gb->cpu_reg.a | __gb_read(gb, gb->cpu_reg.hl);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB7: /* OR A */
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xB8: /* CP B */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.b;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.b ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xB9: /* CP C */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.c;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.c ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xBA: /* CP D */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.d;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.d ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xBB: /* CP E */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.e;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.e ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xBC: /* CP H */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.h;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.h ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xBD: /* CP L */
    {
        uint16_t temp = gb->cpu_reg.a - gb->cpu_reg.l;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ gb->cpu_reg.l ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    /* TODO: Optimsation by combining similar opcode routines. */
    case 0xBE: /* CP (HL) */
    {
        uint8_t val = __gb_read(gb, gb->cpu_reg.hl);
        uint16_t temp = gb->cpu_reg.a - val;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ val ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xBF: /* CP A */
        gb->cpu_reg.f_bits.z = 1;
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xC0: /* RET NZ */
        if(!gb->cpu_reg.f_bits.z){
            gb->cpu_reg.pc = __gb_read(gb, gb->cpu_reg.sp++);
            gb->cpu_reg.pc |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
            inst_cycles += 12;
        }

        break;

    case 0xC1: /* POP BC */
        gb->cpu_reg.c = __gb_read(gb, gb->cpu_reg.sp++);
        gb->cpu_reg.b = __gb_read(gb, gb->cpu_reg.sp++);
        break;

    case 0xC2: /* JP NZ, imm */
        if(!gb->cpu_reg.f_bits.z){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xC3: /* JP imm */
    {
        uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
        temp |= __gb_read(gb, gb->cpu_reg.pc) << 8;
        gb->cpu_reg.pc = temp;
        break;
    }

    case 0xC4: /* CALL NZ imm */
        if(!gb->cpu_reg.f_bits.z){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xC5: /* PUSH BC */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.b);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.c);
        break;

    case 0xC6: /* ADD A, imm */
    {
        /* Taken from SameBoy, which is released under MIT Licence. */
        uint8_t value = __gb_read(gb, gb->cpu_reg.pc++);
        uint16_t calc = gb->cpu_reg.a + value;
        gb->cpu_reg.f_bits.z = ((uint8_t)calc == 0) ? 1 : 0;
        gb->cpu_reg.f_bits.h =
            ((gb->cpu_reg.a & 0xF) + (value & 0xF) > 0x0F) ? 1 : 0;
        gb->cpu_reg.f_bits.c = calc > 0xFF ? 1 : 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.a = (uint8_t)calc;
        break;
    }

    case 0xC7: /* RST 0x0000 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0000 + gb->load_address;
        break;

    case 0xC8: /* RET Z */
        if(gb->cpu_reg.f_bits.z){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.sp++);
            temp |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }

        break;

    case 0xC9: /* RET */
    {
        uint16_t temp = __gb_read(gb, gb->cpu_reg.sp++);
        temp |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
        gb->cpu_reg.pc = temp;
        break;
    }

    case 0xCA: /* JP Z, imm */
        if(gb->cpu_reg.f_bits.z){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xCB: /* CB INST */
        inst_cycles = __gb_execute_cb(gb);
        break;

    case 0xCC: /* CALL Z, imm */
        if(gb->cpu_reg.f_bits.z){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xCD: /* CALL imm */
    {
        uint16_t addr = __gb_read(gb, gb->cpu_reg.pc++);
        addr |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = addr;
    }
    break;

    case 0xCE: /* ADC A, imm */
    {
        uint8_t value, a, carry;
        value = __gb_read(gb, gb->cpu_reg.pc++);
        a = gb->cpu_reg.a;
        carry = gb->cpu_reg.f_bits.c;
        gb->cpu_reg.a = a + value + carry;

        gb->cpu_reg.f_bits.z = gb->cpu_reg.a == 0 ? 1 : 0;
        gb->cpu_reg.f_bits.h =
            ((a & 0xF) + (value & 0xF) + carry > 0x0F) ? 1 : 0;
        gb->cpu_reg.f_bits.c =
            (((uint16_t) a) + ((uint16_t) value) + carry > 0xFF) ? 1 : 0;
        gb->cpu_reg.f_bits.n = 0;
        break;
    }

    case 0xCF: /* RST 0x0008 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0008 + gb->load_address;
        break;

    case 0xD0: /* RET NC */
        if(!gb->cpu_reg.f_bits.c){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.sp++);
            temp |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }

        break;

    case 0xD1: /* POP DE */
        gb->cpu_reg.e = __gb_read(gb, gb->cpu_reg.sp++);
        gb->cpu_reg.d = __gb_read(gb, gb->cpu_reg.sp++);
        break;

    case 0xD2: /* JP NC, imm */
        if(!gb->cpu_reg.f_bits.c){
            uint16_t temp =  __gb_read(gb, gb->cpu_reg.pc++);
            temp |=  __gb_read(gb, gb->cpu_reg.pc++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xD4: /* CALL NC, imm */
        if(!gb->cpu_reg.f_bits.c){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xD5: /* PUSH DE */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.d);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.e);
        break;

    case 0xD6: /* SUB imm */
    {
        uint8_t val = __gb_read(gb, gb->cpu_reg.pc++);
        uint16_t temp = gb->cpu_reg.a - val;
        gb->cpu_reg.f_bits.z = ((temp & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ val ^ temp) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp & 0xFF);
        break;
    }

    case 0xD7: /* RST 0x0010 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0010 + gb->load_address;
        break;

    case 0xD8: /* RET C */
        if(gb->cpu_reg.f_bits.c){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.sp++);
            temp |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }

        break;

    case 0xD9: /* RETI */
    {
        uint16_t temp = __gb_read(gb, gb->cpu_reg.sp++);
        temp |= __gb_read(gb, gb->cpu_reg.sp++) << 8;
        gb->cpu_reg.pc = temp;
        gb->gb_ime = 1;
    }
    break;

    case 0xDA: /* JP C, imm */
        if(gb->cpu_reg.f_bits.c){
            uint16_t addr = __gb_read(gb, gb->cpu_reg.pc++);
            addr |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            gb->cpu_reg.pc = addr;
            inst_cycles += 4;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xDC: /* CALL C, imm */
        if(gb->cpu_reg.f_bits.c){
            uint16_t temp = __gb_read(gb, gb->cpu_reg.pc++);
            temp |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
            __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
            gb->cpu_reg.pc = temp;
            inst_cycles += 12;
        }
        else
            gb->cpu_reg.pc += 2;

        break;

    case 0xDE: /* SBC A, imm */
    {
        uint8_t temp_8 = __gb_read(gb, gb->cpu_reg.pc++);
        uint16_t temp_16 = gb->cpu_reg.a - temp_8 - gb->cpu_reg.f_bits.c;
        gb->cpu_reg.f_bits.z = ((temp_16 & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h =
            (gb->cpu_reg.a ^ temp_8 ^ temp_16) & 0x10 ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp_16 & 0xFF00) ? 1 : 0;
        gb->cpu_reg.a = (temp_16 & 0xFF);
        break;
    }

    case 0xDF: /* RST 0x0018 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0018 + gb->load_address;
        break;

    case 0xE0: /* LD (0xFF00+imm), A */
        __gb_write(gb, 0xFF00 | __gb_read(gb, gb->cpu_reg.pc++),
               gb->cpu_reg.a);
        break;

    case 0xE1: /* POP HL */
        gb->cpu_reg.l = __gb_read(gb, gb->cpu_reg.sp++);
        gb->cpu_reg.h = __gb_read(gb, gb->cpu_reg.sp++);
        break;

    case 0xE2: /* LD (C), A */
        __gb_write(gb, 0xFF00 | gb->cpu_reg.c, gb->cpu_reg.a);
        break;

    case 0xE5: /* PUSH HL */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.h);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.l);
        break;

    case 0xE6: /* AND imm */
        /* TODO: Optimisation? */
        gb->cpu_reg.a = gb->cpu_reg.a & __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 1;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xE7: /* RST 0x0020 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0020 + gb->load_address;
        break;

    case 0xE8: /* ADD SP, imm */
    {
        int8_t offset = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
        /* TODO: Move flag assignments for optimisation. */
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.sp & 0xF) + (offset & 0xF) > 0xF) ? 1 : 0;
        gb->cpu_reg.f_bits.c = ((gb->cpu_reg.sp & 0xFF) + (offset & 0xFF) > 0xFF);
        gb->cpu_reg.sp += offset;
        break;
    }

    case 0xE9: /* JP HL */
        gb->cpu_reg.pc = gb->cpu_reg.hl;
        break;

    case 0xEA: /* LD (imm), A */
    {
        uint16_t addr = __gb_read(gb, gb->cpu_reg.pc++);
        addr |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
        __gb_write(gb, addr, gb->cpu_reg.a);
        break;
    }

    case 0xEE: /* XOR imm */
        gb->cpu_reg.a = gb->cpu_reg.a ^ __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xEF: /* RST 0x0028 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0028 + gb->load_address;
        break;

    case 0xF0: /* LD A, (0xFF00+imm) */
        gb->cpu_reg.a =
            __gb_read(gb, 0xFF00 | __gb_read(gb, gb->cpu_reg.pc++));
        break;

    case 0xF1: /* POP AF */
    {
        uint8_t temp_8 = __gb_read(gb, gb->cpu_reg.sp++);
        gb->cpu_reg.f_bits.z = (temp_8 >> 7) & 1;
        gb->cpu_reg.f_bits.n = (temp_8 >> 6) & 1;
        gb->cpu_reg.f_bits.h = (temp_8 >> 5) & 1;
        gb->cpu_reg.f_bits.c = (temp_8 >> 4) & 1;
        gb->cpu_reg.a = __gb_read(gb, gb->cpu_reg.sp++);
        break;
    }

    case 0xF2: /* LD A, (C) */
        gb->cpu_reg.a = __gb_read(gb, 0xFF00 | gb->cpu_reg.c);
        break;

    case 0xF3: /* DI */
        gb->gb_ime = 0;
        break;

    case 0xF5: /* PUSH AF */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.a);
        __gb_write(gb, --gb->cpu_reg.sp,
               gb->cpu_reg.f_bits.z << 7 | gb->cpu_reg.f_bits.n << 6 |
               gb->cpu_reg.f_bits.h << 5 | gb->cpu_reg.f_bits.c << 4);
        break;

    case 0xF6: /* OR imm */
        gb->cpu_reg.a = gb->cpu_reg.a | __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.f_bits.z = (gb->cpu_reg.a == 0x00);
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = 0;
        gb->cpu_reg.f_bits.c = 0;
        break;

    case 0xF7: /* RST 0x0030 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0030 + gb->load_address;
        break;

    case 0xF8: /* LD HL, SP+/-imm */
    {
        /* Taken from SameBoy, which is released under MIT Licence. */
        int8_t offset = (int8_t) __gb_read(gb, gb->cpu_reg.pc++);
        gb->cpu_reg.hl = gb->cpu_reg.sp + offset;
        gb->cpu_reg.f_bits.z = 0;
        gb->cpu_reg.f_bits.n = 0;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.sp & 0xF) + (offset & 0xF) > 0xF) ? 1 : 0;
        gb->cpu_reg.f_bits.c = ((gb->cpu_reg.sp & 0xFF) + (offset & 0xFF) > 0xFF) ? 1 : 0;
        break;
    }

    case 0xF9: /* LD SP, HL */
        gb->cpu_reg.sp = gb->cpu_reg.hl;
        break;

    case 0xFA: /* LD A, (imm) */
    {
        uint16_t addr = __gb_read(gb, gb->cpu_reg.pc++);
        addr |= __gb_read(gb, gb->cpu_reg.pc++) << 8;
        gb->cpu_reg.a = __gb_read(gb, addr);
        break;
    }

    case 0xFB: /* EI */
        gb->gb_ime = 1;
        break;

    case 0xFE: /* CP imm */
    {
        uint8_t temp_8 = __gb_read(gb, gb->cpu_reg.pc++);
        uint16_t temp_16 = gb->cpu_reg.a - temp_8;
        gb->cpu_reg.f_bits.z = ((temp_16 & 0xFF) == 0x00);
        gb->cpu_reg.f_bits.n = 1;
        gb->cpu_reg.f_bits.h = ((gb->cpu_reg.a ^ temp_8 ^ temp_16) & 0x10) ? 1 : 0;
        gb->cpu_reg.f_bits.c = (temp_16 & 0xFF00) ? 1 : 0;
        break;
    }

    case 0xFF: /* RST 0x0038 */
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc >> 8);
        __gb_write(gb, --gb->cpu_reg.sp, gb->cpu_reg.pc & 0xFF);
        gb->cpu_reg.pc = 0x0038 + gb->load_address;
        break;
    }

    /* DIV register timing */
    gb->counter.div_count += inst_cycles;

    if(gb->counter.div_count >= DIV_CYCLES){
        gb->gb_reg.DIV++;
        gb->counter.div_count -= DIV_CYCLES;
    }

    /* TIMA register timing */
    /* TODO: Change tac_enable to struct of TAC timer control bits. */
    if(gb->gb_reg.tac_enable){
        static const uint_fast16_t TAC_CYCLES[4] = {1024, 16, 64, 256};

        gb->counter.tima_count += inst_cycles;

        while(gb->counter.tima_count >= TAC_CYCLES[gb->gb_reg.tac_rate]){
            gb->counter.tima_count -= TAC_CYCLES[gb->gb_reg.tac_rate];

            if(++gb->gb_reg.TIMA == 0){
                gb->gb_reg.IF |= TIMER_INTR;
                /* On overflow, set TMA to TIMA. */
                gb->gb_reg.TIMA = gb->gb_reg.TMA;
            }
        }
    }

    /* Audio */
    /*gb->counter.apu_len_count += inst_cycles;
    gb->counter.apu_swp_count += inst_cycles;
    gb->counter.apu_env_count += inst_cycles;

    if(gb->counter.apu_swp_count >= APU_SWP_CYCLES){
        if(gb->audio.ch1SweepCounterI && gb->audio.ch1SweepShift){
            if(--gb->audio.ch1SweepCounter == 0){
                gb->audio.ch1Freq = gb->hram[0x13] + ((gb->hram[0x14] & 7) << 8);
                if(gb->audio.ch1SweepDir){
                    gb->audio.ch1Freq -= gb->audio.ch1Freq >> gb->audio.ch1SweepShift;
                    if(gb->audio.ch1Freq & 0xF800) gb->audio.ch1Freq = 0;
                }else{
                    gb->audio.ch1Freq += gb->audio.ch1Freq >> gb->audio.ch1SweepShift;
                    if(gb->audio.ch1Freq & 0xF800){
                        gb->audio.ch1Freq = 0;
                        gb->audio.ch1EnvCounter = 0;
                        gb->audio.ch1Vol = 0;
                    }
                }
                gb->hram[0x13] = gb->audio.ch1Freq & 0xFF;
                gb->hram[0x14] &= 0xF8;
                gb->hram[0x14] += (gb->audio.ch1Freq >> 8) & 0x07;
                gb->audio.ch1SweepCounter = gb->audio.ch1SweepCounterI;
            }
        }

        gb->counter.apu_swp_count -= APU_SWP_CYCLES;
    }

    if(gb->counter.apu_env_count >= APU_ENV_CYCLES){
        if(gb->audio.ch1EnvCounter){
            if(--gb->audio.ch1EnvCounter == 0){
                if(gb->audio.ch1Vol && !gb->audio.ch1EnvDir){
                    gb->audio.ch1Vol--;
                    gb->audio.ch1EnvCounter = gb->audio.ch1EnvCounterI;
                }else if(gb->audio.ch1Vol < 0x0F && gb->audio.ch1EnvDir){
                    gb->audio.ch1Vol++;
                    gb->audio.ch1EnvCounter = gb->audio.ch1EnvCounterI;
                }
            }
        }
        
        if(gb->audio.ch2EnvCounter){
            if(--gb->audio.ch2EnvCounter == 0){
                if(gb->audio.ch2Vol && !gb->audio.ch2EnvDir){
                    gb->audio.ch2Vol--;
                    gb->audio.ch2EnvCounter = gb->audio.ch2EnvCounterI;
                }else if(gb->audio.ch2Vol < 0x0F && gb->audio.ch2EnvDir){
                    gb->audio.ch2Vol++;
                    gb->audio.ch2EnvCounter = gb->audio.ch2EnvCounterI;
                }
            }
        }
        
        if(gb->audio.ch4EnvCounter){
            if(--gb->audio.ch4EnvCounter == 0){
                if(gb->audio.ch4Vol && !gb->audio.ch4EnvDir){
                    gb->audio.ch4Vol--;
                    gb->audio.ch4EnvCounter = gb->audio.ch4EnvCounterI;
                }else if(gb->audio.ch4Vol < 0x0F && gb->audio.ch4EnvDir){
                    gb->audio.ch4Vol++;
                    gb->audio.ch4EnvCounter = gb->audio.ch4EnvCounterI;
                }
            }
        }

        gb->counter.apu_env_count -= APU_ENV_CYCLES;
    }

    if(gb->counter.apu_len_count >= APU_LEN_CYCLES){
        if(gb->audio.ch1Len){
            if(--gb->audio.ch1Len == 0 && gb->audio.ch1LenOn){
                gb->hram[0x26] &= 0xFE;
            }
        }
        
        if(gb->audio.ch2Len){
            if(--gb->audio.ch2Len == 0 && gb->audio.ch2LenOn){
                gb->hram[0x26] &= 0xFD;
            }
        }
        
        if(gb->audio.ch3Len){
            if(--gb->audio.ch3Len == 0 && gb->audio.ch3LenOn){
                gb->hram[0x26] &= 0xFB;
            }
        }
        
        if(gb->audio.ch4Len){
            if(--gb->audio.ch4Len == 0 && gb->audio.ch4LenOn){
                gb->hram[0x26] &= 0xF7;
            }
        }
        
        gb->counter.apu_len_count -= APU_LEN_CYCLES;
    }*/

    /* TODO Check behaviour of LCD during LCD power off state. */
    /* If LCD is off, don't update LCD state. */
    if((gb->gb_reg.LCDC & LCDC_ENABLE) == 0)
        return;

    /* LCD Timing */
    gb->counter.lcd_count += inst_cycles;

    /* New Scanline */
    if(gb->counter.lcd_count > LCD_LINE_CYCLES){
        gb->counter.lcd_count -= LCD_LINE_CYCLES;

        /* LYC Update */
        if(gb->gb_reg.LY == gb->gb_reg.LYC){
            gb->gb_reg.STAT |= STAT_LYC_COINC;

            if(gb->gb_reg.STAT & STAT_LYC_INTR)
                gb->gb_reg.IF |= LCDC_INTR;
        }
        else
            gb->gb_reg.STAT &= 0xFB;

        /* Next line */
        gb->gb_reg.LY = (gb->gb_reg.LY + 1) % LCD_VERT_LINES;

        /* VBLANK Start */
        if(gb->gb_reg.LY == LCD_HEIGHT){
            gb->lcd_mode = LCD_VBLANK;
            gb->gb_frame = 1;
            gb->gb_reg.IF |= VBLANK_INTR;

            if(gb->gb_reg.STAT & STAT_MODE_1_INTR)
                gb->gb_reg.IF |= LCDC_INTR;
        }
        /* Normal Line */
        else if(gb->gb_reg.LY < LCD_HEIGHT){

            gb->lcd_mode = LCD_HBLANK;

            if(gb->gb_reg.STAT & STAT_MODE_0_INTR)
                gb->gb_reg.IF |= LCDC_INTR;
        }
    }
    /* OAM access */
    else if(gb->lcd_mode == LCD_HBLANK
            && gb->counter.lcd_count >= LCD_MODE_2_CYCLES){
        gb->lcd_mode = LCD_SEARCH_OAM;

        if(gb->gb_reg.STAT & STAT_MODE_2_INTR)
            gb->gb_reg.IF |= LCDC_INTR;
    }
    /* Update LCD */
    else if(gb->lcd_mode == LCD_SEARCH_OAM
            && gb->counter.lcd_count >= LCD_MODE_3_CYCLES){
        gb->lcd_mode = LCD_TRANSFER;
    }
}

void gb_run_frame(struct gb_s *gb){
    gb->gb_frame = 0;
    while(!gb->gb_frame)
        __gb_step_cpu(gb);
}


/**
 * Resets the context, and initialises startup values.
 */
void gb_init(struct gb_s *gb, uint8_t song){
    gb->gb_halt = 0;
    gb->gb_ime = 0;
    gb->lcd_mode = LCD_HBLANK;

    gb->selected_rom_bank = 1;
    gb->cart_ram_bank = 0;
    gb->cart_ram_bank_offset = CART_RAM_ADDR;
    gb->enable_cart_ram = 0;
    gb->cart_mode_select = 0;

    /* Initialise CPU registers as though a DMG. */
    gb->cpu_reg.sp = gb->stack_pointer;
    __gb_write(gb, gb->stack_pointer, 0x00);
    __gb_write(gb, gb->stack_pointer + 1, 0x00);
    gb->cpu_reg.pc = gb->init_address;

    gb->counter.lcd_count = 0;
    gb->counter.div_count = 0;
    gb->counter.tima_count = 0;

    gb->counter.apu_len_count = APU_LEN_CYCLES;
    gb->counter.apu_swp_count = APU_SWP_CYCLES - 16384;
    gb->counter.apu_env_count = APU_ENV_CYCLES - 57344;

    gb->gb_reg.TIMA      = 0x00;
    gb->gb_reg.TMA       = gb->timer_modulo;
    gb->gb_reg.TAC       = gb->timer_control; 
    gb->gb_reg.DIV       = 0xAB;

    gb->gb_reg.IF        = 0xE1;

    gb->gb_reg.LCDC      = 0x91;
    gb->gb_reg.SCY       = 0x00;
    gb->gb_reg.SCX       = 0x00;
    gb->gb_reg.LYC       = 0x00;

    gb->gb_reg.SB = 0x00;
    gb->gb_reg.SC = 0x7E;
    gb->gb_reg.STAT = 0x85;
    gb->gb_reg.LY = 0x00;

    __gb_write(gb, 0xFF10, 0x80);
    __gb_write(gb, 0xFF11, 0xBF);
    __gb_write(gb, 0xFF12, 0xF3);
    __gb_write(gb, 0xFF13, 0xFF);
    __gb_write(gb, 0xFF14, 0xBF);
    __gb_write(gb, 0xFF15, 0xFF);
    __gb_write(gb, 0xFF16, 0x3F);
    __gb_write(gb, 0xFF17, 0x00);
    __gb_write(gb, 0xFF18, 0xFF);
    __gb_write(gb, 0xFF19, 0xBF);
    __gb_write(gb, 0xFF1A, 0x7F);
    __gb_write(gb, 0xFF1B, 0xFF);
    __gb_write(gb, 0xFF1C, 0x9F);
    __gb_write(gb, 0xFF1D, 0xFF);
    __gb_write(gb, 0xFF1E, 0xBF);
    __gb_write(gb, 0xFF1F, 0xFF);
    __gb_write(gb, 0xFF20, 0xFF);
    __gb_write(gb, 0xFF21, 0x00);
    __gb_write(gb, 0xFF22, 0x00);
    __gb_write(gb, 0xFF23, 0xBF);
    __gb_write(gb, 0xFF24, 0x77);
    __gb_write(gb, 0xFF25, 0xF3);
    __gb_write(gb, 0xFF26, 0xF1);
    __gb_write(gb, 0xFF27, 0xFF);
    __gb_write(gb, 0xFF28, 0xFF);
    __gb_write(gb, 0xFF29, 0xFF);
    __gb_write(gb, 0xFF2A, 0xFF);
    __gb_write(gb, 0xFF2B, 0xFF);
    __gb_write(gb, 0xFF2C, 0xFF);
    __gb_write(gb, 0xFF2D, 0xFF);
    __gb_write(gb, 0xFF2E, 0xFF);
    __gb_write(gb, 0xFF2F, 0xFF);

    for(int i = 0; i < 0x20; i++) gb->audio.WAVRAM[i] = 0;
    gb->audio.ch1Freq = 0;
    gb->audio.ch1SweepCounter = 0;
    gb->audio.ch1SweepCounterI = 0;
    gb->audio.ch1SweepDir = 0;
    gb->audio.ch1SweepShift = 0;
    gb->audio.ch1Vol = gb->audio.ch2Vol = gb->audio.ch3Vol = gb->audio.ch4Vol = 0;
    gb->audio.ch1VolI = gb->audio.ch2VolI = gb->audio.ch3VolI = gb->audio.ch4VolI = 0;
    gb->audio.ch1Len = gb->audio.ch2Len = gb->audio.ch3Len = gb->audio.ch4Len = 0;
    gb->audio.ch1LenI = gb->audio.ch2LenI = gb->audio.ch3LenI = gb->audio.ch4LenI = 0;
    gb->audio.ch1LenOn = gb->audio.ch2LenOn = gb->audio.ch3LenOn = gb->audio.ch4LenOn = 0;
    gb->audio.ch1EnvCounter = gb->audio.ch2EnvCounter = gb->audio.ch4EnvCounter = 0;
    gb->audio.ch1EnvCounterI = gb->audio.ch2EnvCounterI = gb->audio.ch4EnvCounterI = 0;
    gb->audio.ch1EnvDir = gb->audio.ch2EnvDir = gb->audio.ch4EnvDir = 0;
    gb->audio.ch1DAC = gb->audio.ch2DAC = gb->audio.ch4DAC = 0;
    gb->audio.idleTimer = 0;

    if(gb->timer_control & 2){
        gb->gb_reg.IE = TIMER_INTR;
    }else{
        gb->gb_reg.IE = VBLANK_INTR;
    }
    gb->cpu_reg.a = song;
    gb->cpu_reg.f = 0xB0;
    gb->cpu_reg.b = 0x00;
    gb->cpu_reg.c = 0x13;
    gb->cpu_reg.d = 0x00;
    gb->cpu_reg.e = 0xD8;
    gb->cpu_reg.h = 0x01;
    gb->cpu_reg.l = 0x4D;
}
