#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QErrorMessage>
#include <QDateTime>
#include <QtDebug>
#include <QPushButton>
#include <QToolBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QInputDialog>

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;

//-----------------------------------------------
class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};

class PermissionsTab : public QWidget
{
    Q_OBJECT

public:
    explicit PermissionsTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};

class ApplicationsTab : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicationsTab(const QFileInfo &fileInfo, QWidget *parent = 0);
};

class TabDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TabDialog(const QString &fileName, QWidget *parent = 0);

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
};

//--------------------------------------------------

class DialogOptionsWidget : public QGroupBox
{
public:
    explicit DialogOptionsWidget(QWidget *parent = 0);

    void addCheckBox(const QString &text, int value);
    void addSpacer();
    int value() const;

private:
    typedef QPair<QCheckBox *, int> CheckBoxEntry;
    QVBoxLayout *layout;
    QList<CheckBoxEntry> checkBoxEntries;
};

class OpenDialog : public QWidget
{
    Q_OBJECT

public:
    OpenDialog(QWidget *parent = 0);

private slots:
    void setInteger();
    void setDouble();
    void setItem();
    void setText();
    void setMultiLineText();
    void setColor();
    void setFont();
    void setExistingDirectory();
    void setOpenFileName();
    void setOpenFileNames();
    void setSaveFileName();
    void criticalMessage();
    void informationMessage();
    void questionMessage();
    void warningMessage();
    void errorMessage();

private:
    QLabel *integerLabel;
    QLabel *doubleLabel;
    QLabel *itemLabel;
    QLabel *textLabel;
    QLabel *multiLineTextLabel;
    QLabel *colorLabel;
    QLabel *fontLabel;
    QLabel *directoryLabel;
    QLabel *openFileNameLabel;
    QLabel *openFileNamesLabel;
    QLabel *saveFileNameLabel;
    QLabel *criticalLabel;
    QLabel *informationLabel;
    QLabel *questionLabel;
    QLabel *warningLabel;
    QLabel *errorLabel;
    QErrorMessage *errorMessageDialog;
    DialogOptionsWidget *fileDialogOptionsWidget;
    DialogOptionsWidget *colorDialogOptionsWidget;
    DialogOptionsWidget *fontDialogOptionsWidget;
    QString openFilesPath;
};

#endif // OPENDIALOG_H
