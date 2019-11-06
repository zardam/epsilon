/*
 * signalprocessing.h
 *
 * (c) 2018 Luka Marohnić
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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
#ifndef __SIGNALPROCESSING_H
#define __SIGNALPROCESSING_H
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "first.h"
#include "gen.h"
#include "unary.h"

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

enum filter_type {
    _LOWPASS_FILTER,
    _HIGHPASS_FILTER
};

bool is_sound_data(const gen &g,int &nc,int &bd,int &sr,int &len);
vecteur decode_chdata(const vecteur &data,int bd,int start=0,int len=-1);
vecteur encode_chdata(const vecteur &data,int bd,double ratio,GIAC_CONTEXT);
gen filter(const vecteur &args,filter_type typ,GIAC_CONTEXT);

gen _createwav(const gen &g,GIAC_CONTEXT);
gen _stereo2mono(const gen &g,GIAC_CONTEXT);
gen _plotwav(const gen &g,GIAC_CONTEXT);
gen _plotspectrum(const gen &g,GIAC_CONTEXT);
gen _channels(const gen &g,GIAC_CONTEXT);
gen _bit_depth(const gen &g,GIAC_CONTEXT);
gen _samplerate(const gen &g,GIAC_CONTEXT);
gen _duration(const gen &g,GIAC_CONTEXT);
gen _channel_data(const gen &g,GIAC_CONTEXT);
gen _cross_correlation(const gen &g,GIAC_CONTEXT);
gen _auto_correlation(const gen &g,GIAC_CONTEXT);
gen _lowpass(const gen &g,GIAC_CONTEXT);
gen _highpass(const gen &g,GIAC_CONTEXT);
gen _moving_average(const gen &g,GIAC_CONTEXT);
gen _resample(const gen &g,GIAC_CONTEXT);
gen _convolution(const gen &g,GIAC_CONTEXT);
gen _threshold(const gen &g,GIAC_CONTEXT);
gen _bartlett_hann_window(const gen &g,GIAC_CONTEXT);
gen _blackman_harris_window(const gen &g,GIAC_CONTEXT);
gen _blackman_window(const gen &g,GIAC_CONTEXT);
gen _bohman_window(const gen &g,GIAC_CONTEXT);
gen _cosine_window(const gen &g,GIAC_CONTEXT);
gen _gaussian_window(const gen &g,GIAC_CONTEXT);
gen _hamming_window(const gen &g,GIAC_CONTEXT);
gen _hann_poisson_window(const gen &g,GIAC_CONTEXT);
gen _hann_window(const gen &g,GIAC_CONTEXT);
gen _parzen_window(const gen &g,GIAC_CONTEXT);
gen _poisson_window(const gen &g,GIAC_CONTEXT);
gen _riemann_window(const gen &g,GIAC_CONTEXT);
gen _triangle_window(const gen &g,GIAC_CONTEXT);
gen _tukey_window(const gen &g,GIAC_CONTEXT);
gen _welch_window(const gen &g,GIAC_CONTEXT);
gen _fourier(const gen &g,GIAC_CONTEXT);
gen _ifourier(const gen &g,GIAC_CONTEXT);
gen _addtable(const gen &g,GIAC_CONTEXT);
gen _rect(const gen &g,GIAC_CONTEXT);
gen _boxcar(const gen &g,GIAC_CONTEXT);
gen _tri(const gen &g,GIAC_CONTEXT);
gen _sinc(const gen &g,GIAC_CONTEXT);

extern const unary_function_ptr * const at_createwav;
extern const unary_function_ptr * const at_plotwav;
extern const unary_function_ptr * const at_plotspectrum;
extern const unary_function_ptr * const at_channels;
extern const unary_function_ptr * const at_samplerate;
extern const unary_function_ptr * const at_bit_depth;
extern const unary_function_ptr * const at_duration;
extern const unary_function_ptr * const at_channel_data;
extern const unary_function_ptr * const at_cross_correlation;
extern const unary_function_ptr * const at_auto_correlation;
extern const unary_function_ptr * const at_lowpass;
extern const unary_function_ptr * const at_highpass;
extern const unary_function_ptr * const at_convolution;
extern const unary_function_ptr * const at_threshold;
extern const unary_function_ptr * const at_bartlett_hann_window;
extern const unary_function_ptr * const at_blackman_harris_window;
extern const unary_function_ptr * const at_blackman_window;
extern const unary_function_ptr * const at_bohman_window;
extern const unary_function_ptr * const at_cosine_window;
extern const unary_function_ptr * const at_gaussian_window;
extern const unary_function_ptr * const at_hamming_window;
extern const unary_function_ptr * const at_hann_poisson_window;
extern const unary_function_ptr * const at_hann_window;
extern const unary_function_ptr * const at_parzen_window;
extern const unary_function_ptr * const at_poisson_window;
extern const unary_function_ptr * const at_riemann_window;
extern const unary_function_ptr * const at_triangle_window;
extern const unary_function_ptr * const at_tukey_window;
extern const unary_function_ptr * const at_welch_window;
extern const unary_function_ptr * const at_fourier;
extern const unary_function_ptr * const at_ifourier;
extern const unary_function_ptr * const at_addtable;
extern const unary_function_ptr * const at_rect;
extern const unary_function_ptr * const at_boxcar;
extern const unary_function_ptr * const at_tri;
extern const unary_function_ptr * const at_sinc;

#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC
#endif // __SIGNALPROCESSING_H
