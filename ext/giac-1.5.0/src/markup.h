/*  markup.h
 *
 * copyright (c) 2019 Luka Marohnić
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MARKUP_H
#define MARKUP_H
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "first.h"
#include "gen.h"

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

void enable_texmacs_compatible_latex_export(bool yes);
string export_latex(const gen &g,GIAC_CONTEXT);
bool has_improved_latex_export(const gen &g,string &s,bool override_texmacs,GIAC_CONTEXT);
string export_mathml(const gen &g,GIAC_CONTEXT);
string export_mathml_presentation(const gen &g,GIAC_CONTEXT);
string export_mathml_content(const gen &g,GIAC_CONTEXT);
gen _export_mathml(const gen &g,GIAC_CONTEXT);
gen _xml_print(const gen &g,GIAC_CONTEXT);

#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC

#endif // MARKUP_H
