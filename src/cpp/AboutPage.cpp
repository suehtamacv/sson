#include "AboutPage.h"
#include <QTextBlock>
#include "AuxiliarMain.h"

AboutPage::AboutPage(QWidget *parent) :  QDialog(parent) {
    setWindowTitle(tr("About SSON"));
    setMinimumSize(600, 300);
    setMaximumSize(600, 300);

    set_AboutPage();
}

void AboutPage::AboutPage_Close() {
    this->close();
}

void AboutPage::set_AboutPage() {

    QImage *SSON_Logo = new QImage(QString::fromStdString(
                                       AuxiliarMain::Installed_Path) + "Figures/sson.png");
    QLabel *SSON_LogoContainer = new QLabel();
    SSON_LogoContainer->setPixmap(QPixmap::fromImage(SSON_Logo->scaled(300, 300,
                                  Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    SSON_Title = new QLabel();
    SSON_Title->setWordWrap(true);
    QFont Font = SSON_Title->font();
    SSON_Title->setText("SSON");
    Font.setBold(true);
    Font.setPointSize(36);
    SSON_Title->setFont(Font);

    SSON_Subtitle = new QLabel();
    SSON_Subtitle->setText(tr("Simulator for Slice Optical Networks"));
    SSON_Subtitle->setWordWrap(true);
    Font = SSON_Subtitle->font();
    Font.setItalic(true);
    Font.setPointSize(12);
    SSON_Subtitle->setFont(Font);

    SSON_Text = new QLabel();
    SSON_Text->setTextFormat(Qt::RichText);
    SSON_Text->setWordWrap(true);
    SSON_Text->setOpenExternalLinks(true);
    SSON_Text->setText(tr("SSON is developed by the <a href=\"http://suehtamacv.github.io/eons-ufcg/\">Radiometry Research Group</a>, under the <a href=\"http://suehtamacv.github.io/eons-ufcg/linhas/modelagem/\">Computational Modeling of Electromagnetic Effects</a> line of research."));

    SSON_Text2 = new QLabel();
    SSON_Text2->setTextFormat(Qt::RichText);
    SSON_Text2->setWordWrap(true);
    SSON_Text2->setOpenExternalLinks(true);
    SSON_Text2->setText(tr("More information about SSON can be found <a href=\"http://suehtamacv.github.io/eons-ufcg/linhas/modelagem/eons.html\">here</a>."));

    QVBoxLayout *VLayout1 = new QVBoxLayout();
    VLayout1->addWidget(SSON_Title);
    VLayout1->addWidget(SSON_Subtitle);
    VLayout1->addStretch();
    VLayout1->addWidget(SSON_Text);
    VLayout1->addStretch();
    VLayout1->addWidget(SSON_Text2);
    VLayout1->addStretch();

    QHBoxLayout *HLayout1 = new QHBoxLayout();
    HLayout1->addWidget(SSON_LogoContainer);
    HLayout1->addLayout(VLayout1);

    this->setLayout(HLayout1);
}

void AboutPage::retranslateUi() {
    setWindowTitle(tr("About SSON"));
    SSON_Title->setText("SSON");
    SSON_Subtitle->setText(tr("Simulator for Slice Optical Networks"));
    SSON_Text->setText(tr("SSON is developed by the <a href=\"http://suehtamacv.github.io/eons-ufcg/\">Radiometry Research Group</a>, under the <a href=\"http://suehtamacv.github.io/eons-ufcg/linhas/modelagem/\">Computational Modeling of Electromagnetic Effects</a> line of research."));
    SSON_Text2->setText(tr("More information about SSON can be found <a href=\"http://suehtamacv.github.io/eons-ufcg/linhas/modelagem/eons.html\">here</a>."));
}
