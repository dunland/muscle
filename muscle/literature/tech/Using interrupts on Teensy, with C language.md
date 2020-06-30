# Using interrupts on Teensy, with C language

from [https://www.pjrc.com/teensy/interrupts.html](https://www.pjrc.com/teensy/interrupts.html)

The flag bit is set even if interrupts are not used.  Software
polling can read the flag bit to check if the condition has
occured, do whatever is necessary, and then reset the flag.

  
Interrupts are a hardware feature that allows a special piece of code, called an "interrupt service routine" to be called when a physical condition occurs. Many interrupts are available for conditions such as pins changing, data received, timers overflowing.

Polling Versus Interrupts
-------------------------

Interrupts are complex to use. Often simply checking for the condition periodically (called "polling") is a much simpler solution, if not elegant.

Interrupt Basics
----------------

Every interrupt has a flag bit, which is set by hardware when the interrupt trigger condition occurs. The flag's purpose is to remember the interrupt condition has occurred until it has been handled by software. An interrupt is said to be "pending" if the trigger condition has set the flag but the interrupt service routine has not been called yet, which can happen if the main program has disabled interrupts or another interrupt service routine is running.

The flag bit is set even if interrupts are not used. Software polling can read the flag bit to check if the condition has occured, do whatever is necessary, and then reset the flag.

Most interrupt flags are automatically reset when the interrupt service routine is called. Some flags must be reset by the software inside the interrupt service routine. Some flags are completely controlled by the peripheral to reflect internal state (such as USART receive) and can only be changed indirectly by manipulating the peripheral.

Every interrupt also has a mask bit, which enable or disables that individual interrupt. These mask bits allow you to control which of the many interrupts are enabled. See below for a [complete list of mask and flag names](#names).

There is also a global interrupt enable bit, which allows you to disable all interrupts and enable all interrupts that have their mask bits set. When the global interrupt enable is set, and individual masks are enabled, and interrupt flags are set, the corresponding interrupt vector is called for each flag.

Interrupt Configuration Steps
-----------------------------

All interrupts are used in roughly the same way.

1.  Configure The Peripheral
2.  Reset Interrupt Flag
3.  Set Interrupt Mask
4.  Enable Global Interrupt, with sei()

When the interrupt condition occurs, the interrupt flag is set. The interrupt service routine will be called at the first opportunity.

Interrupt Service Routine Syntax
--------------------------------

To use interrupts, you must include the AVR interrupt header.

``` C++
#include <avr/io.h>
#include <avr/interrupt.h>
```

This will define the ISR() macro and names for each of the possible interrupt routines. A [complete list of names](#names) appears below. For example, to create an interupt service routine to handle Timer 0 overflow:

``` C++
ISR(TIMER0\_OVF\_vect)
{
    /\* Timer 0 overflow \*/
}
```

Because the interrupt service routine is called directly by the hardware, rather than your own code, it must use the special name for the desired interrupt, so that the compiler may properly associate it with the interrupt vector.

Interrupt service routines must be careful to restore the machine state (except for intentionally shared variables) to exactly what it was, so that the main program does not see random changes. The ISR() macro instructs the compiler to handle all these details.

Interrupt Design Strategy
-------------------------

The simplest and most common strategy is to keep all interrupt service routines short and simple, so they execute quickly, and to minimize time the main program disables interrupts. Virtually all examples follow this model.

When the hardware calls an interrupt service routine, it clears the global interrupt flag, so that no other interrupt routine may be called. The return from an interrupt service routine automatically reenables interrupts, and if any other interrupt flags are set, the hardware will call the next pending interrupt routine rather than returning to the main program.

A less common strategy is called "nested interrupts", where the some interrupt service routines enable the global interrupt with sei(). Usually this is done when an interrupt routine may take a very long time to execute, and some other interrupt routine is considered very urgent and can not be blocked from running. Great caution is needed to make sure the already-in-service interrupt can not trigger again, leading to the hardware calling the interrupt routine over and over until the entire memory is overwritten!

Generally, it is safest to never use sei() within any interrupt service routine.

Shared Variables
----------------

All but the simplest interrupt service routines need to share data with the main program. Special techniques are needed to share variables.

Shared variables must be declared with the "volatile" keyword, which instructs the compiler to always access the variable. Without volatile, the compiler may apply optimizations which assume the variable can not change on its own.

``` C++
volatile unsigned int overflow\_count=0;

ISR(TIMER0\_OVF\_vect)
{
    if (overflow\_count < 0xFFFF) overflow\_count++;
}
``` 

In this example, a 16 bit number counts the number of times Timer 0 has overflowed, which can be useful for measuring elapsed time.

When accessing shared varaiables from the main program, steps need to be taken to prevent wrong results if the interrupt is triggered in the middle of an operation. The simplest and most common approach is to simply disable the global interrupt setting with cli() and reenable with with sei(). For example:

``` C++
void begin\_timeout(void)
{
	cli();
	overflow\_count = 0;
	sei();
}

char is\_timeout(void)
{
	unsigned int count\_copy;

	cli();
	count\_copy = overflow\_count;
	sei();
	if (count\_copy > 5600) return 1;
	return 0;
}
```

Even a simple operation like setting the variable to zero needs to be protected with cli() and sei(), because the compiler will need to make 2 writes and the interrupt could trigger between them. Complex operations, such as adding or removing data from a buffer and adjusting pointers need to be protected from start to finish.

With volatile variables, making a local copy is often a good idea. The compiler optimizations on the local copy usually outweigh the overhead of a copy.

The above code assumes interrupts are enabled, and the sei() instruction reenables them. This is the simplest and most common case. However, code can be written to backup the interrupt enable state and restore it.

``` C++
void begin\_timeout(void)
{
	unsigned char sreg\_backup;

	sreg\_backup = SREG;   /\* save interrupt enable/disable state \*/
        cli();
        overflow\_count = 0;
	SREG = sreg\_backup    /\* restore interrupt state \*/
}
```

Many examples you will find use this more cautious approach, so they will be compatible when you use them in your program.

You can also disable the individual mask bit for your interrupt, rather than using cli(). This has the advantage of not blocking other unrelated interrupts from running. However, most people simply use cli() and sei(), perhaps because the compiler can implement them more efficiently, or maybe because they are easier to remember and shorter to type!

Interrupt Vector, Mask & Flag Names
-----------------------------------

**ISR() Name**: The name used with ISR() to define the interrupt service routine.

**Mask**: (byte,bit#) Bit that enables this interrupt. See [accessing a single bit](moz-extension://aed13e24-8afb-496d-9580-ddd851bddc28/pins.html#single_bit) for C syntax to write byte,bit# pairs.

**Flag**: (byte,bit#) Flag indicates if the interrupt is pending. Many flags are reset by writing 1 (yes, that seems horribly backwards, but that's the way the hardware works). Most flags are automatically reset when the interrupt service routine is called.

| ISR() Name | Mask | Flag | Function |
| --- | --- | --- | --- |
| INT0\_vect | EIMSK,IINT0 | EIFR,INTF0 | Interrupt Request, External Signal |
| INT1\_vect | EIMSK,IINT1 | EIFR,INTF1 |
| INT2\_vect | EIMSK,IINT2 | EIFR,INTF2 |
| INT3\_vect | EIMSK,IINT3 | EIFR,INTF3 |
| INT4\_vect | EIMSK,IINT4 | EIFR,INTF4 |
| INT5\_vect | EIMSK,IINT5 | EIFR,INTF5 |
| INT6\_vect | EIMSK,IINT6 | EIFR,INTF6 |
| INT7\_vect | EIMSK,IINT7 | EIFR,INTF7 |
| PCINT0\_vect | PCICR,PCIE0 | PCIFR,PCIF0 | Pin Change |
| PCINT1\_vect | PCICR,PCIE1 | PCIFR,PCIF1 |
| TIMER0\_COMPA\_vect | TIMSK0,OCIE0A | TIFR0,OCF0A | Timer 0 Compare A Match |
| TIMER0\_COMPB\_vect | TIMSK0,OCIE0B | TIFR0,OCF0B | Timer 0 Compare B Match |
| TIMER0\_OVF\_vect | TIMSK0,TOIE0 | TIFR0,TOV0 | Timer 0 Overflow |
| TIMER1\_CAPT\_vect | TIMSK1,ICIE1 | TIFR1,ICF1 | Timer 1 Input Capture |
| TIMER1\_COMPA\_vect | TIMSK1,OCIE1A | TIFR1,OCF1A | Timer 1 Compare A Match |
| TIMER1\_COMPB\_vect | TIMSK1,OCIE1B | TIFR1,OCF1B | Timer 1 Compare B Match |
| TIMER1\_COMPC\_vect | TIMSK1,OCIE1C | TIFR1,OCF1C | Timer 1 Compare C Match |
| TIMER1\_OVF\_vect | TIMSK1,TOIE1 | TIFR1,TOV1 | Timer 1 Overflow |
| WDT\_vect | WDTCSR,WDIE | WDTCSR,WDIF | Watchdog Timer |
| USART1\_RX\_vect | UCSR1B,RXCIE1 | UCSR1A,RXC1 | USART Receive |
| USART1\_TX\_vect | UCSR1B,TXCIE1 | UCSR1A,TXC1 | USART Transmit Complete |
| USART1\_UDRE\_vect | UCSR1B,UDRIE1 | UCSR1A,UDRE1 | USART Transmit Ready |
| SPI\_STC\_vect | SPCR,SPIE | SPSR,SPIF | SPI Transfer Complete |
| ANALOG\_COMP\_vect | ACSR,ACIE | ACSR,ACI | Analog Comparison Change |
| EE\_READY\_vect | EECR,EERIE | (none) | EEPROM Operation Complete |
| USB\_GEN\_vect | (complex) | (complex) | USB Device Event |
| USB\_COM\_vect | (very complex) | (very complex) | USB Communication |


[Source](https://www.pjrc.com/teensy/interrupts.html)
