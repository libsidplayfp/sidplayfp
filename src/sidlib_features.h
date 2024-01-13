/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2024 Leandro Nini
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SIDLIB_FEATURES_H
#define SIDLIB_FEATURES_H

#include <sidplayfp/sidplayfp.h>

#if LIBSIDPLAYFP_VERSION_MAJ > 1
#  define FEAT_CONFIG_CIAMODEL
#  define FEAT_NEW_SONLEGTH_DB
#  define FEAT_DIGIBOOST
#endif

#if LIBSIDPLAYFP_VERSION_MAJ > 1 || (LIBSIDPLAYFP_VERSION_MAJ == 1 && LIBSIDPLAYFP_VERSION_MIN >= 8)
#  define FEAT_THIRD_SID
#  define FEAT_NEW_TUNEINFO_API
#endif

#if LIBSIDPLAYFP_VERSION_MAJ > 2 || (LIBSIDPLAYFP_VERSION_MAJ == 2 && LIBSIDPLAYFP_VERSION_MIN >= 2)
#  define FEAT_REGS_DUMP_SID
#  define FEAT_DB_WCHAR_OPEN
#endif

#if LIBSIDPLAYFP_VERSION_MAJ > 2 || (LIBSIDPLAYFP_VERSION_MAJ == 2 && LIBSIDPLAYFP_VERSION_MIN >= 7)
#  define FEAT_FILTER_RANGE
#endif

#endif
