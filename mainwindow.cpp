#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->xAxis->setLabel("X");
    ui->widget->yAxis->setLabel("Y");
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // Чтобы менять масштаб мышкой и двигаться по графику
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    //ui->lineEdit->text()
}

void MainWindow::on_startBtn_clicked()
{
    QString data = ui->lineEdit->text().simplified(); // заменяю любое число пробелов на один
    ui->lineEdit->setText(data);
    data.replace(" ", ""); // убираю все пробелы

    addCurve();
    showInfo();
}

QMap<QString, double> MainWindow::parseEquation(QString equation)
{
    equation = equation + "#";

    coeffs.insert("x", 0);
    coeffs.insert("y", 0);
    coeffs.insert("x^2", 0);
    coeffs.insert("y^2", 0);
    coeffs.insert("xy", 0);
    coeffs.insert("freeMember", 0);
    QChar tmpChar;

    QString StrCoeff;
    int intCoeff;
    bool isRight = false;
    for (int i = 0; i < equation.size(); i++)
    {
        tmpChar = equation[i];

        if (tmpChar.isDigit())
        {
            StrCoeff += tmpChar;
        }

        if (tmpChar == 'x' || tmpChar == 'y' ||  tmpChar == '#' || ((tmpChar == '+' || tmpChar == '-' || tmpChar == '=') && StrCoeff != ""))
        {
            intCoeff = StrCoeff.toInt();
            if(StrCoeff == "" || StrCoeff == "+")
                intCoeff = 1;
            if(StrCoeff == "-")
                intCoeff = -1;
            if(isRight)
                intCoeff *= -1;

            QString variable;
            while (true)
            {
                tmpChar = equation[i];
                if(tmpChar == '+' || tmpChar == '-' || tmpChar == '='  || i == equation.size()-1)
                    break;
                variable += tmpChar;
                i++;
            }

            if (variable == "yx") variable = "xy";

            if (variable == "")
            {
                variable = "freeMember";
            }
            else if (coeffs.find(variable) == coeffs.end())
            {
                coeffs.clear();
                return coeffs;
            }

            qDebug() << variable << intCoeff;
            coeffs[variable] += intCoeff;

            StrCoeff = "";
            if(i == equation.size()) break;
        }

        if(tmpChar == '+' || tmpChar == '-')
        {
            StrCoeff = tmpChar + StrCoeff;
        }

        if(tmpChar == '=')
        {
            isRight = true;
        }
    }
    return coeffs;
}

void MainWindow::drawCurve(QMap<QString, double> coeffs)
{
    QVector<double> xVect, yVect;
    qDebug() << ui->min->value() << " " << ui->max->value();
    for (double x = ui->min->value(); x <= ui->max->value(); x += 0.01)
    {
        if (a22 == 0)
        {
            xVect.push_back((double)x);
            yVect.push_back(double((a11*x*x+2*a13*x+a33) / 2*(a12*x+a23)));
        }
        else
        {
        // a*x*x+b*y*y+2*c*x*y+2*d*x+2*e*y+f=0;
            discriminant = (a12*x+a23)*(a12*x+a23)-a22*(a11*x*x+2*a13*x+a33);
            if (discriminant == 0)
            {
                xVect.push_back((double)x);
                yVect.push_back(double(-(a12*x+a23)/a22));
            }
            else
                if (discriminant > 0)
                {
                    xVect.push_back((double)x);
                    yVect.push_back(double((-(a12*x+a23)+sqrt(discriminant))/a22));

                    xVect.push_back((double)x);
                    yVect.push_back(double((-(a12*x+a23)-sqrt(discriminant))/a22));
                }
        }
    }
    qDebug() << xVect;
    qDebug() << yVect;

    ui->widget->addGraph();
    ui->widget->graph(0)->data()->clear();
    ui->widget->graph(0)->setPen(QColor(50, 50, 50, 255));
    if (yVect.size() == 0) {
        qDebug() << "Не найдено действительных корней!";
        return;
    }
    //ui->widget->graph(0)->addData(QVector<double>::fromStdVector(xVect), QVector<double>::fromStdVector(yVect));
    ui->widget->graph(0)->addData(xVect, yVect);
    /*auto yMin = min_element(yVect.begin(), yVect.end());
    auto yMax = max_element(yVect.begin(), yVect.end());*/
    double minY = yVect[0], maxY = yVect[0];
    for (int i = 1; i < yVect.size(); i++)
    {
        if (yVect[i] < minY) minY = yVect[i];
        if (yVect[i] > maxY) maxY = yVect[i];
    }
    //ui->widget->graph(0)->setLineStyle((QCPGraph::lsNone));
    ui->widget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    /*if (*yMax!=*yMin)
        ui->widget->yAxis->setRange(*yMin,*yMax);
    else
        ui->widget->yAxis->setRange(*yMin-5,*yMin+5);*/
    //ui->widget->yAxis->setRange(minY, maxY);
    ui->widget->replot();
}

void MainWindow::addCurve()
{
    QMap<QString, double> variables = parseEquation(ui->lineEdit->text());

    if(variables.isEmpty())
        return;

    drawCurve(variables);
}

void MainWindow::showInfo()
{
    a12 = coeffs["xy"] / 2;
    a11 = coeffs["x^2"];
    a22 = coeffs["y^2"];
    a33 = coeffs["freeMember"];
    a13 = coeffs["x"] / 2;
    a23 = coeffs["y"] / 2;


    qDebug() << "a11 = " << a11;
    qDebug() << "a22 = " << a22;
    qDebug() << "a33 = " << a33;
    qDebug() << "a12 = " << a12;
    qDebug() << "a13 = " << a13;
    qDebug() << "a23 = " << a23;

    double det = (a11 * a22 * a33 + 2*(a13 * a23 * a12)) -
            (a13 * a22 * a13 + a11* a23 * a23 + a33 * a12 * a12);
    double D = a11 * a22 - a12 * a12;
    double I = a11 + a22;
    double B = a11* a33 - a13 * a13 + a22 * a33 - a23 * a23;

    qDebug() << det;
    qDebug() << D;
    qDebug() << I;
    qDebug() << B;

    ui->detLabel->setText("det = " + QString::number(det));
    ui->DLabel->setText("D = " + QString::number(D));
    ui->ILabel->setText("I = " + QString::number(I));
    ui->BLabel->setText("B = " + QString::number(B));

    if (!det)
    {
        if (D > 0) ui->curveType->setText("Точка (вырожденный эллипс)");
        else if (D < 0) ui->curveType->setText("Пара пересекающихся прямых (вырожденная гипербола)");
        else if (D == 0)
        {
            if (B > 0) ui->curveType->setText("Пара мнимых параллельных кривых");
            else if (B < 0) ui->curveType->setText("Прямая (две совпадающих прямых)");
            else if (B == 0) ui->curveType->setText("Вырожденная гипербола");
        }
    }
    else
    {
        if (a11 == a22 && a12 == 0) ui->curveType->setText("Окружность");
        else if (D > 0 && I*det < 0) ui->curveType->setText("Эллипс");
        else if (D == 0 && I*det > 0) ui->curveType->setText("Мнимый эллипс");
        else if (D < 0) ui->curveType->setText("Гипербола");
        else if (D == 0) ui->curveType->setText("Парабола");
    }
}


void MainWindow::on_clear_btn_clicked()
{
    ui->widget->clearItems();
    ui->widget->replot();
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_action_author_triggered()
{
    QMessageBox::about(this, "Автор", "Unknown author");
}

void MainWindow::on_action_help_triggered()
{
    QMessageBox::about(this, "Помощь", "Помощи не будет. Программа отдыхает.");
}
