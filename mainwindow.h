#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE
class Button;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void digitClicked();
    void plusMinusClicked();
    void multDivClicked();
    void equalsClicked();
    void pointClicked();
    void flipSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();

private:
    template<typename PointerToMemberFunction>
    Button *createButton(const QString &text, const PointerToMemberFunction &member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);

    double sumSoFar;
    double factorSoFar;
    QString pendingAddOp;
    QString pendingMultOp;
    bool waitingForOperand;

    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];
};
#endif // MAINWINDOW_H
