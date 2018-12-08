/**
 * \file alpha_slider.hpp
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
#ifndef ALPHA_SLIDER_HPP
#define ALPHA_SLIDER_HPP

// #include <memory>
#include "hue_slider.hpp"

namespace color_widgets {

class QCP_EXPORT AlphaSlider : public GradientSlider {
    Q_OBJECT

public:
    explicit AlphaSlider(QWidget* parent = nullptr);
    virtual ~AlphaSlider();

    QColor color() const;

public Q_SLOTS:
    void setColor(QColor c);

Q_SIGNALS:
    void colorChanged(QColor c);

private:
    QColor m_color;
};

} // namespace color_widgets

#endif
