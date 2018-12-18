#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegExp>
#include <QMap>
#include <QString>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_lineEdit_textEdited(const QString &arg1);

    void on_startBtn_clicked();

    QMap<QString, double> parseEquation(QString);

    void drawCurve(QMap<QString, double> variables);

    void addCurve();

    void showInfo();

    void on_clear_btn_clicked();

    void on_action_exit_triggered();

    void on_action_author_triggered();

    void on_action_help_triggered();

private:
    Ui::MainWindow *ui;

    double a11, a22, a33, a12, a13, a23, discriminant;
    QMap<QString, double> coeffs;
    int counter;
    //int minX, maxX, x;
};

#endif // MAINWINDOW_H
