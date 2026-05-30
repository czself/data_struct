// 第7章 进阶实战：简易计算器
// 笔记中的完整计算器代码

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QFont>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr) : QWidget(parent), leftOperand(0), isNewInput(true)
    {
        setWindowTitle("计算器");
        setFixedSize(300, 350);

        display = new QLineEdit("0");
        display->setReadOnly(true);
        display->setAlignment(Qt::AlignRight);
        display->setFont(QFont("Consolas", 20));
        display->setMinimumHeight(50);

        QStringList btnTexts = {
            "7", "8", "9", "+",
            "4", "5", "6", "-",
            "1", "2", "3", "*",
            "0", "C", "=", "/"
        };

        QGridLayout *grid = new QGridLayout;
        grid->addWidget(display, 0, 0, 1, 4);

        int row = 1, col = 0;
        for (const QString &text : btnTexts) {
            QPushButton *btn = new QPushButton(text);
            btn->setMinimumSize(60, 50);
            btn->setFont(QFont("Arial", 14));
            grid->addWidget(btn, row, col);

            if (text == "C") {
                connect(btn, &QPushButton::clicked, this, &Calculator::onClearClicked);
            } else if (text == "=") {
                connect(btn, &QPushButton::clicked, this, &Calculator::onEqualClicked);
            } else if (text == "+" || text == "-" || text == "*" || text == "/") {
                connect(btn, &QPushButton::clicked, this, &Calculator::onOperatorClicked);
            } else {
                connect(btn, &QPushButton::clicked, this, &Calculator::onNumberClicked);
            }

            col++;
            if (col > 3) { col = 0; row++; }
        }

        setLayout(grid);
    }

private slots:
    void onNumberClicked()
    {
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (!btn) return;
        QString digit = btn->text();
        if (isNewInput) { display->clear(); isNewInput = false; }
        display->setText(display->text() + digit);
    }

    void onOperatorClicked()
    {
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (!btn) return;
        leftOperand = display->text().toDouble();
        currentOperator = btn->text();
        isNewInput = true;
    }

    void onEqualClicked()
    {
        double rightOperand = display->text().toDouble();
        double result = 0;
        if (currentOperator == "+") result = leftOperand + rightOperand;
        else if (currentOperator == "-") result = leftOperand - rightOperand;
        else if (currentOperator == "*") result = leftOperand * rightOperand;
        else if (currentOperator == "/") {
            if (rightOperand != 0) result = leftOperand / rightOperand;
            else { display->setText("错误"); isNewInput = true; return; }
        }
        display->setText(QString::number(result));
        isNewInput = true;
    }

    void onClearClicked()
    {
        display->setText("0");
        leftOperand = 0;
        currentOperator.clear();
        isNewInput = true;
    }

private:
    QLineEdit *display;
    double leftOperand;
    QString currentOperator;
    bool isNewInput;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Calculator calc;
    calc.show();
    return app.exec();
}

#include "main.moc"