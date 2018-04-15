/* setBfree - DSP tonewheel organ
 *
 * Copyright (C) 2003-2004 Fredrik Kilander <fk@dsv.su.se>
 * Copyright (C) 2008-2012 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2012 Will Panther <pantherb@setbfree.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ovt_quadratic.c --- Quadratic transfer function for overmaker.c */
#include <stdio.h>
#include <stdlib.h>

#include "overmaker.h"

/* The number of MIDI controllers requested by this module */

#define NOF_MIDI_CONTROLLERS 1

static char buf[BUFSZ];

/*
 * The names of the MIDI controller adapter functions.
 */

static char* controllerNames[NOF_MIDI_CONTROLLERS] = {
	"setQuadraticThreshold"
};
int
getNofMC_quadratic ()
{
	return NOF_MIDI_CONTROLLERS;
}
char*
getMCNam_quadratic (int i)
{
	return controllerNames[i];
}
/*
 * Sets up static and global symbols in the overdrive.c file which is
 * generated by overmaker.c
 */

void
hdr_quadratic ()
{
	commentln ("Constants for the quadratic transfer function");
	codeln ("static float q_Ts  = 0.0;");
	codeln ("static float q_Tsi = 0.0;");
	codeln ("static float q_Ta  = 0.0;");
	codeln ("static float q_Tn  = 0.0;");
	codeln ("static float q_Tni = 0.0;");
	codeln ("static float q_Th  = 1.0;");
}

/*
 * This function emits code to produce a function that allows for
 * runtime configuration of the transfer function. It is typically
 * called once from the module initialization code and later from
 * a MIDI controller adapter (if implemented).
 */
void
cfg_quadratic ()
{
	codeln ("void cfg_quadratic (float threshold) {");
	pushIndent ();
	codeln ("q_Ts  = 1.0 - threshold;");
	codeln ("q_Tsi = 1.0 / q_Ts;");
	codeln ("q_Ta  = 1.0 + threshold;");
	codeln ("q_Tn  = 0.5 * q_Ta;");
	codeln ("q_Tni = 1.0 / q_Tn;");
	codeln ("q_Th  = threshold;");
	codeln ("printf (\"\\rTHR=%10.4f\", threshold);");
	codeln ("fflush (stdout);");
	popIndent ();
	codeln ("}");
}

/*
 * Emits code to produce a MIDI controller adapter for the quadratic
 * transfer function.
 */
void
ctl_quadratic ()
{
	sprintf (buf, "void %s (unsigned char uc) {", controllerNames[0]);
	codeln (buf);
	pushIndent ();
	codeln ("cfg_quadratic (((float) uc) / 127.0);");
	popIndent ();
	codeln ("}");
}

/*
 * Emits inline code to perform the initialization.
 */
void
ini_quadratic ()
{
	codeln ("cfg_quadratic (0.5);");
}

/*
 * Inline transfer function.
 * @param xs String of input sample expression.
 * @param ys String of output sample expression (to be assigned).
 */
void
xfr_quadratic (char* xs, char* ys)
{
	char buf[BUFSZ];
	commentln ("Quadratic function");
	codeln ("{");
	pushIndent ();
	codeln ("float tx;");
	sprintf (buf, "tx = (%s < 0.0) ? -%s : %s;", xs, xs, xs);
	codeln (buf);
	codeln ("if (tx < q_Th) {");
	pushIndent ();
	sprintf (buf, "%s = tx * q_Tni;", ys);
	codeln (buf);
	popIndent ();
	codeln ("}");
	codeln ("else if (tx < 1.0) {");
	pushIndent ();
	codeln ("float xa = tx - q_Th;");
	codeln ("float xb = xa * q_Tsi;");
	sprintf (buf, "%s = (q_Th + (xa / (1.0 + (xb * xb)))) * q_Tni;", ys);
	codeln (buf);
	popIndent ();
	codeln ("}");
	codeln ("else {");
	pushIndent ();
	sprintf (buf, "%s = 1.0;", ys);
	codeln (buf);
	popIndent ();
	codeln ("}");
	sprintf (buf, "if (%s < 0.0) %s = -%s;", xs, ys, ys);
	codeln (buf);

	popIndent ();
	codeln ("}");
}
