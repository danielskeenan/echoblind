/**
 * @file FileSelectorWidget.h
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#ifndef FILESELECTORWIDGET_H
#define FILESELECTORWIDGET_H

#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace echoblind
{

    /**
     * A QLineEdit for a file path with an associated "Browse" QPushButton.
     */
    class FileSelectorWidget : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged USER true)
        Q_PROPERTY(QFileDialog::AcceptMode acceptMode READ acceptMode WRITE setAcceptMode)
        Q_PROPERTY(QFileDialog::FileMode fileMode READ fileMode WRITE setFileMode)
    public:
        explicit FileSelectorWidget(QWidget* parent = nullptr);
        [[nodiscard]] QString path() const;
        void setPath(QString path);
        [[nodiscard]] QFileDialog::AcceptMode acceptMode() const { return acceptMode_; }
        void setAcceptMode(QFileDialog::AcceptMode acceptMode) { acceptMode_ = acceptMode; }
        [[nodiscard]] QFileDialog::FileMode fileMode() const { return fileMode_; }
        void setFileMode(QFileDialog::FileMode fileMode) { fileMode_ = fileMode; }
        [[nodiscard]] const QStringList& nameFilters() const { return nameFilters_; }
        void setNameFilters(const QStringList& nameFilters)
        {
            nameFilters_ = nameFilters;
            mimeTypeFilters_.clear();
        }
        [[nodiscard]] QStringList mimeTypeFilters() const { return mimeTypeFilters_; }
        void setMimeTypeFilters(const QStringList& mimeTypeFilters)
        {
            mimeTypeFilters_ = mimeTypeFilters;
            nameFilters_.clear();
        }

    Q_SIGNALS:
        void pathChanged(QString path);

    private:
        QLineEdit* lineEdit_;
        QPushButton* browseButton_;
        QFileDialog::AcceptMode acceptMode_ = QFileDialog::AcceptOpen;
        QFileDialog::FileMode fileMode_ = QFileDialog::ExistingFile;
        QStringList nameFilters_;
        QStringList mimeTypeFilters_;

    private Q_SLOTS:
        void browse();
        void lineEditFinished();
    };

} // namespace echoblind

#endif // FILESELECTORWIDGET_H
