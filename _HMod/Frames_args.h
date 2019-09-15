/******************************************************************************
 * Frames_args.h: Parameter x264HMod
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


#ifndef FRAMES_ARGS_H
#define FRAMES_ARGS_H


#include "HMEC/CHIP/Misc/Defines.h"


typedef struct {
	bool enable_HMod;
	bool enable_quant; // Quantisierung
	bool enable_HFBs;  // Filter
	bool enable_scale; // Skalierung
	bool enable_HMEC;  // ME und MC


	unsigned int order;       // Ordnung Hexarray
	float        scale_hip;   // Skalierung Hintransformation
	float        scale_sq;    // und Rücktransformation
	unsigned int mode;        // Bilineare | bikubische Interpolation
	float        radius;      // Radius Rücktransformation
	unsigned int threads;     // Anzahl Pthreads Rücktransformation

	// Quantisierung
	unsigned int quant_mode;  // exponentielle | lineare | konstante
	unsigned int quant_qf;    // Qualitätsfaktor Quantisierung

	// Filter
	unsigned int filter;      // Blurring- | Unblurring- | Low- | High-pass filters (4)

	// Skalierung
	unsigned int scale;       // Herunterskalierung nach HIP - eine Version | nach HIP - sieben Versionen

	// ME und MC
	unsigned int HMEC_size;   // Größe der Makroblöcke ("MBs")
	unsigned int HMEC_mode;   // nur MBs der CHIP-Sub-Hexarrays verrechnen | alle möglichen MBs
	unsigned int HMEC_metric; // SAD | MAD | MSE | RMSE | PSNR | SSIM | DSSIM
	float        HMEC_range;  // max. Länge der Bewegungsvektoren
	float        HMEC_factor; // zulässiges Max. / Min. aus dem Vergleich der MBs
} Frames_args;


#endif
