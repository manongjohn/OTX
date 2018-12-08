/**
 * \file alpha_slider.cpp
 * \brief Alpha slider
 *
 * \author caryoscelus
 *
 * \copyright Copyright (C) 2018 caryoscelus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "alpha_slider.hpp"

namespace color_widgets {

const int MAX_VALUE = 1000;

AlphaSlider::AlphaSlider(QWidget* parent) :
    GradientSlider(parent)
{
    setRange(0, MAX_VALUE);
    connect(this, &QSlider::valueChanged, [this](int value) {
        m_color.setAlphaF(value*1.0/MAX_VALUE);
        Q_EMIT colorChanged(m_color);
    });
}

AlphaSlider::~AlphaSlider()
{}

QColor AlphaSlider::color() const {
    return m_color;
}

void AlphaSlider::setColor(QColor c) {
    m_color = c;
    setValue(c.alphaF()*MAX_VALUE);
}

} // namespace color_widgets
