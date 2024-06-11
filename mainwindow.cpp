#include "mainwindow.h"
#include "button.h" // this is responsible for easing the creation of buttons
#include <QGridLayout>
#include <QLineEdit>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), sumSoFar(0)
    , factorSoFar(0), waitingForOperand(true)
{
    // create the display, set it to read-only and set its maximum length to 15 characters
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    // grab the font of the display, increase its size by 8 pixels and re-import it back
    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    // create the buttons for the digits and attach them to the digitClicked function
    for(int i = 0; i < NumDigitButtons; i++)
        digitButtons[i] = createButton(QString::number(i), &MainWindow::digitClicked);

    // create and attach the buttons for the decimal point and to reverse the number
    Button *decimalButton = createButton(tr("."), &MainWindow::pointClicked);
    Button *flipSignButton = createButton(tr("\302\261"),&MainWindow::flipSignClicked);

    // create and attach the buttons for the backspace, clear screen and clear all functions
    Button *backspaceButton = createButton(tr("\342\206\220"), &MainWindow::backspaceClicked);
    Button *clearButton = createButton(tr("C"), &MainWindow::clear);
    Button *clearAllButton = createButton(tr("AC"), &MainWindow::clearAll);

    // create and attach the buttons for the operations (plus, minus, multiply and divide)
    Button *divButton = createButton(tr("\303\267"), &MainWindow::multDivClicked);
    Button *multButton = createButton(tr("\303\227"), &MainWindow::multDivClicked);
    Button *minusButton = createButton(tr("-"), &MainWindow::plusMinusClicked);
    Button *plusButton = createButton(tr("+"), &MainWindow::plusMinusClicked);

    // create and attach the equals button
    Button *equalsButton = createButton(tr("="), &MainWindow::equalsClicked);

    // create the layout grid for the calculator and set its size to a fixed one
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    // add the display and backspace/clear keys to the grid
    mainLayout->addWidget(display, 0, 0, 1, 6);
    mainLayout->addWidget(backspaceButton, 1, 0, 1, 2);
    mainLayout->addWidget(clearButton, 1, 2, 1, 2);
    mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);

    // add the digit keys (except for 0) to the grid
    for(int i = 1; i < NumDigitButtons; i++) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    // add the 0, decimal point and number reversal keys to the grid
    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(decimalButton, 5, 2);
    mainLayout->addWidget(flipSignButton, 5, 3);

    // add the operation keys to the grid
    mainLayout->addWidget(divButton, 2, 4);
    mainLayout->addWidget(multButton, 3, 4);
    mainLayout->addWidget(minusButton, 4, 4);
    mainLayout->addWidget(plusButton, 5, 4);

    // add the equals key to the grid and set the layout to said grid
    mainLayout->addWidget(equalsButton, 5, 5);
    setLayout(mainLayout);

    // set the window title to "Calculator"
    setWindowTitle(tr("Calculator"));
}

// function is invoked if a digit is clicked
void MainWindow::digitClicked() {
    // create clickedButton to read the digit of the button pressed
    Button *clickedButton = qobject_cast<Button *>(sender());
    // read the digit of the button pressed
    int digitValue = clickedButton->text().toInt();

    // if the button pressed is 0, and 0 is currently replaced, return nothing
    if(display->text() == "0" && digitValue == 0)
        return;

    // if waiting for an operation, clear the display and set waitingForOperand to false
    if(waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }

    // set the display to what's displayed there, plus the digit pressed
    display->setText(display->text() + QString::number(digitValue));
}

// function is invoked if either plus or minus are pressed
void MainWindow::plusMinusClicked() {
    // create clickedButton to read the symbol of the button pressed
    Button *clickedButton = qobject_cast<Button *>(sender());
    // if clickedButton is false, return nothing
    if(!clickedButton)
        return;
    // read the symbol of the button pressed and the current value displayed
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    /* if pendingMultOp isn't empty and there are no calculations happenning
    between the displayed value and pendingMultOp, abort the operation and
    return nothing */
    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        // display the current factor, clear factorSoFar and pendingMultOp
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0;
        pendingMultOp.clear();
    }

    /* if pendingAddOp isn't empty and there are no calculations happenning
    between the displayed value and pendingAddOp, abort the operation and
    return nothing */
    if(!pendingAddOp.isEmpty()) {
        if(!calculate(operand, pendingAddOp)) {
            abortOperation();
            return;
        }
        // display the current sum
        display->setText(QString::number(sumSoFar));
    } else {
        // the sum is now equal to the displayed value
        sumSoFar = operand;
    }

    // pendingAddOp is now equal to the operator clicked and waitingForOperand is set to true
    pendingAddOp = clickedOperator;
    waitingForOperand = true;
}

// function is invoked if either multiply or divide are pressed
void MainWindow::multDivClicked() {
    // create clickedButton to read the symbol of the button pressed
    Button *clickedButton = qobject_cast<Button *>(sender());
    // if clickedButton is false, return nothing
    if(!clickedButton)
        return;
    // read the symbol of the button pressed and the current value displayed
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    /* if pendingMultOp isn't empty and there are no calculations happenning
    between the displayed value and pendingMultOp, abort the operation and
    return nothing */
    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        // display the current factor
        display->setText(QString::number(factorSoFar));
    } else {
        // the factor is now equal to the displayed value
        factorSoFar = operand;
    }

    // pendingMultOp is now equal to the operator clicked and waitingForOperand is set to true
    pendingMultOp = clickedOperator;
    waitingForOperand = true;
}

// function is invoked if equals is pressed
void MainWindow::equalsClicked() {
    // read the current value displayed
    double operand = display->text().toDouble();

    /* if pendingMultOp isn't empty and there are no calculations happenning
    between the displayed value and pendingMultOp, abort the operation and
    return nothing */
    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        // clear factorSoFar and pendingMultOp
        operand = factorSoFar;
        factorSoFar = 0;
        pendingMultOp.clear();
    }
    /* if pendingAddOp isn't empty and there are no calculations happenning
    between the displayed value and pendingAddOp, abort the operation and
    return nothing */
    if(!pendingAddOp.isEmpty()) {
        if(!calculate(operand, pendingAddOp)) {
            abortOperation();
            return;
        }
        // clear pendingAddOp
        pendingAddOp.clear();
    } else {
        // the sum is now equal to the displayed value
        sumSoFar = operand;
    }

    // display the current sum, clear sumSoFar and set waitingForOperand to true
    display->setText(QString::number(sumSoFar));
    sumSoFar = 0;
    waitingForOperand = true;
}

// function invoked if the point button is pressed
void MainWindow::pointClicked() {
    // if waitingForOperand is true, display 0
    if(waitingForOperand)
        display->setText("0");
    // if there is currently no point displayed, display a point after whatever was on screen
    if(!display->text().contains("."))
        display->setText(display->text() + tr("."));
    // set waitingForOperand to false
    waitingForOperand = false;
}

// function invoked if the flip sign button is pressed
void MainWindow::flipSignClicked() {
    // read the current value displayed
    QString text = display->text();
    double value = text.toDouble();

    // if the value displayed is above 0, put a dash befor anything else, otherwise remove it
    if(value > 0) {
        text.prepend(tr("-"));
    } else if(value < 0) {
        text.remove(0, 1);
    }
    // display resulting value
    display->setText(text);
}

// function invoked if the backspace button is pressed
void MainWindow::backspaceClicked() {
    // if waitingForOperand is true, return nothing
    if(waitingForOperand)
        return;

    // read the current value displayed
    QString text = display->text();
    // remove the last digit
    text.chop(1);
    // if the value string is now empty,change it to 0 and set waitingForOperand to true
    if(text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    // display resulting value
    display->setText(text);
}

// function is invoked if the clear button is pressed
void MainWindow::clear() {
    // if waitingForOperand is true, return nothing
    if(waitingForOperand)
        return;

    // set displayed value to 0 and set waitingForOperand to true
    display->setText("0");
    waitingForOperand = true;
}

// function is invoked if the clear all button is pressed
void MainWindow::clearAll() {
    // reset all values, set displayed value to 0 and waitingForOperand to true
    sumSoFar = 0;
    factorSoFar = 0;
    pendingAddOp.clear();
    pendingMultOp.clear();
    display->setText("0");
    waitingForOperand = true;
}

// function is invoked when creating a button using a pointer to a function
template<typename PointerToMemberFunction>
Button *MainWindow::createButton(const QString &text, const PointerToMemberFunction &member) {
    // creates a new button with the text set above
    Button *button = new Button(text);
    // connects the button, its clicked signal and the function above
    connect(button, &Button::clicked, this, member);
    // returns now-connected button
    return button;
}

// function is invoked when an abort to an operation is needed
void MainWindow::abortOperation() {
    // run a clearAll and set the displayed value to 4 hashes
    clearAll();
    display->setText(tr("####"));
}

// function is invoked to run an operation (contains the right value in an equation and the operator used)
bool MainWindow::calculate(double rightOperand, const QString &pendingOperator) {
    // if the operator is +, add the right value to sumSoFar
    if(pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    // if the operator is -, subtract the right value from sumSoFar
    } else if(pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    // if the operator is the multiply sign, multiply factorSoFar by the right value
    } else if(pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    // if the operator is the division sign, divide factorSoFar by the right value (unless the right value is 0, then return false)
    } else if(pendingOperator == tr("\303\267")) {
        if(rightOperand == 0)
            return false;
        factorSoFar /= rightOperand;
    }
    // return true if calculation is successful
    return true;
}
