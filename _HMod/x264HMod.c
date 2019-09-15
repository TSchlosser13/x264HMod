/******************************************************************************
 * x264HMod.c: CHIP-Wrapper für x264
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <math.h>

#include <stdint.h>
#include "../x264.h"
#include "Frames_args.h"

#include "HMEC/CHIP/Misc/Types.h"
#include "HMEC/CHIP/Misc/pArray2d.h"
#include "HMEC/CHIP/Misc/Precalcs.h"

#include "HMEC/CHIP/CHIP/Hexarray.h"
#include "HMEC/CHIP/CHIP/Hexsamp.h"
#include "HMEC/CHIP/HDCT/HDCT.h"
#include "HMEC/CHIP/HFBs/HFBs.h"
#include "HMEC/CHIP/Quant/Quant.h"

#include "HMEC/HMEC/HMEC.h"

#include "x264HMod.h"


RGB_Hexarray frame_last;


void x264HMod_init(int argc, char** argv, Frames_args* args) {
	char* params[5] = { "++enable_HMod",  \
	                    "++enable_quant", \
	                    "++enable_HFBs",  \
	                    "++enable_scale", \
	                    "++enable_HMEC" };

	args->enable_HMod  = false;
	args->enable_quant = false;
	args->enable_HFBs  = false;
	args->enable_scale = false;
	args->enable_HMEC  = false;


	for(unsigned int i = 0; i < argc; i++) {
		if(!strcmp(argv[i], params[0])) {
			args->enable_HMod = true;

			args->order     = (unsigned int)atoi(argv[++i]);
			args->scale_hip =        (float)atof(argv[++i]);
			args->scale_sq  =        (float)atof(argv[++i]);
			args->mode      = (unsigned int)atoi(argv[++i]);
			args->radius    =        (float)atof(argv[++i]);
			args->threads   = (unsigned int)atoi(argv[++i]);

			printf(
				"HMod [info]: order = %u, scale_hip = %.2f, scale_sq = %.2f,"
				" mode = %u, radius = %.2f, threads = %u\n",
				 args->order, args->scale_hip, args->scale_sq,
				 args->mode, args->radius, args->threads);

		} else if(args->enable_HMod && !strcmp(argv[i], params[1])) {
			args->enable_quant = true;

			args->quant_mode = (unsigned int)atoi(argv[++i]);
			args->quant_qf   = (unsigned int)atoi(argv[++i]);

			printf(" -> quant aktiviert: mode = %u, qf = %u\n", \
				args->quant_mode, args->quant_qf);

		} else if(args->enable_HMod && !strcmp(argv[i], params[2])) {
			args->enable_HFBs = true;

			args->filter = (unsigned int)atoi(argv[++i]);

			printf(" -> HFBs aktiviert: filter = %u\n", args->filter);

		} else if(args->enable_HMod && !strcmp(argv[i], params[3])) {
			args->enable_scale = true;

			args->scale = (unsigned int)atoi(argv[++i]);

			printf(" -> scale aktiviert: scale = %u\n", args->scale);

		} else if(args->enable_HMod && !strcmp(argv[i], params[4])) {
			args->enable_HMEC = true;

			args->HMEC_size   = (unsigned int)atoi(argv[++i]);
			args->HMEC_mode   = (unsigned int)atoi(argv[++i]);
			args->HMEC_metric = (unsigned int)atoi(argv[++i]);
			args->HMEC_range  =        (float)atof(argv[++i]);
			args->HMEC_factor =        (float)atof(argv[++i]);

			printf(
				" -> HMEC aktiviert: size = %u, mode = %u, metric = %u,"
				" range = %.2f, factor = %.2f\n",
				 args->HMEC_size, args->HMEC_mode, args->HMEC_metric,
				 args->HMEC_range, args->HMEC_factor);
		}
	}

	putchar('\n');


	if(args->enable_HMod) {
		if(!args->enable_HMEC) {
			precalcs_init(args->order, args->scale_sq, args->radius);
		} else {
			precalcs_init(args->order + 1, args->scale_sq, args->radius);
		}


		struct stat st = {0};

		if(stat("Data", &st) == -1)
			mkdir("Data", 0700);
	}
}


void x264HMod(x264_picture_t* pic, x264_param_t param, int i_frame,
 Frames_args args) {
	char         filename[64];
	RGB_Array    rgb_array;
	RGB_Hexarray rgb_hexarray;


	// Patch Größe Hexarray
	if(args.order == 5) {
		pArray2d_init(&rgb_array,
			param.i_width  <=  305 ?  305 : (unsigned int)param.i_width,
			param.i_height <=  297 ?  297 : (unsigned int)param.i_height);
	} else if(args.order == 6) {
		pArray2d_init(&rgb_array, \
			param.i_width  <=  777 ?  777 : (unsigned int)param.i_width,
			param.i_height <=  817 ?  817 : (unsigned int)param.i_height);
	} else if(args.order == 7) {
		pArray2d_init(&rgb_array,
			param.i_width  <= 2141 ? 2141 : (unsigned int)param.i_width,
			param.i_height <= 2061 ? 2061 : (unsigned int)param.i_height);
	} else {
		pArray2d_init(&rgb_array, (unsigned int)param.i_width, (unsigned int)param.i_height);
	}




	const unsigned int i_width_2 = (unsigned int)param.i_width / 2;

	unsigned int w_min, w_off, w_off_p;
	unsigned int h_min, h_off, h_off_p;

	if(param.i_width  < rgb_array.x) {
		w_off  = (rgb_array.x - param.i_width)  / 2;
		w_off -= w_off % 2;
	} else {
		w_off  = 0;
	}

	if(param.i_height < rgb_array.y) {
		h_off  = (rgb_array.y - param.i_height) / 2;
		h_off -= h_off % 2;
	} else {
		h_off  = 0;
	}


	// YCbCr -> RGB
	for(unsigned int h = 0; h < param.i_height; h++) {
		for(unsigned int w = 0; w < param.i_width; w++) {
			const unsigned int p      =  h      * param.i_width   +  w;
			const unsigned int p_CbCr = (h / 2) *       i_width_2 + (w / 2);

			const int C = (unsigned int)pic->img.plane[0][p]      -  16;
			const int D = (unsigned int)pic->img.plane[1][p_CbCr] - 128;
			const int E = (unsigned int)pic->img.plane[2][p_CbCr] - 128;

			const int R = (const int)roundf( 1.164383f * C                 + 1.596027f * E );
			const int G = (const int)roundf( 1.164383f * C - 0.391762f * D - 0.812968f * E );
			const int B = (const int)roundf( 1.164383f * C + 2.017232f * D                 );


			const unsigned int ww = w + w_off;
			const unsigned int hh = h + h_off;

			if(R < 0) {
				rgb_array.p[ww][hh][0] =   0;
			} else if(R > 255) {
				rgb_array.p[ww][hh][0] = 255;
			} else {
				rgb_array.p[ww][hh][0] = (unsigned int)R;
			}
			if(G < 0) {
				rgb_array.p[ww][hh][1] =   0;
			} else if(G > 255) {
				rgb_array.p[ww][hh][1] = 255;
			} else {
				rgb_array.p[ww][hh][1] = (unsigned int)G;
			}
			if(B < 0) {
				rgb_array.p[ww][hh][2] =   0;
			} else if(B > 255) {
				rgb_array.p[ww][hh][2] = 255;
			} else {
				rgb_array.p[ww][hh][2] = (unsigned int)B;
			}
		}
	}




	hipsampleColour(rgb_array, &rgb_hexarray, args.order, args.scale_hip, args.mode); // sq -> HIP
	pArray2d_free(&rgb_array);
	sprintf(filename, "Data/Frame_%u-CHIP.dat", i_frame);
	Hexarray2file(&rgb_hexarray, filename, 0);


	/*
	 * Quantisierung
	 */

	if(args.enable_quant) {
		float**       psiCos_table;
		fRGB_Hexarray frgb_hexrray;


		DCTH2_init(&psiCos_table, 5);

		DCTH2(rgb_hexarray, &frgb_hexrray, psiCos_table, 0, 5);

		if(args.quant_qf > 0 && args.quant_qf < 100)
			Quant_custom(&frgb_hexrray, args.quant_mode + 2, 5, args.quant_qf);

		IDCTH2(frgb_hexrray, &rgb_hexarray, psiCos_table, 0, 5);

		DCTH_free(&psiCos_table, 1, 5);
	}


	/*
	 * Filter
	 */

	if(args.enable_HFBs) {
		if(args.filter < 2) {
			filter_unblurring(&rgb_hexarray, args.filter);
		} else if(args.filter) {
			filter_4x16(&rgb_hexarray, args.filter - 2);
		}
	}


	/*
	 * Skalierung
	 */

	if(args.enable_scale) {
		if(!args.scale) {
			Hexarray_scale_HIP(&rgb_hexarray, 0, 10);
		} else {
			Hexarray_scale_HIP(&rgb_hexarray, 1, 10);
		}
	}


	sqsampleColour(rgb_hexarray, &rgb_array, args.radius, args.scale_sq, args.mode, args.threads); // HIP -> sq


	/*
	 * ME und MC
	 */

	if(args.enable_HMEC) {
		if(frame_last.p) {
			unsigned int* vectors;


			const unsigned int vectors_found = HME(&vectors,
				rgb_hexarray, frame_last, args.HMEC_size, args.HMEC_mode,
				args.HMEC_metric, args.HMEC_range, args.HMEC_factor);


			sprintf(filename, "Data/Frame_%u-vectors.dat", i_frame);

			vectors2file(vectors, vectors_found, filename);

			free(vectors);
		} else {
			Hexarray_init(&frame_last, args.order, 0);
		}

		for(unsigned int i = 0; i < frame_last.size; i++) {
			frame_last.p[i][0] = rgb_hexarray.p[i][0];
			frame_last.p[i][1] = rgb_hexarray.p[i][1];
			frame_last.p[i][2] = rgb_hexarray.p[i][2];
		}
	}


	Hexarray_free(&rgb_hexarray, 0);




	if(param.i_width  < rgb_array.x) {
		w_min    = param.i_width;
		w_off    = (rgb_array.x    - param.i_width)  / 2;
		w_off   -= w_off % 2;
		w_off_p  = 0;
	} else {
		w_min    = rgb_array.x;
		w_off    = 0;
		w_off_p  = (param.i_width  - rgb_array.x)    / 2;
		w_off_p -= w_off_p % 2;
	}

	if(param.i_height < rgb_array.y) {
		h_min    = param.i_height;
		h_off    = (rgb_array.y    - param.i_height) / 2;
		h_off   -= h_off % 2;
		h_off_p  = 0;
	} else {
		h_min    = rgb_array.y;
		h_off    = 0;
		h_off_p  = (param.i_height - rgb_array.y)    / 2;
		h_off_p -= h_off_p % 2;
	}


	// Frame leeren
	for(unsigned int h = 0; h < param.i_height; h++) {
		for(unsigned int w = 0; w < param.i_width; w++) {
			pic->img.plane[0][h * param.i_width + w] = (uint8_t)16;

			if(!(w % 2 || h % 2)) {
				const unsigned int p_CbCr = (h / 2) * i_width_2 + (w / 2);

				pic->img.plane[1][p_CbCr] = (uint8_t)128;
				pic->img.plane[2][p_CbCr] = (uint8_t)128;
			}
		}
	}

	// RGB -> YCbCr
	for(unsigned int h = 0; h < h_min; h++) {
		for(unsigned int w = 0; w < w_min; w++) {
			const unsigned int ww = w + w_off;
			const unsigned int hh = h + h_off;

			const unsigned int R = rgb_array.p[ww][hh][0];
			const unsigned int G = rgb_array.p[ww][hh][1];
			const unsigned int B = rgb_array.p[ww][hh][2];


			const unsigned int p = (h + h_off_p) * param.i_width + (w + w_off_p);

			pic->img.plane[0][p] = (uint8_t)(roundf(0.256788f * R + 0.504129f * G + 0.097906f * B) + 16);

			if(!(w % 2 || h % 2)) {
				const unsigned int p_CbCr = (h + h_off_p) / 2 * i_width_2 + (w + w_off_p) / 2;

				pic->img.plane[1][p_CbCr] = (uint8_t)(roundf( -0.148223f * R - 0.290993f * G + 0.439216f * B ) + 128);
				pic->img.plane[2][p_CbCr] = (uint8_t)(roundf(  0.439216f * R - 0.367788f * G - 0.071427f * B ) + 128);
			}
		}
	}




	pArray2d_free(&rgb_array);
}
