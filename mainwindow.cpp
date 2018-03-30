#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QSettings>
#include <QWebEngineFullScreenRequest>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineProfile>
#include "urlrequestinterceptor.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)

{
  QWebEngineSettings::globalSettings()->setAttribute(
      QWebEngineSettings::PluginsEnabled, true);
  appSettings = new QSettings("Qtwebflix", "Save State", this);
  ui->setupUi(this);
  readSettings();
  webview = new QWebEngineView;
  //RequestInterceptor interceptor;
  ui->horizontalLayout->addWidget(webview);
  UrlRequestInterceptor interceptor;

  if (appSettings->value("site").toString() == "") {
    webview->setUrl(QUrl(QStringLiteral("http://netflix.com")));

  } else {
    webview->setUrl(QUrl(appSettings->value("site").toString()));
  }
  webview->settings()->setAttribute(
      QWebEngineSettings::FullScreenSupportEnabled, true);

  // connect handler for fullscreen press on video
  connect(webview->page(), &QWebEnginePage::fullScreenRequested, this,
          &MainWindow::fullScreenRequested);
//webview->page()->profile()->setRequestInterceptor(&interceptor);
  this->m_interceptor = new UrlRequestInterceptor;
  this->webview->page()->profile()->setRequestInterceptor(this->m_interceptor);

//Check if user is using arm processor(Raspberry pi)
  QString UserAgent = this->webview->page()->profile()->httpUserAgent();
  qDebug()<< UserAgent;
  qDebug()<< UserAgent.contains("Linux arm");
  if ( UserAgent.contains("Linux arm") )
  {
      qDebug()<<"Changing user agent for raspberry pi users";
      QString UserAgent = "Mozilla/5.0 (X11; CrOS armv7l 6946.86.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.91 Safari/537.36";
      this->webview->page()->profile()->setHttpUserAgent(UserAgent);
  }




  // key short cuts
  keyF11 = new QShortcut(this); // Initialize the object
  keyF11->setKey(Qt::Key_F11);  // Set the key code
  // connect handler to keypress
  connect(keyF11, SIGNAL(activated()), this, SLOT(slotShortcutF11()));
  keyCtrlQ = new QShortcut(this);         // Initialize the object
  keyCtrlQ->setKey(Qt::CTRL + Qt::Key_Q); // Set the key code
  // connect handler to keypress
  connect(keyCtrlQ, SIGNAL(activated()), this, SLOT(slotShortcutCtrlQ()));

  // Window size settings
  QSettings settings;
  restoreState(settings.value("mainWindowState").toByteArray());

  webview->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(webview, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(ShowContextMenu(const QPoint &)));

}

MainWindow::~MainWindow() { delete ui; }

// Slot handler of F11
void MainWindow::slotShortcutF11() {
  /* This handler will make switching applications in full screen mode
   * and back to normal window mode
   * */
  if (this->isFullScreen()) {
    this->showNormal();
  } else {
    this->showFullScreen();
  }
}

// Slot handler for Ctrl + Q
void MainWindow::slotShortcutCtrlQ() {
  writeSettings();
  QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *) {
  // This will be called whenever this window is closed.
  writeSettings();
}

void MainWindow::writeSettings() {
  // Write the values to disk in categories.
  appSettings->setValue("state/mainWindowState", saveState());
  appSettings->setValue("geometry/mainWindowGeometry", saveGeometry());
  QString site = webview->url().toString();
  appSettings->setValue("site", site);
  qDebug() << " write settings:" << site;
}

void MainWindow::restore() {

  QByteArray stateData =
      appSettings->value("state/mainWindowState").toByteArray();

  QByteArray geometryData =
      appSettings->value("geometry/mainWindowGeometry").toByteArray();

  restoreState(stateData);
  restoreGeometry(geometryData);
}

void MainWindow::readSettings() { restore(); }

void MainWindow::fullScreenRequested(QWebEngineFullScreenRequest request) {

  // fullscreen on video players

  if (request.toggleOn()) {
    this->showFullScreen();
  } else {
    this->showNormal();
  }
  request.accept();
}

void MainWindow::ShowContextMenu(const QPoint &pos) // this is a slot
{
  QPoint globalPos = webview->mapToGlobal(pos);

  QMenu myMenu;
  myMenu.addAction("Amazon Prime");
  myMenu.addAction("Netflix");
  myMenu.addAction("Hulu");
  myMenu.addAction("CrunchyRoll");
  myMenu.addAction("HBO");
  myMenu.addAction("hbogo.pl");

  QAction *selectedItem = myMenu.exec(globalPos);
  if (selectedItem == NULL) {
    return;
  } else if (selectedItem->text() == "Amazon Prime") {
    webview->setUrl(QUrl(QStringLiteral(
        "https://www.amazon.com/Amazon-Video/b/?&node=2858778011")));
  }

  else if (selectedItem->text() == "Hulu") {
    webview->setUrl(QUrl(QStringLiteral("https://hulu.com")));
  }

  else if (selectedItem->text() == "CrunchyRoll") {
    webview->setUrl(QUrl(QStringLiteral("https://crunchyroll.com")));
  }

  else if (selectedItem->text() == "HBO") {
    webview->setUrl(QUrl(QStringLiteral("https://hbo.com")));
  }
  
  else if (selectedItem->text() == "hbogo.pl") {
    QString UserAgent = "Mozilla/5.0 (Windows NT 6.3; Trident/7.0; rv 11.0) like Gecko";
    this->webview->page()->profile()->setHttpUserAgent(UserAgent);
    webview->setUrl(QUrl(QStringLiteral("https://hbogo.pl")));
  }

  else if (selectedItem->text() == "Netflix") {
    //webview->PagesetUrl(QUrl(QStringLiteral("https://netflix.com")));
    webview->setUrl(QUrl(QStringLiteral("https://netflix.com")));

  }

  else {
    // nothing was chosen
  }
}
