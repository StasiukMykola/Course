#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QDir>

namespace Ui {
class window;
}

class window : public QWidget
{
    Q_OBJECT

public:
    explicit window(QWidget *parent = 0);
    void set_db();
    ~window();

    QString genre;
    QString song_name;
    QString song_path;
    QString duration;
    QString autor;

private:
    Ui::window *ui;
    QStandardItemModel  *m_playListModel;
    QMediaPlayer        *m_player;
    QMediaPlaylist      *m_playlist;

     void show_play();


private slots:
    void on_btn_add_clicked();
    void on_btn_del_clicked();

    void on_cb_change();

    void on_btn_search_clicked();
    void on_show_all_clicked();

    void add_play();
    void add_tabl();

    void del_play();
    void del_tabl();

    void clear_play();

};

#endif // WINDOW_H


