/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  Copyright (C) 2001-2011                               * */
/* *  Boris Konev                                           * */
/* *  The University of Liverpool                           * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the GNU        * */
/* *  General Public License as published by the Free       * */
/* *  Software Foundation; either version 2 of the License, * */
/* *  or (at your option) any later version.                * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the GNU General Public * */
/* *  License for more details.                             * */
/* *                                                        * */
/* *  You should have received a copy of the GNU General    * */
/* *  Public License along with this program; if not, write * */
/* *  to the Free Software Foundation, Inc., 59 Temple      * */
/* *  Place, Suite 330, Boston, MA  02111-1307  USA         * */
/* *                                                        * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/
#include "i2s.h"

std::string itos(int number)
{
    /* 10 digits + 1 sign + 1 trailing nul */
    static char buf[12];

	unsigned int unsignedNumber;
	int isNegative = 0;
    char *pos = buf + sizeof(buf) - 1;

	if (number < 0) {
		isNegative = 1;
		unsignedNumber = ((unsigned int)(-(1+number))) + 1;
	} else {
		unsignedNumber = number;
	}

	*pos = 0;

	do {
		*--pos = '0' + (unsignedNumber % 10);
		unsignedNumber /= 10;
	} while (unsignedNumber);

	if (isNegative) {
		*--pos = '-';
	}

	return std::string(pos);
}
