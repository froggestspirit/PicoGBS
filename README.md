Raspberry Pi Pico GBS Player

By FroggestSpirit

This is a POC GBS player, based on a PWM sound example by Robin Grosset, and a heavily modified paenut-gb emulator by Mahyar Koshkouei

By using this, I am not liable to any damage to hardware caused by this


GBS file will need to be converted to a header file named "gbs.h" if using bash, you can run "convertGBS.sh" and it *should* create a gbs.h file from a gbs.gbs in the same folder

To build, the raspberry pi C/C++ SDK needs to be installed. In the gbs player folder:

mkdir build && cd build && cmake ..


Not everything works right now, and is subject to improvements over time. I may be looking into loading files from an SD, or a small display


Features:
- Play GBS files in stereo, through pins 27 and 28
- Tracks play for a default of 90 seconds (can be changed in gbs_player.c)
- Tracks that do not loop, and end, attempt to detect this, and start the next song after 4 seconds

Known Bugs:
- Many GBS files are not working
- Audio emulation is not perfect (yet)

Tested GBS Files (Working):
- Pokemon Crystal
- Pokemon TCG
- Legend of Zelda: Link's Awakening DX
- Kirby's Dream Land 2

Tested GBS Files (Not Working):
- Mole Mania
- Legend of Zelda: Oracle of Ages
