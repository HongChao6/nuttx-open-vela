/****************************************************************************
 * graphics/nxglib/lcd/nxglib_setpixel.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>

#include <nuttx/lcd/lcd.h>
#include <nuttx/nx/nxglib.h>

#include "nxglib_bitblit.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef NXGLIB_SUFFIX
#  error "NXGLIB_SUFFIX must be defined before including this header file"
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxgl_setpixel_*bpp
 *
 * Description:
 *   Draw a single pixel in LCD memory at the given position and with the
 *   given color.  This is equivalent to nxgl_fillrectangle_*bpp() with a 1x1
 *   rectangle but is more efficient.
 *
 ****************************************************************************/

void NXGL_FUNCNAME(nxgl_setpixel, NXGLIB_SUFFIX)
(
  FAR struct lcd_planeinfo_s *pinfo,
  FAR const struct nxgl_point_s *pos,
  NXGL_PIXEL_T color)
{
#if NXGLIB_BITSPERPIXEL < 8
  uint8_t shift;
  uint8_t mask;
  uint8_t pixel;

  /* Read the byte that contains the pixel to be changed */

  pinfo->getrun(pinfo->dev, pos->y, pos->x, &pixel, 8 / NXGLIB_BITSPERPIXEL);

  /* Shift the color into the proper position */

# ifdef CONFIG_NX_PACKEDMSFIRST

#  if NXGLIB_BITSPERPIXEL == 1
  shift   = (7 - (pos->x & 7));              /* Shift is 0, 1, ... 7 */
  mask    = (1 << shift);                    /* Mask is 0x01, 0x02, .. 0x80 */
  color <<= shift;                           /* Color is positioned under the mask */
#  elif NXGLIB_BITSPERPIXEL == 2
  shift   = (6 - ((pos->x & 3) << 1));       /* Shift is 0, 2, 4, or 6 */
  mask    = (3 << shift);                    /* Mask is 0x03, 0x0c, 0x30, or 0xc0 */
  color <<= shift;                           /* Color is positioned under the mask */
#  elif NXGLIB_BITSPERPIXEL == 4
  shift   = (4 - ((pos->x & 1) << 2));       /* Shift is 0 or 4 */
  mask    = (15 << shift);                   /* Mask is 0x0f or 0xf0 */
  color <<= shift;                           /* Color is positioned under the mask */
#  else
#    error "Unsupported pixel depth"
#  endif

#else /* CONFIG_NX_PACKEDMSFIRST */

#  if NXGLIB_BITSPERPIXEL == 1
  shift   = (pos->x & 7);                    /* Shift is 0, 1, ... 7 */
  mask    = (1 << shift);                    /* Mask is 0x01, 0x02, .. 0x80 */
  color <<= shift;                           /* Color is positioned under the mask */
#  elif NXGLIB_BITSPERPIXEL == 2
  shift   = (pos->x & 3) << 1;               /* Shift is 0, 2, 4, or 6 */
  mask    = (3 << shift);                    /* Mask is 0x03, 0x0c, 0x30, or 0xc0 */
  color <<= shift;                           /* Color is positioned under the mask */
#  elif NXGLIB_BITSPERPIXEL == 4
  shift   = (pos->x & 1) << 2;               /* Shift is 0 or 4 */
  mask    = (15 << shift);                   /* Mask is 0x0f or 0xf0 */
  color <<= shift;                           /* Color is positioned under the mask */
#  else
#    error "Unsupported pixel depth"
#  endif
#endif /* CONFIG_NX_PACKEDMSFIRST */

  /* Handle masking of the fractional byte */

  pixel = (pixel & ~mask) | (color & mask);

  /* Write the modified byte back to graphics memory */

  pinfo->putrun(pinfo->dev,
                pos->y,
                pos->x,
                (FAR uint8_t *)&pixel,
                8 / NXGLIB_BITSPERPIXEL);
#else
  /* Draw a single pixel at this position raster line at this row */

  pinfo->putrun(pinfo->dev, pos->y, pos->x, (FAR uint8_t *)&color, 1);
#endif
}
