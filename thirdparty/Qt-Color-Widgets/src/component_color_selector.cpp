/**
 * \file component_color_selector.cpp
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
#include "component_color_selector.hpp"

#include "gradient_slider.hpp"
#include "hue_slider.hpp"

#include <QBoxLayout>

namespace color_widgets {

class ComponentContainer {
public:
    ComponentContainer(ComponentColorSelector* widget)
        : w(widget)
        , layout(new QVBoxLayout())
    {
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    }

    virtual ~ComponentContainer() {}

    void addWidget(QSlider* slider) {
        layout->addWidget(slider);
        QObject::connect(slider, &QSlider::valueChanged, [this]() {
            Q_EMIT w->colorChanged(color());
        });
    }

    virtual void init() = 0;

    virtual QColor color() const = 0;
    virtual void setColor(QColor c) = 0;

protected:
    ComponentColorSelector* const w;
    QVBoxLayout* layout;
};

ComponentColorSelector::ComponentColorSelector(std::unique_ptr<ComponentContainer> container, QWidget* parent) :
    QWidget(parent),
    p(std::move(container))
{
    p->init();
}

ComponentColorSelector::~ComponentColorSelector()
{
}

QColor ComponentColorSelector::color() const {
    return p->color();
}

void ComponentColorSelector::setColor(QColor c) {
    p->setColor(c);
}

class RgbContainer : public ComponentContainer
{
public:
    RgbContainer(ComponentColorSelector *widget)
        : ComponentContainer(widget)
        , red_slider(new GradientSlider())
        , green_slider(new GradientSlider())
        , blue_slider(new GradientSlider())
    {
        red_slider->setFirstColor("#000000");
        red_slider->setLastColor("#ff0000");
        red_slider->setMaximum(255);
        green_slider->setFirstColor("#000000");
        green_slider->setLastColor("#00ff00");
        green_slider->setMaximum(255);
        blue_slider->setFirstColor("#000000");
        blue_slider->setLastColor("#0000ff");
        blue_slider->setMaximum(255);

        addWidget(red_slider);
        addWidget(green_slider);
        addWidget(blue_slider);
    }

    void init() override {
        w->setLayout(layout);
    }

    QColor color() const override {
        return QColor(red_slider->value(),
                      green_slider->value(),
                      blue_slider->value(),
                      alpha
        );
    }

    void setColor(QColor c) override {
        red_slider->setValue(c.red());
        green_slider->setValue(c.green());
        blue_slider->setValue(c.blue());
        alpha = c.alpha();
    }

private:
    GradientSlider* red_slider;
    GradientSlider* green_slider;
    GradientSlider* blue_slider;
    int alpha = 255;
};

class HsvContainer : public ComponentContainer
{
public:
    HsvContainer(ComponentColorSelector *widget)
        : ComponentContainer(widget)
        , hue_slider(new HueSlider())
        , saturation_slider(new GradientSlider())
        , value_slider(new GradientSlider())
    {
        hue_slider->setMaximum(360);
        saturation_slider->setFirstColor("#888888");
        saturation_slider->setLastColor("#ff0000");
        saturation_slider->setMaximum(255);
        value_slider->setFirstColor("#000000");
        value_slider->setLastColor("#ffffff");
        value_slider->setMaximum(255);

        addWidget(hue_slider);
        addWidget(saturation_slider);
        addWidget(value_slider);
    }

    void init() override {
        w->setLayout(layout);
    }

    QColor color() const override {
        return QColor::fromHsv(
            hue_slider->value(),
            saturation_slider->value(),
            value_slider->value(),
            alpha
        );
    }

    void setColor(QColor c) override {
        hue_slider->setValue(c.hue());
        value_slider->setValue(c.value());
        saturation_slider->setLastColor(QColor::fromHsv(c.hue(), 255, 255));
        saturation_slider->setValue(c.saturation());
        alpha = c.alpha();
    }

private:
    GradientSlider* hue_slider;
    GradientSlider* saturation_slider;
    GradientSlider* value_slider;
    int alpha = 255;
};

// TODO: use std::make_unique
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

RgbColorSelector::RgbColorSelector(QWidget* parent) :
    ComponentColorSelector(make_unique<RgbContainer>(this), parent)
{
}

RgbColorSelector::~RgbColorSelector() {
}

HsvColorSelector::HsvColorSelector(QWidget* parent) :
    ComponentColorSelector(make_unique<HsvContainer>(this), parent)
{
}

HsvColorSelector::~HsvColorSelector() {
}

} // namespace color_widgets
