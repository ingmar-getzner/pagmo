/*****************************************************************************
 *   Copyright (C) 2004-2009 The PaGMO development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://apps.sourceforge.net/mediawiki/pagmo                             *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Developers  *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Credits     *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#include "MigrationReplacementPolicy.h"
#include "../../../exceptions.h"

// 09/03/2009: Initial version by Marek Rucinski.

int MigrationReplacementPolicy::getMaxMigrationRate(const Population& population)
{
	if(maxMigrationRateAbs < 0) {
		if(maxMigrationRateFrac > 1.0) {
			pagmo_throw(assertion_error, "Fractional maximum migration rate is greate than 1!");
		}
		return (int)(maxMigrationRateFrac * (double)population.size());
	} else {
                if((size_t)maxMigrationRateAbs > population.size()) {
			pagmo_throw(assertion_error, "Absolute maximum migration rate exceeds population size!");
		}
		return maxMigrationRateAbs;
	}
}

std::ostream &operator<<(std::ostream &s, const MigrationReplacementPolicy& mrp)
{
	s << "Replacement policy type:     " << typeid(mrp).name() << std::endl;
	s << "Maximum migration rate (in): ";
	
	if(mrp.maxMigrationRateAbs < 0) {
		s << (100.0 * mrp.maxMigrationRateFrac) << " %";
	} else {
		s << mrp.maxMigrationRateAbs;
	}
	s << std::endl;
	return s;
}