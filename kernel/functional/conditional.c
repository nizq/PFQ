/***************************************************************
 *
 * (C) 2011-14 Nicola Bonelli <nicola.bonelli@cnit.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 ****************************************************************/

#include "conditional.h"

struct pfq_function_descr high_order_functions[] = {

        { "conditional",	INLINE_FUN(conditional) , FUN_ACTION | FUN_ARG_FUN 	},
        { "when",       	INLINE_FUN(when) 	, FUN_ACTION | FUN_ARG_FUN	},
        { "unless",     	INLINE_FUN(unless)  	, FUN_ACTION | FUN_ARG_FUN	},

        { NULL, NULL}};

