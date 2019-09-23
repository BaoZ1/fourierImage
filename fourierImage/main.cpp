#include "options.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QTranslator* translator_CN = new QTranslator;
    translator_CN->load(":/translate/fourierimage_zh.qm");
    a.installTranslator(translator_CN);

    options w;
    w.show();

    return a.exec();
}
