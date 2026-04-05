#include "calculatorwindow.h"

#include <QStringList>
#include <QSizePolicy>
#include <QtMath>

CalculatorWindow::CalculatorWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_central = new QWidget(this);
    setCentralWidget(m_central);

    m_rootLayout = new QVBoxLayout(m_central);
    m_rootLayout->setContentsMargins(12, 12, 12, 12);
    m_rootLayout->setSpacing(10);

    m_display = new QLabel("0", m_central);
    m_display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_display->setMinimumHeight(70);

    m_rootLayout->addWidget(m_display);

    m_grid = new QGridLayout();
    m_grid->setSpacing(8);
    m_rootLayout->addLayout(m_grid);

    int number = 1;
    for (int row = 2; row >= 0; --row) {
        for (int col = 0; col < 3; ++col) {
            const int digit = number++;
            const QString obj = QString::number(digit);

            QPushButton* b = new QPushButton(obj, m_central);
            b->setObjectName(obj);

            b->setMinimumSize(60, 55);
            b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            connect(b, &QPushButton::clicked, this, &CalculatorWindow::onButtonClicked);
            m_grid->addWidget(b, row + 1, col);
        }
    }

    QPushButton* b0 = makeButton("0", "0");
    m_grid->addWidget(b0, 4, 0, 1, 2);

    QPushButton* dot = makeButton(".", ".");
    m_grid->addWidget(dot, 4, 2);

    const QStringList ops = { "/", "*", "-", "+", "=" };
    for (int i = 0; i < ops.size(); ++i) {
        const QString op = ops[i];
        QPushButton* b = makeButton(op, op);
        m_grid->addWidget(b, i, 3);
    }

    QPushButton* clear = makeButton("C", "C");
    QPushButton* back  = makeButton("<-", "back");
    QPushButton* sign  = makeButton("±", "sign");

    m_grid->addWidget(clear, 0, 0);
    m_grid->addWidget(back,  0, 1);
    m_grid->addWidget(sign,  0, 2);
}

QPushButton* CalculatorWindow::makeButton(const QString& text, const QString& obj)
{
    QPushButton* b = new QPushButton(text, m_central);
    b->setObjectName(obj);

    b->setMinimumSize(60, 55);
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(b, &QPushButton::clicked, this, &CalculatorWindow::onButtonClicked);

    return b;
}

double CalculatorWindow::currentValue() const
{
    bool ok = false;
    const double v = m_display->text().toDouble(&ok);
    return ok ? v : 0.0;
}

void CalculatorWindow::setDisplayNumber(double v)
{
    if (qFuzzyCompare(v + 1.0, 1.0)) {
        v = 0.0;
    }

    m_display->setText(QString::number(v, 'g', 15));
}

void CalculatorWindow::clearAll()
{
    m_acc = 0.0;
    m_pendingOp.clear();
    m_waitingForNewNumber = true;
    m_error = false;
    m_display->setText("0");
}

void CalculatorWindow::applyPendingOperation(double rhs)
{
    if (m_pendingOp.isEmpty()) {
        m_acc = rhs;
        return;
    }

    if (m_pendingOp == "+") {
        m_acc = m_acc + rhs;
    }
    else if (m_pendingOp == "-") {
        m_acc = m_acc - rhs;
    }
    else if (m_pendingOp == "*") {
        m_acc = m_acc * rhs;
    }
    else if (m_pendingOp == "/") {
        if (qFuzzyIsNull(rhs)) {
            m_error = true;
            m_display->setText("Error");
            return;
        }
        m_acc = m_acc / rhs;
    }
}

void CalculatorWindow::inputDigit(const QString& d)
{
    if (m_error) {
        clearAll();
    }

    if (m_waitingForNewNumber) {
        m_display->setText(d);
        m_waitingForNewNumber = false;
        return;
    }

    if (m_display->text() == "0") {
        m_display->setText(d);
    } else {
        m_display->setText(m_display->text() + d);
    }
}

void CalculatorWindow::inputDot()
{
    if (m_error) {
        clearAll();
    }

    if (m_waitingForNewNumber) {
        m_display->setText("0.");
        m_waitingForNewNumber = false;
        return;
    }

    const QString t = m_display->text();
    if (!t.contains('.')) {
        m_display->setText(t + ".");
    }
}

void CalculatorWindow::inputBackspace()
{
    if (m_error) {
        clearAll();
        return;
    }

    if (m_waitingForNewNumber) {
        return;
    }

    QString t = m_display->text();
    if (!t.isEmpty()) {
        t.chop(1);
    }

    if (t.isEmpty() || t == "-") {
        t = "0";
    }

    m_display->setText(t);
}

void CalculatorWindow::inputOp(const QString& op)
{
    if (m_error) {
        return;
    }

    if (m_waitingForNewNumber) {
        m_pendingOp = op;
        return;
    }

    const double rhs = currentValue();

    if (m_pendingOp.isEmpty()) {
        m_acc = rhs;
    } else {
        applyPendingOperation(rhs);
        if (m_error) {
            return;
        }
    }

    setDisplayNumber(m_acc);

    m_pendingOp = op;
    m_waitingForNewNumber = true;
}

void CalculatorWindow::inputEquals()
{
    if (m_error) {
        return;
    }

    if (m_pendingOp.isEmpty()) {
        return;
    }

    const double rhs = currentValue();
    applyPendingOperation(rhs);
    if (m_error) {
        return;
    }

    setDisplayNumber(m_acc);

    m_pendingOp.clear();
    m_waitingForNewNumber = true;
}

void CalculatorWindow::onButtonClicked()
{
    auto* b = qobject_cast<QPushButton*>(sender());
    if (!b) {
        return;
    }

    const QString key = b->objectName();

    if (key.size() == 1 && key[0].isDigit()) {
        inputDigit(key);
        return;
    }

    if (key == "C") {
        clearAll();
        return;
    }

    if (key == "back") {
        inputBackspace();
        return;
    }

    if (key == ".") {
        inputDot();
        return;
    }

    if (key == "+" || key == "-" || key == "*" || key == "/") {
        inputOp(key);
        return;
    }

    if (key == "=") {
        inputEquals();
        return;
    }

    if (key == "sign") {
        if (m_error) {
            return;
        }

        if (m_waitingForNewNumber) {
            return;
        }

        double v = currentValue();
        v = -v;
        setDisplayNumber(v);
        return;
    }
}
