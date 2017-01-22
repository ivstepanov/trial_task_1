/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution and was contributed
    to the project by Technolution B.V. (www.technolution.nl,
    freertos-riscv@technolution.eu) under the terms of the FreeRTOS
    contributors license.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "configstring.h"


/* A variable is used to keep track of the critical section nesting.  This
variable has to be stored as part of the task context and must be initialised to
a non zero value to ensure interrupts don't inadvertently become unmasked before
the scheduler starts.  As it is stored as part of the task context it will
automatically be set to 0 when the first task is started. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/* Contains context when starting scheduler, save all 31 registers */
#ifdef __gracefulExit
BaseType_t xStartContext[31] = {0};
#endif

volatile uint64_t* mtime;
volatile uint64_t* timecmp;


void parse_config_string(void);
static void query_rtc(const char* config_string);


/*
 * Handler for timer interrupt
 */
void vPortSysTickHandler( void );

/*
 * Setup the timer to generate the tick interrupts.
 */
void vPortSetupTimer( void );

/*
 * Set the next interval for the timer
 */
static void prvSetNextTimerInterrupt( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* Sets the next timer interrupt
 * Reads previous timer compare register, and adds tickrate */
static void prvSetNextTimerInterrupt(void)
{
	*timecmp = *mtime + configTICK_CLOCK_HZ / configTICK_RATE_HZ;
	clear_csr(mip, MIP_STIP);
	set_csr(mie, MIP_MTIP);
}

/*-----------------------------------------------------------*/

/* Sets and enable the timer interrupt */
void vPortSetupTimer(void)
{
	parse_config_string();
	*timecmp = *mtime + configTICK_CLOCK_HZ / configTICK_RATE_HZ;
	/* Enable timer interupt */
	__asm volatile("csrs mie,%0"::"r"(0x80));
}
/*-----------------------------------------------------------*/

void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Clear current interrupt mask and set given mask */
void vPortClearInterruptMask(int mask)
{
	__asm volatile("csrw mie, %0"::"r"(mask));
}
/*-----------------------------------------------------------*/

/* Set interrupt mask and return current interrupt enable register */
int vPortSetInterruptMask(void)
{
	int ret;
	__asm volatile("csrr %0,mie":"=r"(ret));
	__asm volatile("csrc mie,%0"::"i"(7));
	return ret;
}
/*-----------------------------------------------------------*/


/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	int i;
	register int *tp asm("x3");
	//*pxTopOfStack = 0xdeadbeef;
	//pxTopOfStack--;
#if 0
	*pxTopOfStack = (portSTACK_TYPE)pxCode;			/* Start address */
	pxTopOfStack--;
	*pxTopOfStack = 0x02;
	pxTopOfStack--;
	*pxTopOfStack = 0x03;
	//pxTopOfStack -= 20;
	for (i = 0; i < 20; i++) {
		*pxTopOfStack-- = i + 4;
	}

	*pxTopOfStack = (portSTACK_TYPE)pvParameters;	/* Register a0 */
	pxTopOfStack--;
	for (i = 0; i < 5; i++) {
			*pxTopOfStack-- = i + 24;
	}
	//pxTopOfStack -= 6;
	*pxTopOfStack = (portSTACK_TYPE)tp; /* Register thread pointer */
	pxTopOfStack -= 3;
	*pxTopOfStack = (portSTACK_TYPE)pxCode; //prvTaskExitError; /* Register ra */
#else
	i = 1;
	pxTopOfStack -= 32;
	pxTopOfStack[0] = (portSTACK_TYPE)pxCode;         //x1
	pxTopOfStack[1] = (portSTACK_TYPE)i++;            //x2
	pxTopOfStack[2] = (portSTACK_TYPE)tp; //i++;      //x3
	pxTopOfStack[3] = (portSTACK_TYPE)0;             //x4
	pxTopOfStack[4] = (portSTACK_TYPE)i++;            //x5
	pxTopOfStack[5] = (portSTACK_TYPE)i++;            //x6
	pxTopOfStack[6] = (portSTACK_TYPE)i++;            //x7
	pxTopOfStack[7] = (portSTACK_TYPE)i++;            //x8
	pxTopOfStack[8] = (portSTACK_TYPE)i++;            //x9
	pxTopOfStack[9] = (portSTACK_TYPE)pvParameters;  //x10
	pxTopOfStack[10] = (portSTACK_TYPE)i++;
	pxTopOfStack[11] = (portSTACK_TYPE)i++;
	pxTopOfStack[12] = (portSTACK_TYPE)i++;
	pxTopOfStack[13] = (portSTACK_TYPE)i++;
	pxTopOfStack[14] = (portSTACK_TYPE)i++;
	pxTopOfStack[15] = (portSTACK_TYPE)i++;
	pxTopOfStack[16] = (portSTACK_TYPE)i++;
	pxTopOfStack[17] = (portSTACK_TYPE)i++;
	pxTopOfStack[18] = (portSTACK_TYPE)i++;
	pxTopOfStack[19] = (portSTACK_TYPE)i++;
	pxTopOfStack[20] = (portSTACK_TYPE)i++;
	pxTopOfStack[21] = (portSTACK_TYPE)i++;
	pxTopOfStack[22] = (portSTACK_TYPE)i++;
	pxTopOfStack[23] = (portSTACK_TYPE)i++;
	pxTopOfStack[24] = (portSTACK_TYPE)i++;
	pxTopOfStack[25] = (portSTACK_TYPE)i++;
	pxTopOfStack[26] = (portSTACK_TYPE)i++;
	pxTopOfStack[27] = (portSTACK_TYPE)i++;
	pxTopOfStack[28] = (portSTACK_TYPE)i++;
	pxTopOfStack[29] = (portSTACK_TYPE)i++;
	pxTopOfStack[30] = (portSTACK_TYPE)i++;
	pxTopOfStack[31] = (portSTACK_TYPE)pxCode;

#endif

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortSysTickHandler( void )
{
	prvSetNextTimerInterrupt();

	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}
}
/*-----------------------------------------------------------*/

static void query_rtc(const char* config_string)
{
  query_result res = query_config_string(config_string, "rtc{addr");
  assert(res.start);
  mtime = (void*)(uintptr_t)get_uint(res);
}

static void query_timecmp(const char* config_string)
{
    query_result res = query_config_string(config_string, "core{0{0{timecmp");
    assert(res.start);
    timecmp = (void*)(uintptr_t)get_uint(res);
}

void parse_config_string()
{
  uint32_t addr = *(uint32_t*)CONFIG_STRING_ADDR;
  const char* s = (const char*)(uintptr_t)addr;
#if 0
  printf("Config: %s\n", s);
#endif
  query_rtc(s);
  query_timecmp(s);
}
