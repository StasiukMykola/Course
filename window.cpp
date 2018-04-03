#include "window.h"
#include "ui_window.h"



window::window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);

    ui->table_name->setVisible(false);
    ui->push_adm->setVisible(false);
    ui->error->setVisible(false);
    ui->del->setVisible(false);

    show_play();

    style();


    QSqlDatabase dbs = QSqlDatabase::addDatabase("QSQLITE");
    dbs.setDatabaseName("coursedb.db3");
    dbs.open();



    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_cb_change()));
    connect(ui->btn_del,SIGNAL(clicked(bool)),this,SLOT(on_btn_del_clicked()));
    connect(ui->push_show,SIGNAL(clicked(bool)),this,SLOT(on_show_all_clicked()));

    connect(ui->push_add,SIGNAL(clicked(bool)),this,SLOT(add_play()));
    connect(ui->push_adm,SIGNAL(clicked(bool)),this,SLOT(add_tabl()));

    connect(ui->push_del,SIGNAL(clicked(bool)),this,SLOT(del_play()));
    connect(ui->del,SIGNAL(clicked(bool)),this,SLOT(del_tabl()));

    connect(ui->push_clear,SIGNAL(clicked(bool)),this,SLOT(clear_play()));


    QPixmap mypix("image.jpg");
    ui->image->setPixmap(mypix);


    m_playListModel = new QStandardItemModel(this);
    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->hideColumn(2);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(70);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);


    connect(ui->push_skip_prev, &QPushButton::clicked, m_playlist, &QMediaPlaylist::previous);
    connect(ui->push_skip_next, &QPushButton::clicked, m_playlist, &QMediaPlaylist::next);
    connect(ui->push_play, &QPushButton::clicked, m_player, &QMediaPlayer::play);
    connect(ui->push_pause, &QPushButton::clicked, m_player, &QMediaPlayer::pause);
    connect(ui->push_stop, &QPushButton::clicked, m_player, &QMediaPlayer::stop);


    connect(ui->playlistView, &QTableView::clicked, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());
    });


    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->current_track->setText(m_playListModel->data(m_playListModel->index(index, 0)).toString());
    });

    dbs.close();
}

window::~window()
{
    delete ui;
    delete m_playListModel;
    delete m_playlist;
    delete m_player;
}

void window::on_btn_add_clicked()
{
    QString tbl_name = ui->comboBox_2->currentText();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    QStringList files = QFileDialog::getOpenFileNames(this,tr("Open files"),QString(),tr("Audio Files (*.mp3)"));
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        query.exec("INSERT INTO "+tbl_name+"(song_name,song_path) VALUES ('"+QDir(filePath).dirName()+"','"+filePath+"');");
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(filePath));
    }
    db.close();
}

void window::on_cb_change(){
    genre=ui->comboBox->currentText();
    m_playListModel->clear();
    QString tbl_name = ui->comboBox_2->currentText();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    query.exec("SELECT * from "+tbl_name+" where genre='"+genre+"'");
    while(query.next()){
        song_name=query.value(1).toString();
        song_path=query.value(2).toString();
        genre=query.value(3).toString();
        duration=query.value(4).toString();
        autor=query.value(5).toString();

        QList<QStandardItem *> items;
        items.append(new QStandardItem(song_name));
        items.append(new QStandardItem(song_path));
        items.append(new QStandardItem(genre));
        items.append(new QStandardItem(duration));
        items.append(new QStandardItem(autor));
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(song_path));
    }
    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->hideColumn(2);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    db.close();
}

void window::on_btn_del_clicked(){
    QString name=ui->current_track->text();
    ui->current_track->setText("name");
    QString tbl_name = ui->comboBox_2->currentText();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    query.exec("DELETE FROM "+tbl_name+" where song_name='"+name+"'");
    m_playListModel->clear();
    on_cb_change();
    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->hideColumn(2);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    db.close();
}


void window::on_btn_search_clicked(){
    QString tbl_name = ui->comboBox_2->currentText();
    QString name=ui->search->toPlainText();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    m_playListModel->clear();
    query.exec("SELECT * FROM "+tbl_name+" WHERE song_name = '"+name+"'");

    while(query.next()){
        song_name=query.value(1).toString();
        song_path=query.value(2).toString();
        genre=query.value(3).toString();
        duration=query.value(4).toString();
        autor=query.value(5).toString();

        QList<QStandardItem *> items;
        items.append(new QStandardItem(song_name));
        items.append(new QStandardItem(song_path));
        items.append(new QStandardItem(genre));
        items.append(new QStandardItem(duration));
        items.append(new QStandardItem(autor));
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(song_path));
    }

    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->hideColumn(2);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    db.close();
}


void window::on_show_all_clicked(){

    QString tbl_name = ui->comboBox_2->currentText();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    m_playListModel->clear();
    query.exec("SELECT * FROM "+tbl_name);

    while(query.next()){
        song_name=query.value(1).toString();
        song_path=query.value(2).toString();
        genre=query.value(3).toString();
        duration=query.value(4).toString();
        autor=query.value(5).toString();

        QList<QStandardItem *> items;
        items.append(new QStandardItem(song_name));
        items.append(new QStandardItem(song_path));
        items.append(new QStandardItem(genre));
        items.append(new QStandardItem(duration));
        items.append(new QStandardItem(autor));
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(song_path));
    }

    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    db.close();
}


void window::add_play(){
   ui->table_name->setVisible(true);
   ui->push_adm->setVisible(true);
   ui->del->setVisible(false);
}

void window::add_tabl(){
      QString tbl_nm =ui->table_name->toPlainText();
      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("coursedb.db3");
      db.open();
      QSqlQuery query;
      m_playListModel->clear();
      if(tbl_nm==""){
          ui->error->setVisible(true);
      }
      else if(tbl_nm!=""){
      ui->comboBox_2->addItem(tbl_nm);
      query.exec("CREATE TABLE "+tbl_nm+"(id INTEGER PRIMARY KEY AUTOINCREMENT, song_name text,song_path text, genre text, duration text, autor text)");
      ui->table_name->setVisible(false);
      ui->push_adm->setVisible(false);
      ui->error->setVisible(false);
      ui->table_name->setText("");
      }
}

void window::show_play(){
    ui->comboBox_2->clear();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
     ui->comboBox_2->addItem(tr("Плейлисти"));
    QSqlQuery query;
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND rootpage != '3'");
    while(query.next()){
        ui->comboBox_2->addItem(query.value(0).toString());
    }
}

void window::del_play(){
   ui->table_name->setVisible(true);
   ui->del->setVisible(true);
   ui->push_adm->setVisible(false);
}

void window::del_tabl(){
      QString tbl_nm =ui->table_name->toPlainText();
      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("coursedb.db3");
      db.open();
      QSqlQuery query;
      m_playListModel->clear();
      if(tbl_nm==""){
          ui->error->setVisible(true);
      }
      else if(tbl_nm!=""){
      ui->comboBox_2->addItem(tbl_nm);
      query.exec("DROP TABLE "+tbl_nm);
      ui->table_name->setVisible(false);
      ui->del->setVisible(false);
      ui->error->setVisible(false);
      ui->table_name->setText("");
      }
      show_play();
}

void window::clear_play(){
    QString tbl_nm;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coursedb.db3");
    db.open();
    QSqlQuery query;
    tbl_nm = ui->comboBox_2->currentText();
    query.exec("DELETE FROM "+tbl_nm);

    query.exec("SELECT * FROM "+tbl_nm);

    while(query.next()){
        song_name=query.value(1).toString();
        song_path=query.value(2).toString();
        genre=query.value(3).toString();
        duration=query.value(4).toString();
        autor=query.value(5).toString();

        QList<QStandardItem *> items;
        items.append(new QStandardItem(song_name));
        items.append(new QStandardItem(song_path));
        items.append(new QStandardItem(genre));
        items.append(new QStandardItem(duration));
        items.append(new QStandardItem(autor));
        m_playListModel->appendRow(items);
        m_playlist->addMedia(QUrl(song_path));
    }

    ui->playlistView->setModel(m_playListModel);
    m_playListModel->setHorizontalHeaderLabels(QStringList()  << tr("Назва")
                                                            << tr("Шлях")
                                               <<tr("Жанр")
                                               <<tr("Тривалість")
                                               <<tr("Виконавець"));
    ui->playlistView->hideColumn(1);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setStretchLastSection(true);

    db.close();
}


























