/* 
 * Copyright (C) 2013 Andreas Misje
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \defgroup ft5x06Driver FT5x06 driver
 *
 * \author Andreas Misje
 * \date 21.02.13
 *
 * \brief A simple driver for the FT5x06 touch controller using I²C
 *
 * Implementation is based on the \ref I2CDriver "I²C driver" and requires the
 * user to query for touch data, wait until the I²C driver has finished
 * receiving and then retrieve the data from the I²C RX buffer. Touch data
 * should only be queried when the FT5x06 controller has sent an interrupt.
 */

/**
 * \addtogroup ft5x06Driver
 * @{
 *
 * \file
 */

#ifndef FT5X06_H
#define FT5X06_H

/**
 * \brief FT5x06 I²C address
 */
#define FT5X06_I2C_ADDR 0x38

/**
 * \brief Types of gestures the FT5x06 can recognise
 *
 * \note Only ::FT5X06_GESTURE_zoomIn and ::FT5X06_GESTURE_zoomOut is known to
 * work. Newhaven has been notified of this, but they could not explain why
 * this is so.
 */
enum ft5x06_gesture {
   /** No gestured recognised */
   FT5X06_GESTURE_none     = 0x00,
   /** Up gesture recognised */
   FT5X06_GESTURE_up       = 0x10,
   /** Left gesture recognised */
   FT5X06_GESTURE_left     = 0x14,
   /** Down gesture recognised */
   FT5X06_GESTURE_down     = 0x18,
   /** Right gesture recognised */
   FT5X06_GESTURE_right    = 0x1c,
   /** Zoom in / expand gesture recognised */
   FT5X06_GESTURE_zoomIn   = 0x48,
   /** Zoom out / pinch gesture recognised */
   FT5X06_GESTURE_zoomOut  = 0x49,
};

/**
 * \brief Type of touch event 
 */
enum ft5x06_touchEvent
{
   /** The finger was put down on the touch surface */
   FT5X06_EVENT_putDown    = 0,
   /**
    * \brief The finger was lifted from the touch surface.
    *
    * The touch coordinates are not valid for touch data with this event.
    * Touch points with this event is not included in the number of points
    * counter.
    */
   FT5X06_EVENT_putUp      = 1,
   /** The finger is still in contact with the touch surface */
   FT5X06_EVENT_contact    = 2,
   /** This event is reserved and should be treated as an invalid event */
   FT5X06_EVENT_invalid    = 3,
};

/**
 * \brief Information about where the touch occurred on the touch surface and
 * what kind of event it is
 *
 * As long as the FT5x06 touch controller is correctly configured, coordinates
 * will be within the defined width and height. Origo is top-left corner.
 * Coordinates are not valid when event is ::FT5X06_EVENT_putUp. See
 * ::ft5x06_touchEvent.
 */
struct touchCoor
{
   /** X coordinate */
   unsigned int x;
   /** Y coordinate */
   unsigned int y;
   /** Touch event (see ::ft5x06_touchEvent ) */
   enum ft5x06_touchEvent event;
};

/**
 * \brief Information about gesture (if recognised) and the registered touch
 * points
 *
 * Note that touch points with the ::FT5X06_EVENT_putUp event are not included 
 * in the numPoints count (see ::ft5x06_touchEvent).
 */
struct touchInfo
{
   /** Gesture (if any recognised) (see ::ft5x06_gesture) */
   unsigned char gesture;
   /** Number of touch points registered */
   unsigned char numPoints;
   /**
    * \brief Array of registered touch points
    *
    * If numPoints is not 5, the remaining touchCoor objects in the array have
    * undefined values.
    */
   struct touchCoor points[ 5 ];
};

/**
 * \brief Query the FT5x06 chip on the I²C bus for touch data
 *
 * The \ref I2CDriver "I²C driver" must be initialised and idle.
 *
 * \param numPoints number of touch points to query
 */
void ft5x06_queryTouchInfo( int numPoints );

/**
 * \brief ft5x06_queryTouchInfo() return values
 */
enum ft5x06_retrieveTouchInfoRetVal
{
   /** Everything went fine */
   FT5X06_RETTOUCH_OK               =  0,
   /**
    * An I²C error occurred. Check #i2c_error for more information about the
    * error
    */
   FT5X06_RETTOUCH_ERR_I2C_ERROR    =  1,
   /** \pn{touchInfo} was NULL */
   FT5X06_RETTOUCH_NULL_ARG         =  2,
   /**
    * The number of points reported from the I²C data was incorrect. This may
    * indicate corrupt I²C data or a bug.
    */
   FT5X06_RETTOUCH_ERR_NUM_POINTS   =  3,
   /** This is guaranteed to be a bug */
   FT5X06_RETTOUCH_ERR_EVENT        =  4,
};

/**
 * \brief Parse touch data from I²C RX buffer.
 *
 * Events will stored for all points queried by ft5x06_queryTouchInfo(), but
 * coordinates will only be stored for active points. The \ref I2CDriver "I²C
 * driver" must be initialised and idle.
 *
 * \param touchInfo object to populate
 *
 * \return ft5x06_retrieveTouchInfoRetVal
 */
int ft5x06_retrieveTouchInfo( struct touchInfo *touchInfo );

#endif // FT5X06_H

/**
 * @}
 */
