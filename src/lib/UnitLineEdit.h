#ifndef UNITYLINEEDIT_H
#define UNITYLINEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QString>

class UnitLineEdit : public QHBoxLayout {
  public:
    UnitLineEdit(QString);
    QLineEdit *Text;
    QLabel *Unit;

    QString text();
    void setText(QString);
    QString GetUnit();
    void SetUnit(QString);

    void show();
    void hide();
    bool isHidden();
    void setStyleSheet(const QString &);
    void setValidator(QValidator *);
    void setSizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);
};

#endif // UNITYLINEEDIT_H
