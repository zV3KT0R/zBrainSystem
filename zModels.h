#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QColor>
#include <QKeySequence>

class zCommandItem
{
public:
	int				m_iId;
	QColor			m_Color;
	QKeySequence	m_ksButton;
	QString			m_strName;
	zCommandItem() : m_iId(0) {}
};

class zCommandModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	zCommandModel(QObject *parent);
	~zCommandModel();

	enum Roles
	{
		Id = Qt::UserRole,
		//		Gid,
		Color,
		Button,
		Name
	};

	enum HeaderNames
	{
		H_ID = 0,
		//		H_GID,
		H_COLOR,
		H_BUTTON,
		H_NAME
	};

	/// количество строк. Устанавливаем так, чтобы скроллер отображался корректно
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const { return m_lstItems.count(); };
	/// устанавливаем количество столбцов.
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { return 4; };
	/// функция для передачи данных пользователю
	virtual QVariant data(const QModelIndex &, int) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	zCommandItem * item(int);

public slots:

	void addItem();
	void editItem(int row);
	void deleteItem(int row);
	void clear();

	void load();
	void save();

protected:
	void addItem(zCommandItem *);
private:
	QList<zCommandItem*>		m_lstItems;
};