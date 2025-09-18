#include "slackDialog.h"

#include "global.h"
#include "textDialog.h"

#include <QCloseEvent>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QVBoxLayout>

#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QToolBar>

// class slackDialog
// Constructor===============================================================================================
// Displays a dialog to configure Slack webhook notifications
slackDialog::slackDialog(QNetworkAccessManager *nm, QDialog *parent)
    : QDialog(parent) {
  setupUI();
  fillWindow();
  this->nm = nm;

  // connect button SLOTs
  connect(pushButton_ok, SIGNAL(clicked()), this, SLOT(okay()));
  connect(pushButton_cancel, SIGNAL(clicked()), this, SLOT(cancel()));
  connect(pushButton_test, SIGNAL(clicked()), this, SLOT(slackTest()));

  GoOn = 0;
}

// setupUI
// Create the dialog UI programmatically
void slackDialog::setupUI() {
  setWindowTitle("luckyBackup - Slack");
  setWindowIcon(QIcon(":/luckyPrefix/luckybackup_96.png"));
  setFixedSize(450, 200);
  setModal(true);

  // Main layout
  QGridLayout *mainLayout = new QGridLayout(this);

  // Header layout with icon and title
  QHBoxLayout *headerLayout = new QHBoxLayout();

  // Slack icon
  label_icon = new QLabel();
  QPixmap slackPixmap(":/luckyPrefix/slack.png");
  if (!slackPixmap.isNull()) {
    label_icon->setPixmap(slackPixmap);
  } else {
    label_icon->setText("Slack");
  }
  headerLayout->addWidget(label_icon);

  // Title label
  label_message = new QLabel("Configure Slack webhook notifications");
  QFont boldFont;
  boldFont.setPointSize(10);
  boldFont.setBold(true);
  label_message->setFont(boldFont);
  headerLayout->addWidget(label_message);
  headerLayout->addStretch();

  mainLayout->addLayout(headerLayout, 0, 0, 1, 3);

  // Webhook URL input
  QLabel *urlLabel = new QLabel("Webhook URL:");
  mainLayout->addWidget(urlLabel, 1, 0);

  lineEdit_webhookUrl = new QLineEdit();
  lineEdit_webhookUrl->setPlaceholderText(
      "https://hooks.slack.com/services/...");
  mainLayout->addWidget(lineEdit_webhookUrl, 1, 1, 1, 2);

  // Spacer
  mainLayout->addItem(
      new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), 2,
      0, 1, 3);

  // Bottom buttons layout
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  // Test button (left)
  pushButton_test = new QPushButton("Test");
  pushButton_test->setMinimumWidth(80);
  buttonLayout->addWidget(pushButton_test);

  // Spacer to push OK/Cancel to the right
  buttonLayout->addStretch();

  // OK and Cancel buttons (right)
  pushButton_ok = new QPushButton("OK");
  pushButton_ok->setMinimumWidth(80);
  pushButton_ok->setDefault(true);
  buttonLayout->addWidget(pushButton_ok);

  pushButton_cancel = new QPushButton("Cancel");
  pushButton_cancel->setMinimumWidth(80);
  buttonLayout->addWidget(pushButton_cancel);

  mainLayout->addLayout(buttonLayout, 3, 0, 1, 3);

  // Set tab order
  setTabOrder(lineEdit_webhookUrl, pushButton_test);
  setTabOrder(pushButton_test, pushButton_ok);
  setTabOrder(pushButton_ok, pushButton_cancel);
}

// SLOTS-------------------------------------------------------------------------------------
// --------------------------------okay
// pressed------------------------------------------------
void slackDialog::okay() {
  GoOn = 0; // ok pressed -> Everything good
  fillVariables();

  if (!checkFields()) {
    textDialog textdialogW("QtWarning", warningMessage, this);
    textdialogW.exec();
    return; // do not proceed if something is wrong with the fields
  }

  close(); // emit a QCloseEvent
}

// --------------------------------cancel
// pressed------------------------------------------------
void slackDialog::cancel() {
  GoOn = 1; // cancel pressed
  close();  // emit a QCloseEvent
}

// --------------------------------slackTest------------------------------------------------
void slackDialog::slackTest() {
  fillVariables();

  if (!checkFields()) {
    textDialog textdialogW("QtWarning", warningMessage, this);
    textdialogW.exec();
    return;
  }

  QNetworkRequest req{QUrl(slackWebhookUrl)};
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonDocument doc(QJsonObject{{"text", "Hello from Qt!"}});
  QNetworkReply *reply = this->nm->post(req, doc.toJson());

  connect(reply, &QNetworkReply::finished, this, [reply]() {
    const int status =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP status:" << status;

    if (reply->error() == QNetworkReply::NoError) {
      qDebug() << "Body:" << reply->readAll();
    } else {
      qDebug() << "Error:" << reply->error() << reply->errorString();
    }
    reply->deleteLater();
  });

  textDialog testDialog("QtInformation", "Test message sent to Slack webhook!",
                        this);
  testDialog.exec();
}

// -------------------------------- closeEvent emitted
// --------------------------------
void slackDialog::closeEvent(QCloseEvent *event) { event->accept(); }

// fillWindow
// Fill gui fields from variables
void slackDialog::fillWindow() {
  lineEdit_webhookUrl->setText(slackWebhookUrl);
}

// fillVariables
// Fill variables from gui fields
void slackDialog::fillVariables() {
  slackWebhookUrl = lineEdit_webhookUrl->text();
}

// checkFields
// Check if all fields are properly filled
bool slackDialog::checkFields() {
  warningMessage = "";

  if (slackWebhookUrl.isEmpty()) {
    warningMessage = "Please enter a Slack webhook URL.";
    return false;
  }

  if (!slackWebhookUrl.startsWith("https://hooks.slack.com/")) {
    warningMessage = "Please enter a valid Slack webhook URL starting with "
                     "'https://hooks.slack.com/'.";
    return false;
  }

  return true;
}

// getGoOn
// Returns the exit status
int slackDialog::getGoOn() { return GoOn; }
