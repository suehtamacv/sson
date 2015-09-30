#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

/**
 * @brief The AboutPage class shows a simple About the SSON Dialog.
 */
class AboutPage : public QDialog {
    Q_OBJECT

  public slots:
    /**
     * @brief AboutPage_Close closes this Dialog.
     */
    void AboutPage_Close();

  public:
    /**
     * @brief AboutPage is AboutPage's constructor.
     * @param parent is the parent of this AboutPage.
     */
    explicit AboutPage(QWidget *parent = 0);
    void retranslateUi();

  private:
    void set_AboutPage();
    QLabel *SSON_Title;
    QLabel *SSON_Subtitle;
    QLabel *SSON_Text;
    QLabel *SSON_Text2;
};

#endif // ABOUTPAGE_H
