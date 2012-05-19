/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBFAVORITETOOLPALETTE_H_
#define UBFAVORITETOOLPALETTE_H_

#include <QtGui>

#include "UBActionPalette.h"

class UBFavoriteToolPalette : public UBActionPalette
{
    Q_OBJECT;

    public:
        UBFavoriteToolPalette(QWidget* parent = 0);
        virtual ~UBFavoriteToolPalette();

    private slots:
        void addFavorite();
};

#endif /* UBFAVORITETOOLPALETTE_H_ */
