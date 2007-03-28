/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
#include "ucb1200.h"

void GUI_TOUCH_X_ActivateX(void) {
    ucb1200_activateX();
}

void GUI_TOUCH_X_ActivateY(void) {
    ucb1200_activateY();
}

int  GUI_TOUCH_X_MeasureX(void) {
    return ucb1200_measureX();
}

int  GUI_TOUCH_X_MeasureY(void) {
    return ucb1200_measureY();
}


