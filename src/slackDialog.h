#ifndef SLACKDIALOG_H
#define SLACKDIALOG_H

#include <QDialog>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

//==============================================================================================================
// Declaration of slackDialog class
class slackDialog : public QDialog {
  Q_OBJECT

public:
  slackDialog(QNetworkAccessManager *nm, QDialog *parent = 0);
  int getGoOn();

private slots:
  void okay();      // Okay pressed
  void cancel();    // Cancel pressed
  void slackTest(); // Send a test Slack message using current webhook URL

private:
  // UI elements
  QLineEdit *lineEdit_webhookUrl;
  QPushButton *pushButton_test;
  QPushButton *pushButton_ok;
  QPushButton *pushButton_cancel;
  QLabel *label_icon;
  QLabel *label_message;

  bool checkFields(); // Function to check if webhook URL field is valid
  void setupUI();     // Setup the dialog UI programmatically

  int GoOn;               // return an integer according to exit status
  void fillWindow();      // Fill gui fields from variables
  void fillVariables();   // Fill relevant variables from gui fields
  QString warningMessage; // Warning message to display if the field is not ok

  QNetworkAccessManager *nm;
protected:
  void closeEvent(QCloseEvent *);
};

#endif

// end of slackDialog.h
// ---------------------------------------------------------------------------
