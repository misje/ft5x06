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

#include "ft5x06.h"
#include "i2cDriver.h"

void ft5x06_queryTouchInfo( int numPoints )
{
   if ( !numPoints )
      return;

   i2c_gets( FT5X06_I2C_ADDR, 0x01,
         /* Gesture ID + number of points: */
         2 +
         /* Touch point data: */
         4 * numPoints +
         /* Two empty/reserved bytes between touch point data: */
         2 * ( numPoints - 1 )
         );

}

int ft5x06_retrieveTouchInfo( struct touchInfo *touchInfo )
{
   if ( !touchInfo )
      return FT5X06_RETTOUCH_NULL_ARG;

   unsigned char buffer[ 30 ];

   if ( i2c_getData( buffer, sizeof( buffer ) ) != I2C_ERR_noError )
      return FT5X06_RETTOUCH_ERR_I2C_ERROR;

   touchInfo->gesture = buffer[ 0 ];

   touchInfo->numPoints = buffer[ 1 ];
   if ( touchInfo->numPoints > 5 )
      return FT5X06_RETTOUCH_ERR_NUM_POINTS;

   int i;
   for ( i = 0; i < 5; ++i )
   {
      /* In order to catch the "put up" event, all the inactive points must be
       * checked, since a point becomes inactive when it has the "put up"
       * event: */
      touchInfo->points[ i ].event = ( buffer[ 2 + i * 6 ] >> 6 ) & 0b11;
      if ( touchInfo->points[ i ].event > 3 )
         return FT5X06_RETTOUCH_ERR_EVENT;

      /* No point in storing coordinates for non-active points (the
       * coordinates are all 0xff): */
      if ( i >= touchInfo->numPoints )
         continue;

      touchInfo->points[ i ].x = ( buffer[ 2 + i * 6 ] & 0x0f ) << 8;
      touchInfo->points[ i ].x |= buffer[ 3 + i * 6 ];

      touchInfo->points[ i ].y = ( buffer[ 4 + i * 6 ] & 0x0f ) << 8;
      touchInfo->points[ i ].y |= buffer[ 5 + i * 6 ];
   }

   return FT5X06_RETTOUCH_OK;
}
