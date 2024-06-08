#include "mainwindow.h"
#include "button.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), sumSoFar(0)
    , factorSoFar(0), waitingForOperand(true)
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    for(int i = 0; i < NumDigitButtons; i++)
        digitButtons[i] = createButton(QString::number(i), &MainWindow::digitClicked);

    Button *decimalButton = createButton(tr("."), &MainWindow::pointClicked);
    Button *flipSignButton = createButton(tr("\302\261"),&MainWindow::flipSignClicked);

    Button *backspaceButton = createButton(tr("\342\206\220"), &MainWindow::backspaceClicked);
    Button *clearButton = createButton(tr("C"), &MainWindow::clear);
    Button *clearAllButton = createButton(tr("AC"), &MainWindow::clearAll);

    Button *divButton = createButton(tr("\303\267"), &MainWindow::multDivClicked);
    Button *multButton = createButton(tr("\303\227"), &MainWindow::multDivClicked);
    Button *minusButton = createButton(tr("-"), &MainWindow::plusMinusClicked);
    Button *plusButton = createButton(tr("+"), &MainWindow::plusMinusClicked);

    Button *equalsButton = createButton(tr("="), &MainWindow::equalsClicked);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addWidget(display, 0, 0, 1, 6);
    mainLayout->addWidget(backspaceButton, 1, 0, 1, 2);
    mainLayout->addWidget(clearButton, 1, 2, 1, 2);
    mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);

    for(int i = 1; i < NumDigitButtons; i++) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }
    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(decimalButton, 5, 2);
    mainLayout->addWidget(flipSignButton, 5, 3);

    mainLayout->addWidget(divButton, 2, 4);
    mainLayout->addWidget(multButton, 3, 4);
    mainLayout->addWidget(minusButton, 4, 4);
    mainLayout->addWidget(plusButton, 5, 4);

    mainLayout->addWidget(equalsButton, 5, 5);
    setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
}

void MainWindow::digitClicked() {
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if(display->text() == "0" && digitValue == 0.0)
        return;

    if(waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void MainWindow::plusMinusClicked() {
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(!clickedButton)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0;
        pendingMultOp.clear();
    }

    if(!pendingAddOp.isEmpty()) {
        if(!calculate(operand, pendingAddOp)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAddOp = clickedOperator;
    waitingForOperand = true;
}

void MainWindow::multDivClicked() {
    Button *clickedButton = qobject_cast<Button *>(sender());
    if(!clickedButton)
        return;
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultOp = clickedOperator;
    waitingForOperand = true;
}

void MainWindow::equalsClicked() {
    double operand = display->text().toDouble();

    if(!pendingMultOp.isEmpty()) {
        if(!calculate(operand, pendingMultOp)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0;
        pendingMultOp.clear();
    }
    if(!pendingAddOp.isEmpty()) {
        if(!calculate(operand, pendingAddOp)) {
            abortOperation();
            return;
        }
        pendingAddOp.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0;
    waitingForOperand = true;
}

void MainWindow::pointClicked() {
    if(waitingForOperand)
        display->setText("0");
    if(!display->text().contains("."))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}

void MainWindow::flipSignClicked() {
    QString text = display->text();
    double value = text.toDouble();

    if(value > 0) {
        text.prepend(tr("-"));
    } else if(value < 0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

void MainWindow::backspaceClicked() {
    if(waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if(text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}

void MainWindow::clear() {
    if(waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}

void MainWindow::clearAll() {
    sumSoFar = 0;
    factorSoFar = 0;
    pendingAddOp.clear();
    pendingMultOp.clear();
    display->setText("o");
    waitingForOperand = true;
}

template<typename PointerToMemberFunction>
Button *MainWindow::createButton(const QString &text, const PointerToMemberFunction &member) {
    Button *button = new Button(text);
    connect(button, &Button::clicked, this, member);
    return button;
}

void MainWindow::abortOperation() {
    clearAll();
    display->setText(tr("####"));
}

bool MainWindow::calculate(double rightOperand, const QString &pendingOperator) {
    if(pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if(pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if(pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if(pendingOperator == tr("\303\267")) {
        if(rightOperand == 0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}
