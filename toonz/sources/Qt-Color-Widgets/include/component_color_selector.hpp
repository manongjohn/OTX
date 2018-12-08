/**
 * \file component_color_selector.hpp
 * \brief Color selector based on few widgets for each component
 *
 * \author caryoscelus
 *
 * \copyright Copyright (C) 2017 caryoscelus
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
#ifndef COMPONENT_COLOR_SELECTOR_HPP
#define COMPONENT_COLOR_SELECTOR_HPP

#include <QWidget>

#include <memory>
#include "color_wheel.hpp"
#include "color_2d_slider.hpp"
#include "hue_slider.hpp"
#include "color_line_edit.hpp"
#include "swatch.hpp"
#include "advanced_color_selector.hpp"

namespace color_widgets {

class ComponentContainer;

/**
 * Color selector consisting of few slider/input components
 */
class ComponentColorSelector : public QWidget {
    Q_OBJECT

public:
    explicit ComponentColorSelector(std::unique_ptr<ComponentContainer> container, QWidget* parent = nullptr);
    virtual ~ComponentColorSelector();

public:
    QColor color() const;

public Q_SLOTS:
    void setColor(QColor c);

Q_SIGNALS:
    void colorChanged(QColor c);

private:
    std::unique_ptr<ComponentContainer> const p;
};

class QCP_EXPORT RgbColorSelector : public ComponentColorSelector {
public:
    explicit RgbColorSelector(QWidget* parent = nullptr);
    virtual ~RgbColorSelector();
};

class QCP_EXPORT HsvColorSelector : public ComponentColorSelector {
public:
    explicit HsvColorSelector(QWidget* parent = nullptr);
    virtual ~HsvColorSelector();
};

} // namespace color_widgets

#endif
