#include "opendialog.h"


TabDialog::TabDialog(const QString &fileName, QWidget *parent)
    : QDialog(parent)
{
    qDebug () << "TabDialog: Ctor IN";

    QFileInfo fileInfo(fileName);

    tabWidget = new QTabWidget;
    tabWidget->addTab(new GeneralTab(fileInfo), tr("General"));
    tabWidget->addTab(new PermissionsTab(fileInfo), tr("Permissions"));
    tabWidget->addTab(new ApplicationsTab(fileInfo), tr("Applications"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Tab Dialog"));

    qDebug () << "TabDialog: Ctor Ok";
}

GeneralTab::GeneralTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QLabel *fileNameLabel = new QLabel(tr("File Name:"));
    QLineEdit *fileNameEdit = new QLineEdit(fileInfo.fileName());

    QLabel *pathLabel = new QLabel(tr("Path:"));
    QLabel *pathValueLabel = new QLabel(fileInfo.absoluteFilePath());
    pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *sizeLabel = new QLabel(tr("Size:"));
    qlonglong size = fileInfo.size()/1024;
    QLabel *sizeValueLabel = new QLabel(tr("%1 K").arg(size));
    sizeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastReadLabel = new QLabel(tr("Last Read:"));
    QLabel *lastReadValueLabel = new QLabel(fileInfo.lastRead().toString());
    lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lastModLabel = new QLabel(tr("Last Modified:"));
    QLabel *lastModValueLabel = new QLabel(fileInfo.lastModified().toString());
    lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameEdit);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathValueLabel);
    mainLayout->addWidget(sizeLabel);
    mainLayout->addWidget(sizeValueLabel);
    mainLayout->addWidget(lastReadLabel);
    mainLayout->addWidget(lastReadValueLabel);
    mainLayout->addWidget(lastModLabel);
    mainLayout->addWidget(lastModValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

PermissionsTab::PermissionsTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *permissionsGroup = new QGroupBox(tr("Permissions"));

    QCheckBox *readable = new QCheckBox(tr("Readable"));
    if (fileInfo.isReadable())
        readable->setChecked(true);

    QCheckBox *writable = new QCheckBox(tr("Writable"));
    if ( fileInfo.isWritable() )
        writable->setChecked(true);

    QCheckBox *executable = new QCheckBox(tr("Executable"));
    if ( fileInfo.isExecutable() )
        executable->setChecked(true);

    QGroupBox *ownerGroup = new QGroupBox(tr("Ownership"));

    QLabel *ownerLabel = new QLabel(tr("Owner"));
    QLabel *ownerValueLabel = new QLabel(fileInfo.owner());
    ownerValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *groupLabel = new QLabel(tr("Group"));
    QLabel *groupValueLabel = new QLabel(fileInfo.group());
    groupValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *permissionsLayout = new QVBoxLayout;
    permissionsLayout->addWidget(readable);
    permissionsLayout->addWidget(writable);
    permissionsLayout->addWidget(executable);
    permissionsGroup->setLayout(permissionsLayout);

    QVBoxLayout *ownerLayout = new QVBoxLayout;
    ownerLayout->addWidget(ownerLabel);
    ownerLayout->addWidget(ownerValueLabel);
    ownerLayout->addWidget(groupLabel);
    ownerLayout->addWidget(groupValueLabel);
    ownerGroup->setLayout(ownerLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(permissionsGroup);
    mainLayout->addWidget(ownerGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

ApplicationsTab::ApplicationsTab(const QFileInfo &fileInfo, QWidget *parent)
    : QWidget(parent)
{
    QLabel *topLabel = new QLabel(tr("Open with:"));

    QListWidget *applicationsListBox = new QListWidget;
    QStringList applications;

    for (int i = 1; i <= 30; ++i)
        applications.append(tr("Application %1").arg(i));
    applicationsListBox->insertItems(0, applications);

    QCheckBox *alwaysCheckBox;

    if (fileInfo.suffix().isEmpty())
        alwaysCheckBox = new QCheckBox(tr("Always use this application to "
            "open this type of file"));
    else
        alwaysCheckBox = new QCheckBox(tr("Always use this application to "
            "open files with the extension '%1'").arg(fileInfo.suffix()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(applicationsListBox);
    layout->addWidget(alwaysCheckBox);
    setLayout(layout);
}

//--------------------------------------------------------------

#define MESSAGE \
    OpenDialog::tr("<p>Message boxes have a caption, a text, " \
               "and any number of buttons, each with standard or custom texts." \
               "<p>Click a button to close the message box. Pressing the Esc button " \
               "will activate the detected escape button (if any).")
#define MESSAGE_DETAILS \
    OpenDialog::tr("If a message box has detailed text, the user can reveal it " \
               "by pressing the Show Details... button.")


DialogOptionsWidget::DialogOptionsWidget(QWidget *parent) :
    QGroupBox(parent) , layout(new QVBoxLayout)
{
    setTitle(OpenDialog::tr("Options"));
    setLayout(layout);
}

void DialogOptionsWidget::addCheckBox(const QString &text, int value)
{
    QCheckBox *checkBox = new QCheckBox(text);
    layout->addWidget(checkBox);
    checkBoxEntries.append(CheckBoxEntry(checkBox, value));
}

void DialogOptionsWidget::addSpacer()
{
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
}

int DialogOptionsWidget::value() const
{
    int result = 0;
    foreach (const CheckBoxEntry &checkboxEntry, checkBoxEntries)
        if (checkboxEntry.first->isChecked())
            result |= checkboxEntry.second;
    return result;
}

OpenDialog::OpenDialog(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QToolBox *toolbox = new QToolBox;
    mainLayout->addWidget(toolbox);

    errorMessageDialog = new QErrorMessage(this);

    int frameStyle = QFrame::Sunken | QFrame::Panel;

    integerLabel = new QLabel;
    integerLabel->setFrameStyle(frameStyle);
    QPushButton *integerButton =
            new QPushButton(tr("QInputDialog::get&Int()"));

    doubleLabel = new QLabel;
    doubleLabel->setFrameStyle(frameStyle);
    QPushButton *doubleButton =
            new QPushButton(tr("QInputDialog::get&Double()"));

    itemLabel = new QLabel;
    itemLabel->setFrameStyle(frameStyle);
    QPushButton *itemButton = new QPushButton(tr("QInputDialog::getIte&m()"));

    textLabel = new QLabel;
    textLabel->setFrameStyle(frameStyle);
    QPushButton *textButton = new QPushButton(tr("QInputDialog::get&Text()"));

    multiLineTextLabel = new QLabel;
    multiLineTextLabel->setFrameStyle(frameStyle);
    QPushButton *multiLineTextButton = new QPushButton(tr("QInputDialog::get&MultiLineText()"));

    colorLabel = new QLabel;
    colorLabel->setFrameStyle(frameStyle);
    QPushButton *colorButton = new QPushButton(tr("QColorDialog::get&Color()"));

    fontLabel = new QLabel;
    fontLabel->setFrameStyle(frameStyle);
    QPushButton *fontButton = new QPushButton(tr("QFontDialog::get&Font()"));

    directoryLabel = new QLabel;
    directoryLabel->setFrameStyle(frameStyle);
    QPushButton *directoryButton =
            new QPushButton(tr("QFileDialog::getE&xistingDirectory()"));

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNameButton =
            new QPushButton(tr("QFileDialog::get&OpenFileName()"));

    openFileNamesLabel = new QLabel;
    openFileNamesLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNamesButton =
            new QPushButton(tr("QFileDialog::&getOpenFileNames()"));

    saveFileNameLabel = new QLabel;
    saveFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *saveFileNameButton =
            new QPushButton(tr("QFileDialog::get&SaveFileName()"));

    criticalLabel = new QLabel;
    criticalLabel->setFrameStyle(frameStyle);
    QPushButton *criticalButton =
            new QPushButton(tr("QMessageBox::critica&l()"));

    informationLabel = new QLabel;
    informationLabel->setFrameStyle(frameStyle);
    QPushButton *informationButton =
            new QPushButton(tr("QMessageBox::i&nformation()"));

    questionLabel = new QLabel;
    questionLabel->setFrameStyle(frameStyle);
    QPushButton *questionButton =
            new QPushButton(tr("QMessageBox::&question()"));

    warningLabel = new QLabel;
    warningLabel->setFrameStyle(frameStyle);
    QPushButton *warningButton = new QPushButton(tr("QMessageBox::&warning()"));

    errorLabel = new QLabel;
    errorLabel->setFrameStyle(frameStyle);
    QPushButton *errorButton =
            new QPushButton(tr("QErrorMessage::showM&essage()"));

    connect(integerButton, SIGNAL(clicked()), this, SLOT(setInteger()));
    connect(doubleButton, SIGNAL(clicked()), this, SLOT(setDouble()));
    connect(itemButton, SIGNAL(clicked()), this, SLOT(setItem()));
    connect(textButton, SIGNAL(clicked()), this, SLOT(setText()));
    connect(multiLineTextButton, SIGNAL(clicked()), this, SLOT(setMultiLineText()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
    connect(fontButton, SIGNAL(clicked()), this, SLOT(setFont()));
    connect(directoryButton, SIGNAL(clicked()),
            this, SLOT(setExistingDirectory()));
    connect(openFileNameButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));
    connect(openFileNamesButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileNames()));
    connect(saveFileNameButton, SIGNAL(clicked()),
            this, SLOT(setSaveFileName()));
    connect(criticalButton, SIGNAL(clicked()), this, SLOT(criticalMessage()));
    connect(informationButton, SIGNAL(clicked()),
            this, SLOT(informationMessage()));
    connect(questionButton, SIGNAL(clicked()), this, SLOT(questionMessage()));
    connect(warningButton, SIGNAL(clicked()), this, SLOT(warningMessage()));
    connect(errorButton, SIGNAL(clicked()), this, SLOT(errorMessage()));

    QWidget *page = new QWidget;
    QGridLayout *layout = new QGridLayout(page);
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(1, 250);
    layout->addWidget(integerButton, 0, 0);
    layout->addWidget(integerLabel, 0, 1);
    layout->addWidget(doubleButton, 1, 0);
    layout->addWidget(doubleLabel, 1, 1);
    layout->addWidget(itemButton, 2, 0);
    layout->addWidget(itemLabel, 2, 1);
    layout->addWidget(textButton, 3, 0);
    layout->addWidget(textLabel, 3, 1);
    layout->addWidget(multiLineTextButton, 4, 0);
    layout->addWidget(multiLineTextLabel, 4, 1);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding), 5, 0);
    toolbox->addItem(page, tr("Input Dialogs"));

    const QString doNotUseNativeDialog = tr("Do not use native dialog");

    page = new QWidget;
    layout = new QGridLayout(page);
    layout->setColumnStretch(1, 1);
    layout->addWidget(colorButton, 0, 0);
    layout->addWidget(colorLabel, 0, 1);
    colorDialogOptionsWidget = new DialogOptionsWidget;
    colorDialogOptionsWidget->addCheckBox(doNotUseNativeDialog, QColorDialog::DontUseNativeDialog);
    colorDialogOptionsWidget->addCheckBox(tr("Show alpha channel") , QColorDialog::ShowAlphaChannel);
    colorDialogOptionsWidget->addCheckBox(tr("No buttons") , QColorDialog::NoButtons);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding), 1, 0);
    layout->addWidget(colorDialogOptionsWidget, 2, 0, 1 ,2);

    toolbox->addItem(page, tr("Color Dialog"));

    page = new QWidget;
    layout = new QGridLayout(page);
    layout->setColumnStretch(1, 1);
    layout->addWidget(fontButton, 0, 0);
    layout->addWidget(fontLabel, 0, 1);
    fontDialogOptionsWidget = new DialogOptionsWidget;
    fontDialogOptionsWidget->addCheckBox(doNotUseNativeDialog, QFontDialog::DontUseNativeDialog);
    fontDialogOptionsWidget->addCheckBox(tr("No buttons") , QFontDialog::NoButtons);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding), 1, 0);
    layout->addWidget(fontDialogOptionsWidget, 2, 0, 1 ,2);
    toolbox->addItem(page, tr("Font Dialog"));

    page = new QWidget;
    layout = new QGridLayout(page);
    layout->setColumnStretch(1, 1);
    layout->addWidget(directoryButton, 0, 0);
    layout->addWidget(directoryLabel, 0, 1);
    layout->addWidget(openFileNameButton, 1, 0);
    layout->addWidget(openFileNameLabel, 1, 1);
    layout->addWidget(openFileNamesButton, 2, 0);
    layout->addWidget(openFileNamesLabel, 2, 1);
    layout->addWidget(saveFileNameButton, 3, 0);
    layout->addWidget(saveFileNameLabel, 3, 1);
    fileDialogOptionsWidget = new DialogOptionsWidget;
    fileDialogOptionsWidget->addCheckBox(doNotUseNativeDialog, QFileDialog::DontUseNativeDialog);
    fileDialogOptionsWidget->addCheckBox(tr("Show directories only"), QFileDialog::ShowDirsOnly);
    fileDialogOptionsWidget->addCheckBox(tr("Do not resolve symlinks"), QFileDialog::DontResolveSymlinks);
    fileDialogOptionsWidget->addCheckBox(tr("Do not confirm overwrite"), QFileDialog::DontConfirmOverwrite);
    fileDialogOptionsWidget->addCheckBox(tr("Do not use sheet"), QFileDialog::DontUseSheet);
    fileDialogOptionsWidget->addCheckBox(tr("Readonly"), QFileDialog::ReadOnly);
    fileDialogOptionsWidget->addCheckBox(tr("Hide name filter details"), QFileDialog::HideNameFilterDetails);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding), 4, 0);
    layout->addWidget(fileDialogOptionsWidget, 5, 0, 1 ,2);
    toolbox->addItem(page, tr("File Dialogs"));

    page = new QWidget;
    layout = new QGridLayout(page);
    layout->setColumnStretch(1, 1);
    layout->addWidget(criticalButton, 0, 0);
    layout->addWidget(criticalLabel, 0, 1);
    layout->addWidget(informationButton, 1, 0);
    layout->addWidget(informationLabel, 1, 1);
    layout->addWidget(questionButton, 2, 0);
    layout->addWidget(questionLabel, 2, 1);
    layout->addWidget(warningButton, 3, 0);
    layout->addWidget(warningLabel, 3, 1);
    layout->addWidget(errorButton, 4, 0);
    layout->addWidget(errorLabel, 4, 1);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding), 5, 0);
    toolbox->addItem(page, tr("Message Boxes"));

    setWindowTitle(tr("Standard Dialogs"));
}

void OpenDialog::setInteger()
{
    bool ok;
    int i = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                 tr("Percentage:"), 25, 0, 100, 1, &ok);
    if (ok)
        integerLabel->setText(tr("%1%").arg(i));
}

void OpenDialog::setDouble()
{
    bool ok;
    double d = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                       tr("Amount:"), 37.56, -10000, 10000, 2, &ok);
    if (ok)
        doubleLabel->setText(QString("$%1").arg(d));
}

void OpenDialog::setItem()
{
    QStringList items;
    items << tr("Spring") << tr("Summer") << tr("Fall") << tr("Winter");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                         tr("Season:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
        itemLabel->setText(item);
}

void OpenDialog::setText()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        textLabel->setText(text);
}

void OpenDialog::setMultiLineText()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, tr("QInputDialog::getMultiLineText()"),
                                                  tr("Address:"), "John Doe\nFreedom Street", &ok);
    if (ok && !text.isEmpty())
        multiLineTextLabel->setText(text);
}

void OpenDialog::setColor()
{
    const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color", options);

    if (color.isValid()) {
        colorLabel->setText(color.name());
        colorLabel->setPalette(QPalette(color));
        colorLabel->setAutoFillBackground(true);
    }
}

void OpenDialog::setFont()
{
    const QFontDialog::FontDialogOptions options = QFlag(fontDialogOptionsWidget->value());
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(fontLabel->text()), this, "Select Font", options);
    if (ok) {
        fontLabel->setText(font.key());
        fontLabel->setFont(font);
    }
}

void OpenDialog::setExistingDirectory()
{
    QFileDialog::Options options = QFlag(fileDialogOptionsWidget->value());
    options |= QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                tr("QFileDialog::getExistingDirectory()"),
                                directoryLabel->text(),
                                options);
    if (!directory.isEmpty())
        directoryLabel->setText(directory);
}

void OpenDialog::setOpenFileName()
{
    const QFileDialog::Options options = QFlag(fileDialogOptionsWidget->value());
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                openFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        openFileNameLabel->setText(fileName);
}

void OpenDialog::setOpenFileNames()
{
    const QFileDialog::Options options = QFlag(fileDialogOptionsWidget->value());
    QString selectedFilter;
    QStringList files = QFileDialog::getOpenFileNames(
                                this, tr("QFileDialog::getOpenFileNames()"),
                                openFilesPath,
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (files.count()) {
        openFilesPath = files[0];
        openFileNamesLabel->setText(QString("[%1]").arg(files.join(", ")));
    }
}

void OpenDialog::setSaveFileName()
{
    const QFileDialog::Options options = QFlag(fileDialogOptionsWidget->value());
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("QFileDialog::getSaveFileName()"),
                                saveFileNameLabel->text(),
                                tr("All Files (*);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        saveFileNameLabel->setText(fileName);
}

void OpenDialog::criticalMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
                                    MESSAGE,
                                    QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
    if (reply == QMessageBox::Abort)
        criticalLabel->setText(tr("Abort"));
    else if (reply == QMessageBox::Retry)
        criticalLabel->setText(tr("Retry"));
    else
        criticalLabel->setText(tr("Ignore"));
}

void OpenDialog::informationMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("QMessageBox::information()"), MESSAGE);
    if (reply == QMessageBox::Ok)
        informationLabel->setText(tr("OK"));
    else
        informationLabel->setText(tr("Escape"));
}

void OpenDialog::questionMessage()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("QMessageBox::question()"),
                                    MESSAGE,
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
        questionLabel->setText(tr("Yes"));
    else if (reply == QMessageBox::No)
        questionLabel->setText(tr("No"));
    else
        questionLabel->setText(tr("Cancel"));
}

void OpenDialog::warningMessage()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("QMessageBox::warning()"),
                       MESSAGE, 0, this);
    msgBox.setDetailedText(MESSAGE_DETAILS);
    msgBox.addButton(tr("Save &Again"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("&Continue"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        warningLabel->setText(tr("Save Again"));
    else
        warningLabel->setText(tr("Continue"));

}

void OpenDialog::errorMessage()
{
    errorMessageDialog->showMessage(
            tr("This dialog shows and remembers error messages. "
               "If the checkbox is checked (as it is by default), "
               "the shown message will be shown again, "
               "but if the user unchecks the box the message "
               "will not appear again if QErrorMessage::showMessage() "
               "is called with the same message."));
    errorLabel->setText(tr("If the box is unchecked, the message "
                           "won't appear again."));
}
