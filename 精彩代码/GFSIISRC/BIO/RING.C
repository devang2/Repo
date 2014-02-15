/**************************************************************
	ring.c

	This is the source file of functions that are used to simulate
	the telephone ring.
	Note:  	When compiling and linking, both 'Stack warning' and 'Use
			register variables' should be turned off.
	Compileing Env.	:	Turbo C 2.0
 **************************************************************/

#include <dos.h>
#include <def.inc>

void ring_on(void);
void ring_off(void);
static void interrupt new_sys_clock();

static void interrupt (* old_vector)();
static int ringing;          /* TRUE if ringing, FALSE if not ringing. */

static int count;
static int ring_count, ring_count1;
static int ring_rep;

/* -------- ring parameters -------- */
static int ring_f1	= 610;
static int ring_t1	= 15;
static int ring_f2	= 820;
static int ring_t2	= 35;
static int ring_len	= 8;
static int ring_inv	= 800;

/* ------------------------------------------------------------
	Name		ring_on
	Prototype	void ring_on(void)
	Function	This function is used to turn on the simulated telephone
				ring.
	Parameters:
		None.
	Return value:
		None.
   ------------------------------------------------------------ */

void ring_on(void)
{
	if (!ringing) {          /* check if it is already ringing */
		count = 0;
		ring_count = 0;
		ring_count1 = 0;
		ring_rep = 4;
		old_vector = getvect(8);
		disable();
		outportb(0x43, 0xb6);
		outportb(0x40, 0x00);
		outportb(0x40, 0x10);
		setvect(8, new_sys_clock);
		enable();
		ringing = TRUE;
	}
}

/* ------------------------------------------------------------
	Name		ring_off
	Prototype	void ring_off(void)
	Function	This function is used to turn off the simulated telephone
				ring.
	Parameters:
		None.
	Return value:
		None.
   ------------------------------------------------------------ */

void ring_off(void)
{
	if (ringing) {       /* check if it is still ringing */
		disable();
		outportb(0x43, 0xb6);
		outportb(0x40, 0x00);
		outportb(0x40, 0x00);
		setvect(8, old_vector);
		enable();
		ringing = FALSE;
	}
}

/* ------------------------------------------------------------
	Name    	:  new_sys_clock
	Prototype	:  void interrupt new_sys_clock(void)
	Function	:
			This is the new system clock interrupt processing routine.
		When the ring is on, the system clock is set to be faster than
		usual.
	Parameters	:
		None.
	Return Value:
		None.
   ------------------------------------------------------------ */

static void interrupt new_sys_clock(void)
{
	disable();           /* disable interrupts */

	if (ring_count1 < ring_rep) {
		if (ring_count == 0) sound(ring_f1);
		else if (ring_count == ring_t1) sound(ring_f2);
		else if (ring_count == ring_t2) {
			ring_count = -1;
			ring_count1 ++;
			if (ring_count1 == ring_rep) nosound();
		}
	}
	else {
		if (ring_count == ring_inv) {
			ring_count = 0;
			ring_count1 = 0;
			ring_rep = ring_len;
		}
	}
	ring_count ++;
	count ++;
	if (count == 16) {
		count = 0;
		old_vector();
	}
	else {
		enable();
		outportb(0x20, 0x20);
	}
}