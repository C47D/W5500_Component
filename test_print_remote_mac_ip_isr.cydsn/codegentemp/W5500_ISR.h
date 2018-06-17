/*******************************************************************************
* File Name: W5500_ISR.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_W5500_ISR_H) /* Pins W5500_ISR_H */
#define CY_PINS_W5500_ISR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "W5500_ISR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 W5500_ISR__PORT == 15 && ((W5500_ISR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    W5500_ISR_Write(uint8 value);
void    W5500_ISR_SetDriveMode(uint8 mode);
uint8   W5500_ISR_ReadDataReg(void);
uint8   W5500_ISR_Read(void);
void    W5500_ISR_SetInterruptMode(uint16 position, uint16 mode);
uint8   W5500_ISR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the W5500_ISR_SetDriveMode() function.
     *  @{
     */
        #define W5500_ISR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define W5500_ISR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define W5500_ISR_DM_RES_UP          PIN_DM_RES_UP
        #define W5500_ISR_DM_RES_DWN         PIN_DM_RES_DWN
        #define W5500_ISR_DM_OD_LO           PIN_DM_OD_LO
        #define W5500_ISR_DM_OD_HI           PIN_DM_OD_HI
        #define W5500_ISR_DM_STRONG          PIN_DM_STRONG
        #define W5500_ISR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define W5500_ISR_MASK               W5500_ISR__MASK
#define W5500_ISR_SHIFT              W5500_ISR__SHIFT
#define W5500_ISR_WIDTH              1u

/* Interrupt constants */
#if defined(W5500_ISR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in W5500_ISR_SetInterruptMode() function.
     *  @{
     */
        #define W5500_ISR_INTR_NONE      (uint16)(0x0000u)
        #define W5500_ISR_INTR_RISING    (uint16)(0x0001u)
        #define W5500_ISR_INTR_FALLING   (uint16)(0x0002u)
        #define W5500_ISR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define W5500_ISR_INTR_MASK      (0x01u) 
#endif /* (W5500_ISR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define W5500_ISR_PS                     (* (reg8 *) W5500_ISR__PS)
/* Data Register */
#define W5500_ISR_DR                     (* (reg8 *) W5500_ISR__DR)
/* Port Number */
#define W5500_ISR_PRT_NUM                (* (reg8 *) W5500_ISR__PRT) 
/* Connect to Analog Globals */                                                  
#define W5500_ISR_AG                     (* (reg8 *) W5500_ISR__AG)                       
/* Analog MUX bux enable */
#define W5500_ISR_AMUX                   (* (reg8 *) W5500_ISR__AMUX) 
/* Bidirectional Enable */                                                        
#define W5500_ISR_BIE                    (* (reg8 *) W5500_ISR__BIE)
/* Bit-mask for Aliased Register Access */
#define W5500_ISR_BIT_MASK               (* (reg8 *) W5500_ISR__BIT_MASK)
/* Bypass Enable */
#define W5500_ISR_BYP                    (* (reg8 *) W5500_ISR__BYP)
/* Port wide control signals */                                                   
#define W5500_ISR_CTL                    (* (reg8 *) W5500_ISR__CTL)
/* Drive Modes */
#define W5500_ISR_DM0                    (* (reg8 *) W5500_ISR__DM0) 
#define W5500_ISR_DM1                    (* (reg8 *) W5500_ISR__DM1)
#define W5500_ISR_DM2                    (* (reg8 *) W5500_ISR__DM2) 
/* Input Buffer Disable Override */
#define W5500_ISR_INP_DIS                (* (reg8 *) W5500_ISR__INP_DIS)
/* LCD Common or Segment Drive */
#define W5500_ISR_LCD_COM_SEG            (* (reg8 *) W5500_ISR__LCD_COM_SEG)
/* Enable Segment LCD */
#define W5500_ISR_LCD_EN                 (* (reg8 *) W5500_ISR__LCD_EN)
/* Slew Rate Control */
#define W5500_ISR_SLW                    (* (reg8 *) W5500_ISR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define W5500_ISR_PRTDSI__CAPS_SEL       (* (reg8 *) W5500_ISR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define W5500_ISR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) W5500_ISR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define W5500_ISR_PRTDSI__OE_SEL0        (* (reg8 *) W5500_ISR__PRTDSI__OE_SEL0) 
#define W5500_ISR_PRTDSI__OE_SEL1        (* (reg8 *) W5500_ISR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define W5500_ISR_PRTDSI__OUT_SEL0       (* (reg8 *) W5500_ISR__PRTDSI__OUT_SEL0) 
#define W5500_ISR_PRTDSI__OUT_SEL1       (* (reg8 *) W5500_ISR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define W5500_ISR_PRTDSI__SYNC_OUT       (* (reg8 *) W5500_ISR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(W5500_ISR__SIO_CFG)
    #define W5500_ISR_SIO_HYST_EN        (* (reg8 *) W5500_ISR__SIO_HYST_EN)
    #define W5500_ISR_SIO_REG_HIFREQ     (* (reg8 *) W5500_ISR__SIO_REG_HIFREQ)
    #define W5500_ISR_SIO_CFG            (* (reg8 *) W5500_ISR__SIO_CFG)
    #define W5500_ISR_SIO_DIFF           (* (reg8 *) W5500_ISR__SIO_DIFF)
#endif /* (W5500_ISR__SIO_CFG) */

/* Interrupt Registers */
#if defined(W5500_ISR__INTSTAT)
    #define W5500_ISR_INTSTAT            (* (reg8 *) W5500_ISR__INTSTAT)
    #define W5500_ISR_SNAP               (* (reg8 *) W5500_ISR__SNAP)
    
	#define W5500_ISR_0_INTTYPE_REG 		(* (reg8 *) W5500_ISR__0__INTTYPE)
#endif /* (W5500_ISR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_W5500_ISR_H */


/* [] END OF FILE */
