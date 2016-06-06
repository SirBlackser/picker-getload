#ifndef LIFT_H
#define LIFT_H

#include "robotapp.h"
#include <math.h>

/**
 * \brief Initialize robot lift unit, performs lift calibration
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int LiftInit(void);


/**
 * \brief Moves lift to specific height
 * \param Height :Height in mm
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int LiftGoto(float Height);


/**
 * \brief Gets current lift height
 * \param Height :Height in mm will be stored at pointer location. If NULL does nothing
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int LiftGetHeight(float *Height);
#endif
