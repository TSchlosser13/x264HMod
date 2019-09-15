/******************************************************************************
 * x264HMod.h: CHIP-Wrapper für x264
 ******************************************************************************
 * v1.0 - 01.04.2016
 *
 * Copyright (c) 2016 Tobias Schlosser
 *  (tobias.schlosser@informatik.tu-chemnitz.de)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/


#ifndef X264HMOD_H
#define X264HMOD_H


#include "../x264.h"
#include "Frames_args.h"

#include "HMEC/CHIP/CHIP/Hexarray.h"


RGB_Hexarray frame_last;


void x264HMod_init(int argc, char** argv, Frames_args* args);

void x264HMod(x264_picture_t* pic, x264_param_t param, int i_frame,
 Frames_args args);


#endif

