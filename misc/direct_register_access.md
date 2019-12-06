https://arduino.stackexchange.com/questions/56304/how-do-i-directly-access-a-memory-mapped-register-of-avr-with-c

It's worth exploring how PORTD is defined in the actual AVR header file. The relevant file is iom328p.h, and it's defined as:
#define PORTD   _SFR_IO8(0x0B)
_SFR_IO8 is defined as
#define _SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + __SFR_OFFSET)
and
#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
This shows that 0x0b is not the address of the PORTD register, but its offset from the address __SFR_OFFSET, which is 0x20. Indeed, if you look at the ATMEGA328 datasheet, the register summary shows that PORTD is at offset 0x2b. So that is the address that you would want to access via a pointer.
Note that this also shows the type of the pointer, if we fully expand the original definition of PORTD we get:
#define PORTD   _SFR_IO8(0x0B)
#define PORTD _MMIO_BYTE((0x0B) + __SFR_OFFSET)
#define PORTD _MMIO_BYTE((0x0B) + 0x20)
#define PORTD (*(volatile uint8_t *)(0x2B))
This syntax takes the integer 0x2B, casts it to type pointer to volatile uint8_t, and dereferences that pointer so you can assign to or read from the location the pointer references. The volatile qualifier is important because it prevents the compiler from optimizing out accesses to memory. Without it, the compiler may store the value to a register instead, or not store it at all, which would mean your IO port would remain unchanged or appear to report an incorrect value.
To go back to the title of your question, you have asked how to "directly access a memory mapped register in C", well the answer is you do that exactly the way the header file defines for you: 
(*(volatile uint8_t *)(0x2B) = 0xFF
aka
PORTD = 0xff


https://garretlab.web.fc2.com/en/arduino/inside/hardware/tools/avr/avr/include/avr/sfr_defs.h/_SFR_IO8.html
