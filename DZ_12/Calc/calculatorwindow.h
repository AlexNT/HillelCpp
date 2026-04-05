#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QString>

class CalculatorWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget* parent = nullptr);

private slots:
    void onButtonClicked();

private:
    QPushButton* makeButton(const QString& text, const QString& obj);

    double currentValue() const;
    void setDisplayNumber(double v);
    void clearAll();
    void applyPendingOperation(double rhs);
    void inputDigit(const QString& d);
    void inputDot();
    void inputOp(const QString& op);
    void inputEquals();
    void inputBackspace();

private:
    QWidget* m_central = nullptr;
    QLabel* m_display = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
    QGridLayout* m_grid = nullptr;

    double m_acc = 0.0;
    QString m_pendingOp; // "+", "-", "*", "/"
    bool m_waitingForNewNumber = true;
    bool m_error = false;
};

#endif // CALCULATORWINDOW_H
