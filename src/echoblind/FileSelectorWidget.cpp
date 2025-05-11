/**
 * @file FileSelectorWidget.cpp
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#include "FileSelectorWidget.h"
#include <QFileDialog>
#include <QHBoxLayout>

#include "Settings.h"

namespace echoblind
{
    FileSelectorWidget::FileSelectorWidget(QWidget* parent) : QWidget(parent), nameFilters_({tr("Any files (*)")})
    {
        auto* layout = new QHBoxLayout(this);

        lineEdit_ = new QLineEdit(this);
        connect(lineEdit_, &QLineEdit::editingFinished, this, &FileSelectorWidget::lineEditFinished);
        layout->addWidget(lineEdit_);
        browseButton_ = new QPushButton(tr("Browse..."), this);
        connect(browseButton_, &QPushButton::clicked, this, &FileSelectorWidget::browse);
        layout->addWidget(browseButton_);
    }

    QString FileSelectorWidget::path() const { return lineEdit_->text(); }

    void FileSelectorWidget::setPath(QString path)
    {
        lineEdit_->setText(path);
        Q_EMIT(pathChanged(path));
    }

    void FileSelectorWidget::browse()
    {
        QFileDialog dialog(this);
        dialog.setAcceptMode(acceptMode_);
        dialog.setFileMode(fileMode_);
        dialog.setNameFilters(nameFilters_);
        if (!path().isEmpty())
        {
            dialog.selectFile(path());
        }
        else
        {
            dialog.setDirectory(settings::getLastFileDialogPath());
        }
        dialog.exec();

        if (dialog.result() == QDialog::Accepted)
        {
            const QFileInfo fileInfo(dialog.selectedFiles().at(0));
            setPath(fileInfo.absoluteFilePath());
            if (fileMode_ == QFileDialog::Directory)
            {
                settings::setLastFileDialogPath(fileInfo.absoluteFilePath());
            }
            else
            {
                settings::setLastFileDialogPath(fileInfo.dir().absolutePath());
            }
        }
    }

    void FileSelectorWidget::lineEditFinished() { Q_EMIT(pathChanged(path())); }

} // namespace echoblind
