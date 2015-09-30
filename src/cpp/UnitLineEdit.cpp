#include "UnitLineEdit.h"

UnitLineEdit::UnitLineEdit(QString UnitName) {
    Unit = new QLabel(UnitName);
    Text = new QLineEdit;

    addWidget(Text, 1);
    addWidget(Unit);

    setSpacing(5);
}

QString UnitLineEdit::text() {
    return Text->text();
}

void UnitLineEdit::setText(QString NewText) {
    Text->setText(NewText);
}

QString UnitLineEdit::GetUnit() {
    return Unit->text();
}

void UnitLineEdit::SetUnit(QString NewUnit) {
    Unit->setText(NewUnit);
}

void UnitLineEdit::show() {
    Unit->show();
    Text->show();
}

void UnitLineEdit::hide() {
    Unit->hide();
    Text->hide();
}

bool UnitLineEdit::isHidden() {
    return Text->isHidden();
}

void UnitLineEdit::setStyleSheet(const QString &T) {
    Text->setStyleSheet(T);
}

void UnitLineEdit::setValidator(QValidator *V) {
    Text->setValidator(V);
}

void UnitLineEdit::setSizePolicy(QSizePolicy::Policy horizontal,
                                 QSizePolicy::Policy vertical) {
    Text->setSizePolicy(horizontal, vertical);
}
