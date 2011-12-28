/*
    QSchoolManager
    Copyright (C) 2011 Pavel Zinin <pzinin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtSql>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    protected:
        void closeEvent(QCloseEvent *e);

private:
    Ui::MainWindow *ui;
    QSettings *s;
    void loadGeometry(); //загружаем состояние окна
    void writeGeometry(); //записываем состояние окна
    void  createClassesList();
    int _currentId;
    void openObject (int id);
    void truncPupil (int id);
    void movePupil (int id, int to);
 private slots:
    void updateClassesList(const QString &n); //обновляет 2-й combobox при переключении первого, n  - номер класса (1-11)
    void loadPupils ();
    void on_cmdCreateClass_clicked();
    void closeObject();
    void removeObject();
    void on_cmdOpenClass_clicked();
    void on_cbLetter_currentIndexChanged(int index);
    void on_actOpenClass_triggered();
    void on_actCreateClass_triggered();
    void on_lblTitle_customContextMenuRequested(const QPoint &pos);
    void on_cmdAdd_clicked();
    void on_cmdRemove_clicked();
    void buttonCheck();
    void showInfo();

    void on_lstPupils_itemDoubleClicked(QListWidgetItem *item);
    void on_cmdMove_clicked();
};

#endif // MAINWINDOW_H
