#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) 
{
	const uint32_t key = inl(KBD_ADDR);
	kbd->keydown = key & KEYDOWN_MASK;
	kbd->keycode = key & KEYDOWN_MASK ? key ^ KEYDOWN_MASK : key;
}
