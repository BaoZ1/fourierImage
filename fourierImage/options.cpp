#include "options.h"

options::options(QWidget* parent)
	: QWidget(parent)
{
	point = new QLineEdit(this);
	sizeO = new QLineEdit(this);
	finenessO = new QLineEdit(this);
	threadNumO = new QLineEdit(this);
	calculateAccuracyO = new QLineEdit(this);
	pointL = new QLabel(this);
	sizeOL = new QLabel(this);
	finenessOL = new QLabel(this);
	pointListL = new QLabel(this);
	jokeL = new QLabel(this);
	record = new QLabel(this);
	threadNumOL = new QLabel(this);
	calculateAccuracyOL = new QLabel(this);
	addBtn = new QPushButton(this);
	graphBtn = new QPushButton(this);
	finishBtn = new QPushButton(this);
	listSwitch = new QPushButton(this);
	pointList = new QListWidget(this);
	antialiasingCB = new QCheckBox(this);
	pointNum = 0;
	units.reserve(pointsLimit);
	pointFormat.setPattern("-?1?[0-9]{1,2}([.][1-9][0-9]{0,3})?[,]-?1?[0-9]{0,2}([.][1-9][0-9]{0,3})?");

	this->setWindowTitle(tr("Options"));
	this->setFixedSize(565, 270);

	pointL->setText(tr("add path point:"));
	pointL->setGeometry(25, 0, 200, 30);

	point->setPlaceholderText("x,y");
	point->setGeometry(25, 30, 170, 30);
	point->setValidator(new QRegExpValidator(pointFormat));

	graphBtn->setText("G");
	graphBtn->setGeometry(195, 30, 30, 30);

	addBtn->setText(tr("add"));
	addBtn->setGeometry(25, 60, 200, 30);

	sizeOL->setText(tr("image size:"));
	sizeOL->setGeometry(25, 120, 125, 30);

	sizeO->setGeometry(25, 150, 125, 30);
	sizeO->setText("800,800");
	sizeO->setValidator(new QRegExpValidator((QRegExp)"[1-9][0-9]{2,3}[,][1-9][0-9]{2,3}"));

	finenessOL->setText(tr("fineness:"));
	finenessOL->setGeometry(155, 120, 70, 30);

	finenessO->setText("20");
	finenessO->setGeometry(155, 150, 70, 30);
	finenessO->setValidator(new QRegExpValidator((QRegExp)"[1-9][0-9]{1,2}"));

	finishBtn->setText(tr("finish"));
	finishBtn->setGeometry(25, 210, 200, 30);

	listSwitch->setText("->");
	listSwitch->setGeometry(540, 115, 25, 20);

	pointListL->setText(tr("list of path points"));
	pointListL->setGeometry(250, 0, 150, 30);

	record->setText("0/" + QString::number(pointsLimit));
	record->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	record->setGeometry(430, 0, 100, 30);

	jokeL->setText(tr("I am a waste now..."));
	jokeL->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	jokeL->setGeometry(250, 100, 290, 70);

	pointList->setGeometry(250, 30, 290, 210);

	threadNumOL->setText(tr("threads:"));
	threadNumOL->setGeometry(590, 0, 200, 30);

	threadNumO->setText("10");
	threadNumO->setGeometry(590, 30, 200, 30);
	threadNumO->setValidator(new QRegExpValidator((QRegExp)"[1-9][0-9]"));

	calculateAccuracyOL->setText(tr("calculate accuracy:"));
	calculateAccuracyOL->setGeometry(590, 60, 200, 30);

	calculateAccuracyO->setText("10");
	calculateAccuracyO->setGeometry(590, 90, 200, 30);
	calculateAccuracyO->setValidator(new QRegExpValidator((QRegExp)"[1-9][0-9]{5,9}"));

	antialiasingCB->setText(tr("antialiasing"));
	antialiasingCB->setGeometry(590, 150, 200, 30);
	antialiasingCB->setChecked(true);

	connect(point, &QLineEdit::returnPressed, this, &options::add);
	connect(addBtn, &QPushButton::clicked, this, &options::add);
	connect(graphBtn, &QPushButton::clicked, [&] {
		dotsGraph = new coordinateWidget(this, pointList, record, &units, &hiddenLines);
		connect(dotsGraph, SIGNAL(addPoint(complex<double>)), this, SLOT(addFromCoord(complex<double>)));
		connect(dotsGraph, SIGNAL(insertPoint(complex<double>, int)), this, SLOT(insertFromCoord(complex<double>, int)));
		connect(dotsGraph, SIGNAL(delPoint(int)), this, SLOT(delFromCoord(int)));
		connect(dotsGraph, &coordinateWidget::closing, [=] {
			pointList->setGeometry(250, 30, 290, 210);
			record->setGeometry(430, 0, 100, 30);

			pointList->show();
			record->show();
			}
		);

		dotsGraph->show();
		}
	);

	connect(listSwitch, &QPushButton::clicked, [&] {
		this->setFixedWidth((this->width() == 565) ? 840 : 565);
		listSwitch->setText((listSwitch->text() == "->") ? "<-" : "->");
		(listSwitch->pos() == QPoint(540, 115)) ? (listSwitch->setGeometry(815, 115, 25, 20)) : (listSwitch->setGeometry(540, 115, 25, 20));
		}
	);

	connect(finishBtn, &QPushButton::clicked, [&] {
		if ((pointNum != 0) && (!sizeO->text().isEmpty()) && (!threadNumO->text().isEmpty()) && (!calculateAccuracyO->text().isEmpty()) && (!finenessO->text().isEmpty()))
		{
			this->coefficients.resize(2 * finenessO->text().toInt() + 1);

			progressBar = new calculateProgress(2 * finenessO->text().toInt() + 1, &coefficients);
			connect(this, SIGNAL(inputPoint(complex<double>)), progressBar, SLOT(addPoint(complex<double>)));
			connect(this, SIGNAL(inputOptions_calculate(double, int, int, vector<double>*)), progressBar, SLOT(addOptions_calculate(double, int, int, vector<double>*)));
			this->close();
			progressBar->show();

			linesLength.push_back(0);

			for (int i = 0; i < pointNum; i++)
			{
				linesLength.push_back(sqrt(pow(units[i]->data().real() - units[(i + 1) % units.size()]->data().real(), 2) + pow(units[i]->data().imag() - units[(i + 1) % units.size()]->data().imag(), 2)));
				linesLength[i + 1] += linesLength[i];
			}

			for (int i = 0; i < hiddenLines.size(); i++)
			{
				if (hiddenLines[i])
				{
					if (i != 0)
					{
						if (hiddenLines[i - 1])
						{
							hiddenPaths[hiddenPaths.size() - 1].imag(linesLength[i + 1]);
						}
						else
						{
							hiddenPaths.push_back({ linesLength[i], linesLength[i + 1] });
						}
					}
					else
					{
						hiddenPaths.push_back({ linesLength[i], linesLength[i + 1] });
					}
				}
			}

			for (int i = 0; i < pointNum; i++)
			{
				emit inputPoint(units[i]->data());
			}
			emit inputOptions_calculate(
				calculateAccuracyO->text().toDouble(),
				finenessO->text().toInt(),
				threadNumO->text().toInt(),
				&linesLength
			);

			connect(progressBar, &calculateProgress::start, [&] {
				progressBar->close();

				image = new fourierImage(
					antialiasingCB->isChecked(),
					sizeO->text().left(sizeO->text().indexOf(',')).toInt(),
					sizeO->text().right(sizeO->text().length() - sizeO->text().indexOf(',') - 1).toInt(),
					finenessO->text().toInt(),
					&coefficients,
					threadNumO->text().toInt(),
					linesLength[linesLength.size() - 1],
					&hiddenPaths
				);
				image->show();
				}
			);
		}
		}
	);
}

options::~options()
{
}

void options::add()
{
	if (pointNum == pointsLimit - 1)
	{
		////////////////////////////////////////////////////////////路径点过多
		return;
	}
	if (pointFormat.exactMatch(point->text()))
	{
		QListWidgetItem* pointItem = new QListWidgetItem();
		pointItem->setSizeHint(QSize(250, 30));
		units.push_back(new listUnit(pointNum, point->text(), pointItem));

		connect(units[pointNum], SIGNAL(delU(listUnit*)), this, SLOT(del(listUnit*)));
		connect(units[pointNum], SIGNAL(insertU(listUnit*)), this, SLOT(insert(listUnit*)));
		connect(units[pointNum], SIGNAL(changeU(listUnit*)), this, SLOT(change(listUnit*)));

		pointList->addItem(pointItem);
		pointList->setItemWidget(pointItem, units[pointNum]);
		pointNum++;
		point->clear();
	}
	hiddenLines.push_back(false);
	listUpDate();
}

void options::listUpDate()
{
	for (int i = 0; i < units.size(); i++)
	{
		units[i]->setNumber(i);
	}
	if (pointList->parent() == dotsGraph)
	{
		dotsGraph->update();
	}

	record->setText(QString::number(pointNum) + "/" + QString::number(pointsLimit));
}

void options::del(listUnit* U)
{
	int i = pointList->row(U->item());
	delete units[i];
	units.erase(units.begin() + i);
	delete pointList->takeItem(i);
	pointNum--;
	hiddenLines.erase(hiddenLines.begin() + i);
	listUpDate();
}

void options::insert(listUnit* U)
{
	if (pointNum == pointsLimit - 1)
	{
		////////////////////////////////////////////////////////////路径点过多
		return;
	}
	int i = pointList->row(U->item());

	QWidget* insertW = new QWidget;
	QLabel* insertL = new QLabel(insertW);
	QLineEdit* insertLE = new QLineEdit(insertW);
	QPushButton* insertB = new QPushButton(insertW);

	insertW->resize(300, 150);
	insertL->setGeometry(50, 0, 200, 30);
	insertLE->setGeometry(50, 30, 200, 30);
	insertB->setGeometry(50, 90, 200, 30);
	insertLE->setValidator(new QRegExpValidator(pointFormat));

	insertW->setWindowTitle(tr("Insert path point"));
	insertL->setText(tr("serial number:"));
	insertL->setText(insertL->text() + QString::number(i + 2));
	insertB->setText(tr("insert"));
	insertW->show();
	insertW->setAttribute(Qt::WA_DeleteOnClose);

	connect(insertB, &QPushButton::clicked, [=] {
		if ((pointFormat.exactMatch(insertLE->text())))
		{
			QListWidgetItem* pointItem = new QListWidgetItem();
			pointItem->setSizeHint(QSize(250, 30));
			units.insert(units.begin() + i + 1, 1, NULL);
			units[i + 1] = new listUnit(i + 1, insertLE->text(), pointItem);

			connect(units[i + 1], SIGNAL(delU(listUnit*)), this, SLOT(del(listUnit*)));
			connect(units[i + 1], SIGNAL(insertU(listUnit*)), this, SLOT(insert(listUnit*)));
			connect(units[i + 1], SIGNAL(changeU(listUnit*)), this, SLOT(change(listUnit*)));

			pointList->insertItem(i + 1, pointItem);
			pointList->setItemWidget(pointItem, units[i + 1]);
			pointNum++;
			insertW->close();

			hiddenLines.insert(hiddenLines.begin() + i + 1, false);
			listUpDate();
		}
		}
	);
}

void options::change(listUnit* U)
{
	int i = pointList->row(U->item());

	QWidget* changeW = new QWidget;
	QLabel* changeL = new QLabel(changeW);
	QLineEdit* changeLE = new QLineEdit(changeW);
	QPushButton* changeB = new QPushButton(changeW);

	changeW->resize(300, 150);
	changeL->setGeometry(50, 0, 200, 30);
	changeLE->setGeometry(50, 30, 200, 30);
	changeB->setGeometry(50, 90, 200, 30);
	changeLE->setValidator(new QRegExpValidator(pointFormat));

	changeW->setWindowTitle(tr("Change the path point"));
	changeL->setText(tr("serial number:"));
	changeL->setText(changeL->text() + QString::number(i + 1));
	changeB->setText(tr("change"));
	changeW->show();
	changeW->setAttribute(Qt::WA_DeleteOnClose);

	connect(changeB, &QPushButton::clicked, [=] {
		if (pointFormat.exactMatch(changeLE->text()))
		{
			units[i]->setText(changeLE->text());
			changeW->close();
		}
		}
	);
}

void options::addFromCoord(complex<double> D)
{
	QListWidgetItem* pointItem = new QListWidgetItem();
	pointItem->setSizeHint(QSize(250, 30));
	units.push_back(new listUnit(pointNum, "", pointItem));
	units[pointNum]->setData(D);

	connect(units[pointNum], SIGNAL(delU(listUnit*)), this, SLOT(del(listUnit*)));
	connect(units[pointNum], SIGNAL(insertU(listUnit*)), this, SLOT(insert(listUnit*)));
	connect(units[pointNum], SIGNAL(changeU(listUnit*)), this, SLOT(change(listUnit*)));

	pointList->addItem(pointItem);
	pointList->setItemWidget(pointItem, units[pointNum]);
	pointNum++;

	hiddenLines.push_back(false);
	listUpDate();
}

void options::insertFromCoord(complex<double> C, int I)
{
	QListWidgetItem* pointItem = new QListWidgetItem();
	pointItem->setSizeHint(QSize(250, 30));
	units.insert(units.begin() + I + 1, 1, NULL);
	units[I + 1] = new listUnit(I + 1, C, pointItem);

	connect(units[I + 1], SIGNAL(delU(listUnit*)), this, SLOT(del(listUnit*)));
	connect(units[I + 1], SIGNAL(insertU(listUnit*)), this, SLOT(insert(listUnit*)));
	connect(units[I + 1], SIGNAL(changeU(listUnit*)), this, SLOT(change(listUnit*)));

	pointList->insertItem(I + 1, pointItem);
	pointList->setItemWidget(pointItem, units[I + 1]);
	pointNum++;

	hiddenLines.insert(hiddenLines.begin() + I + 1, false);
	listUpDate();
}

void options::delFromCoord(int i)
{
	delete units[i];
	units.erase(units.begin() + i);
	delete pointList->takeItem(i);
	pointNum--;
	hiddenLines.erase(hiddenLines.begin() + i);
	listUpDate();
}