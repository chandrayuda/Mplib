/**
 *******************************************************************************
 * @file    system_TMPM361.c
 * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer Source File for the
 *          TOSHIBA 'TMPM361' Device Series 
 * @version V2.2.0
 * @date    2010/06/23
 * 
 * THE SOURCE CODE AND ITS RELATED DOCUMENTATION IS PROVIDED "AS IS". TOSHIBA
 * CORPORATION MAKES NO OTHER WARRANTY OF ANY KIND, WHETHER EXPRESS, IMPLIED OR,
 * STATUTORY AND DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
 * SATISFACTORY QUALITY, NON INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * THE SOURCE CODE AND DOCUMENTATION MAY INCLUDE ERRORS. TOSHIBA CORPORATION
 * RESERVES THE RIGHT TO INCORPORATE MODIFICATIONS TO THE SOURCE CODE IN LATER
 * REVISIONS OF IT, AND TO MAKE IMPROVEMENTS OR CHANGES IN THE DOCUMENTATION OR
 * THE PRODUCTS OR TECHNOLOGIES DESCRIBED THEREIN AT ANY TIME.
 * 
 * TOSHIBA CORPORATION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGE OR LIABILITY ARISING FROM YOUR USE OF THE SOURCE CODE OR
 * ANY DOCUMENTATION, INCLUDING BUT NOT LIMITED TO, LOST REVENUES, DATA OR
 * PROFITS, DAMAGES OF ANY SPECIAL, INCIDENTAL OR CONSEQUENTIAL NATURE, PUNITIVE
 * DAMAGES, LOSS OF PROPERTY OR LOSS OF PROFITS ARISING OUT OF OR IN CONNECTION
 * WITH THIS AGREEMENT, OR BEING UNUSABLE, EVEN IF ADVISED OF THE POSSIBILITY OR
 * PROBABILITY OF SUCH DAMAGES AND WHETHER A CLAIM FOR SUCH DAMAGE IS BASED UPON
 * WARRANTY, CONTRACT, TORT, NEGLIGENCE OR OTHERWISE.
 * 
 * (C)Copyright TOSHIBA CORPORATION 2010 All rights reserved
 *******************************************************************************
 */

#include <stdint.h>
#include "TMPM361.h"

/*-------- <<< Start of configuration section >>> ----------------------------*/

/* Watchdog Timer (WD) Configuration */
#define WD_SETUP    1
#define WDMOD_Val   (0x00000000UL)
#define WDCR_Val    (0x000000B1UL)

/* Clock Generator (CG) Configuration */
#define CLOCK_SETUP     1
#define SYSCR_Val       (0x00010000UL)
#define OSCCR_Ready     (0x03200330UL) /* Warm-up time[OSCCR<WUPT13:2>] = 0x32(100us @ XTALH = 8MHz) */
#define OSCCR_Val       (0x06400334UL) /* Warm-up time[OSCCR<WUPT13:2>] = 0x64(200us @ XTALH = 8MHz) */
#define STBYCR_Val      (0x00000103UL)
#define PLLSEL_Ready    (0x0000721EUL)
#define PLLSEL_Val      (0x0000721FUL)
#define CKSEL_Val       (0x00000000UL)


/*-------- <<< End of configuration section >>> ------------------------------*/

/*-------- DEFINES -----------------------------------------------------------*/
/* Define clocks */
#define XTALH (8000000UL)  /* External high-speed oscillator freq */
#define XTALL (  32768UL)  /* External low-speed oscillator freq  */

/* Determine core clock frequency according to settings */
#if (CKSEL_Val & (1U << 1))            /* If system clock is low-speed clock */
  #define __CORE_CLK (XTALL)
#else                                 /* If system clock is high-speed clock*/
  #if ((PLLSEL_Val & (1U<<0)) && (OSCCR_Val & (1U<<2))) /* If PLL selected and enabled */
    #if   ((PLLSEL_Val & 0x0000FFF8) == 0x0000A138UL) /* If PLL set as 8 times */
      #if   ((SYSCR_Val & 7U) == 0U)      /* Gear -> fc                         */
        #define __CORE_CLK   (XTALH * 8U)
      #elif ((SYSCR_Val & 7U) == 4U)      /* Gear -> fc/2                       */
        #define __CORE_CLK   (XTALH * 8U / 2U)
      #elif ((SYSCR_Val & 7U) == 5U)      /* Gear -> fc/4                       */
        #define __CORE_CLK   (XTALH * 8U / 4U)
      #elif ((SYSCR_Val & 7U) == 6U)      /* Gear -> fc/8                       */
        #define __CORE_CLK   (XTALH * 8U / 8U)
      #else                             /* Gear -> reserved                   */
        #define __CORE_CLK   (0U)
      #endif
    #elif ((PLLSEL_Val & 0x0000FFF8) == 0x00007218UL) /* If PLL set as 4 times */
      #if   ((SYSCR_Val & 7U) == 0U)      /* Gear -> fc                         */
        #define __CORE_CLK   (XTALH * 4U)
      #elif ((SYSCR_Val & 7U) == 4U)      /* Gear -> fc/2                       */
        #define __CORE_CLK   (XTALH * 4U / 2U)
      #elif ((SYSCR_Val & 7U) == 5U)      /* Gear -> fc/4                       */
        #define __CORE_CLK   (XTALH * 4U / 4U)
      #elif ((SYSCR_Val & 7U) == 6U)      /* Gear -> fc/8                       */
        #define __CORE_CLK   (XTALH * 4U / 8U)
      #else                             /* Gear -> reserved                   */
        #define __CORE_CLK   (0U)
      #endif
    #else
      #define __CORE_CLK   (0U)
    #endif
  #else
    #if   ((SYSCR_Val & 7U) == 0U)      /* Gear -> fc                         */
      #define __CORE_CLK   (XTALH)
    #elif ((SYSCR_Val & 7U) == 4U)      /* Gear -> fc/2                       */
      #define __CORE_CLK   (XTALH / 2U)
    #elif ((SYSCR_Val & 7U) == 5U)      /* Gear -> fc/4                       */
      #define __CORE_CLK   (XTALH / 4U)
    #elif ((SYSCR_Val & 7U) == 6U)      /* Gear -> fc/8                       */
      #define __CORE_CLK   (XTALH / 8U)
    #else                             /* Gear -> reserved                   */
      #define __CORE_CLK   (0U)
    #endif
  #endif                              /* If PLL not used                    */
#endif

/* Clock Variable definitions */
uint32_t SystemCoreClock = __CORE_CLK;/*!< System Clock Frequency (Core Clock)*/


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Update SystemCoreClock according register values.
 */
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  /* Determine clock frequency according to clock register values        */
  if (TSB_CG_CKSEL_SYSCK) {           /* If system clock is low-speed clock  */
    SystemCoreClock = XTALL;
  } else {                        /* If system clock is high-speed clock */
    if (TSB_CG_PLLSEL_PLLSEL && TSB_CG_OSCCR_PLLON) {   /* If PLL enabled      */
      if      ((TSB_CG->PLLSEL  & 0x0000FFF8) == 0x0000A138UL){ /* If PLL set as 8 times */
        switch (TSB_CG->SYSCR & 7U) {
          case 0U:                                 /* Gear -> fc          */
            SystemCoreClock = XTALH * 8U;
            break;
          case 1U:
          case 2U:
          case 3U:
          case 7U:                                 /* Gear -> reserved    */
            SystemCoreClock = 0U;
            break;
          case 4U:                                 /* Gear -> fc/2        */
            SystemCoreClock = XTALH * 8U / 2U;
            break;
          case 5U:                                 /* Gear -> fc/4        */
            SystemCoreClock = XTALH * 8U / 4U;
            break;
          case 6U:                                 /* Gear -> fc/8        */
            SystemCoreClock = XTALH * 8U / 8U;
            break;
          default:
            SystemCoreClock = 0U;
        }
      }
      else if ((TSB_CG->PLLSEL & 0x0000FFF8) == 0x00007218UL){ /* If PLL set as 4 times */
        switch (TSB_CG->SYSCR & 7U) {
          case 0U:                                 /* Gear -> fc          */
            SystemCoreClock = XTALH * 4U;
            break;
          case 1U:
          case 2U:
          case 3U:
          case 7U:                                 /* Gear -> reserved   */
            SystemCoreClock = 0U;
            break;
          case 4U:                                 /* Gear -> fc/2      */
            SystemCoreClock = XTALH * 4U / 2U;
            break;
          case 5U:                                 /* Gear -> fc/4      */
            SystemCoreClock = XTALH * 4U / 4U;
            break;
          case 6U:                                 /* Gear -> fc/8      */
            SystemCoreClock = XTALH * 4U / 8U;
            break;
          default:
            SystemCoreClock = 0U;
        }
      }
      else {
        SystemCoreClock = 0U;
      }
    } else {
      switch (TSB_CG->SYSCR & 7U) {
        case 0U:                                 /* Gear -> fc          */
          SystemCoreClock = XTALH;
          break;
        case 1U:
        case 2U:
        case 3U:
        case 7U:                                 /* Gear -> reserved    */
          SystemCoreClock = 0U;
          break;
        case 4U:                                 /* Gear -> fc/2        */
          SystemCoreClock = XTALH / 2U;
          break;
        case 5U:                                 /* Gear -> fc/4        */
          SystemCoreClock = XTALH / 4U;
          break;
        case 6U:                                 /* Gear -> fc/8        */
          SystemCoreClock = XTALH / 8U;
          break;
        default:
          SystemCoreClock = 0U;
      }
    }
  }
}
/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void)
{
#if (WD_SETUP)                  /* Watchdog Setup */
  TSB_WD->MOD = WDMOD_Val;
  if (!TSB_WD_MOD_WDTE) {           /* If watchdog is to be disabled */
    TSB_WD->CR = WDCR_Val;
  }
#endif

#if (CLOCK_SETUP)               /* Clock Setup */
  TSB_CG->SYSCR  = SYSCR_Val;
  TSB_CG->STBYCR = STBYCR_Val;

  TSB_CG->OSCCR  = OSCCR_Ready;
  TSB_CG->PLLSEL = PLLSEL_Ready;
  TSB_CG_OSCCR_WUEON = 1U;
  while(TSB_CG_OSCCR_WUEF){}        /* Wait for PLL to be stable */

  TSB_CG->OSCCR  = OSCCR_Val;
  if (TSB_CG_OSCCR_PLLON) {         /* If PLL enabled */
    TSB_CG_OSCCR_WUEON = 1U;
    while(TSB_CG_OSCCR_WUEF){}      /* Warm-up         */
  }
  TSB_CG->PLLSEL = PLLSEL_Val;
  TSB_CG->CKSEL  = CKSEL_Val;
#endif
}

