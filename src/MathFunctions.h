#ifndef MathFunctions_h
#define MathFunctions_h

/*
 Based on Pichenettes pseudocode https://gist.github.com/pichenettes/7453b3d6e3a0e252124f
 See https://mutable-instruments.net/forum/discussion/6005/the-dsp-g1-analog-modeling-synth-source-code/p1
 */
const uint16_t coarse_table[] = { 33, 50, 78, 120, 184, 284, 437, 673, 1036, 1596, 2457, 3784, 5827, 8973, 13818, 21279 };
const uint16_t fine_table[] = { 32768, 33664, 34585, 35531, 36503, 37501, 38527, 39580, 40663, 41775, 42918, 44092, 45298, 46536, 47809, 49117, 50460 };

// Feed in a uint16_t from 0 to 65536 and receive a value between 32 and 32768 with a log response
uint16_t log_map(uint16_t value) {
 uint16_t coarse = coarse_table[(value >> 12) & 0xf];
 uint16_t fine_integral = (value >> 8) & 0xf;
 uint16_t fine_fractional = value & 0xff;
 uint16_t fine = fine_table[fine_integral] + ((fine_table[fine_integral + 1] - fine_table[fine_integral]) * fine_fractional >> 8);
 return coarse * fine >> 15;
}

#endif