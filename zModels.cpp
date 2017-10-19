#include "zModels.h"

#include "zBrainSystem.h"

#include <QMessageBox>
#include <QSettings>

#include "zBrainSystem.h"
#include "zDlgCommand.h"

//////////////////////////////////////////////////////////////////////////
//zPostModel
//////////////////////////////////////////////////////////////////////////

zCommandModel::zCommandModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	load();
}

zCommandModel::~zCommandModel()
{
	qDeleteAll(m_lstItems);
}
QVariant zCommandModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant result;

	if (role != Qt::DisplayRole)
		return result;

	switch (section)
	{
	case H_ID:
		result = tr("#");
		break;
	case H_BUTTON:
		result = QString::fromLocal8Bit("Кнопка");
		break;
	case H_COLOR:
		result = QString::fromLocal8Bit("Цвет");
		break;
	case H_NAME:
		result = QString::fromLocal8Bit("Имя");
		break;
	};
	return result;
}
QVariant zCommandModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();

	const zCommandItem * item = m_lstItems.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (index.column())
			{
			case H_ID:
				return index.row()+1;
				break;
			case H_COLOR:
				//return item->m_Color;
				break;
			case H_BUTTON:
				return item->m_ksButton.toString();
				break;
			case H_NAME:
				return item->m_strName;
				break;
			};
			break;
		}
		case Qt::TextAlignmentRole:
		{
			switch (index.column())
			{
			case H_ID:
				return int(Qt::AlignRight | Qt::AlignVCenter);
				break;
			default:
				return int(Qt::AlignLeft | Qt::AlignVCenter);
				break;
			};
			break;
		}
		case Qt::DecorationRole:
		{
			switch (index.column())
			{
			case H_COLOR:
				return item->m_Color;
			}
			break;
		}		
	}
	
	return QVariant();
}

void zCommandModel::addItem(zCommandItem * p)
{
	int row = m_lstItems.count();
	beginInsertRows(QModelIndex(), row, row);
	p->m_iId = m_lstItems.count() + 1;
	m_lstItems.append(p);
	endInsertRows();
}

void zCommandModel::addItem()
{
	zDlgCommand dlg(zMainWnd);

	if (dlg.exec() == QDialog::Accepted)
		addItem(new zCommandItem(dlg.command()));
}
void zCommandModel::editItem(int row)
{
	zDlgCommand dlg(zMainWnd, m_lstItems.at(row));

	if (dlg.exec() == QDialog::Accepted)
		*m_lstItems.at(row) = dlg.command();
}
void zCommandModel::deleteItem(int row)
{
	QMessageBox mb(QMessageBox::Icon::Warning, QString::fromLocal8Bit("Внимание!"), QString::fromLocal8Bit("Удалить команду?"),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, zMainWnd);

	if (mb.exec() == QMessageBox::StandardButton::Yes)
	{
		beginRemoveRows(QModelIndex(), row, row);
		m_lstItems.removeAt(row);
		endRemoveRows();
	}
}

void zCommandModel::clear()
{
	QMessageBox mb(QMessageBox::Icon::Warning, QString::fromLocal8Bit("Внимание!"), 
		QString::fromLocal8Bit("Удалить все команды?"),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, zMainWnd);

	if (mb.exec() != QMessageBox::StandardButton::Yes)
		return;

	beginRemoveRows(QModelIndex(), 0, m_lstItems.size());
	qDeleteAll(m_lstItems);
	m_lstItems.clear();
	endRemoveRows();
	beginResetModel();
	endResetModel();
}

void zCommandModel::load()
{
	beginRemoveRows(QModelIndex(), 0, m_lstItems.size());
	qDeleteAll(m_lstItems);
	m_lstItems.clear();
	endRemoveRows();

	QSettings * settings = zBrainSystem::settings();

	settings->beginGroup(QLatin1String("Commands"));

	int count = settings->value(QLatin1String("count"), 0).toInt();

	for (int i = 0; i < count; i++)
	{
		zCommandItem cmd;
		cmd.m_iId = i;
		cmd.m_strName = settings->value(tr("Name_%1").arg(i), "").toString();
		cmd.m_Color = settings->value(tr("Color_%1").arg(i), 0).toUInt();
		cmd.m_ksButton = QKeySequence::fromString( settings->value(tr("Button_%1").arg(i), "").toString());

		addItem(new zCommandItem(cmd));
	}

	settings->endGroup();

	beginResetModel();
	endResetModel();
}
void zCommandModel::save()
{
	QSettings * settings = zBrainSystem::settings();

	settings->beginGroup(QLatin1String("Commands"));
	settings->remove("");

	settings->setValue(QLatin1String("count"), m_lstItems.count());

	for (int i = 0; i < m_lstItems.count(); i++)
	{
		zCommandItem * cmd = m_lstItems.at(i);

		settings->setValue(tr("Name_%1").arg(i), cmd->m_strName);
		settings->setValue(tr("Color_%1").arg(i), cmd->m_Color.rgba());
		settings->setValue(tr("Button_%1").arg(i), cmd->m_ksButton.toString());
	}

	settings->endGroup();
}

zCommandItem * zCommandModel::item(int i)
{
	if (i < 0 || i >= m_lstItems.count())
		return nullptr;

	return m_lstItems.at(i);
}
