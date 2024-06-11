#include "button.h"

// defines the class of Button, containing text and its parent
Button::Button(const QString &text, QWidget *parent) : QToolButton(parent) {
    // set size policy of buttons to expanding and preferred
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // set the text of the button to the text specified
    setText(text);
}

// defines the size hint for a button
QSize Button::sizeHint() const {
    // sets its size to the built-in sizeHint function
    QSize size = QToolButton::sizeHint();
    // increases its referenced height by 20
    size.rheight() += 20;
    // sets the reference width to the maximum possible with the width and height
    size.rwidth() = qMax(size.width(), size.height());
    // return size
    return size;
}
