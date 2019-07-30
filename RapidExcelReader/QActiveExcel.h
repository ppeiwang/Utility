// Author: Peiwang 2018.6

#pragma  once
#include <QString>
#include <QVariant>
#include <QList>
#include <QDir>
#include <QAxObject>
#include <memory>
#include <QSet>
#include <QMap>

// 1. 不允许作为全局数据使用;
// 2. 非线程安全
// 3. 建议每个线程只实例化一个CActiveXExcel

// QAxObject实际调用VBA对象的接口，请参考：Microsoft Object model (Excel VBA reference)
// https://msdn.microsoft.com/en-us/vba/excel-vba/articles/object-model-excel-vba-reference

namespace State_CExcel_Sheet
{
	const int E_OK = 0;
	const int E_EMPTY_ROW = 1; // 存在空行
	const int E_COLUMN_OUT_RANGE = 1 << 1; // 存在列数据超过Title长度
};

namespace ACTIVE_EXCEL
{
	class CQStringCmp
	{
	public:
		bool operator () (const QString& a, const QString& b) const
		{
			return QString::compare(a, b, Qt::CaseInsensitive) < 0;
		}
	};
}

class CExcelSheet
{
public:
	const QString& GetWorkBookName() const;

	void SetWorkBookName(const QString&);

	const QString& GetSheetName() const;

	void SetSheetName(const QString&);

	void Insert(QList<QVariant>&& line);

	void Insert(const QList<QVariant>& line);

	void RemoveAt(int index);

	const QList<QList<QVariant>>& Value() const;

	int GetColumnsCount() const;
	
	int GetRowsCount() const; // include title
	
	int GetTargetStrIndexFromTitles(const QString&) const; // 获取string在Tile列对应的Index

	int TrytoFindTitleIndex(const QString&) const noexcept;

	int RemoveEmptyLine(); // 移除空行

	bool CheckTitleUnique(QString& info) const noexcept; // 检查标题列的唯一性

private:
	const QList<QVariant>& GetTitle() const;

	void SetTitleMap() const noexcept;

private:
	QString workBookName; // for Debug

	QString sheetName;

	QList<QList<QVariant>> m_vecSheetValues; // { {ROW0} {ROW1}...} }

	mutable QMap<QString, int> m_mapTitle; //<title, index>
};

class CExcelWorkbook
{
public:
	CExcelWorkbook(const QFileInfo& _fileInfo) :
		m_fileInfo(_fileInfo)
	{
		m_name = _fileInfo.fileName();
	}

	CExcelWorkbook(CExcelWorkbook&& _other):
		m_vecSheets(std::move(_other.m_vecSheets)),
		m_name(std::move(_other.m_name))
	{
		int debug = 0;
	}

	const QFileInfo& GetFileInfo() const
	{
		return m_fileInfo;
	}

	void RemoveAllSheetsEmptyLine();

public:
	QList<CExcelSheet> m_vecSheets;
	QString m_name;

private:
	const QFileInfo m_fileInfo;
};

class CActiveXExcel
{
public:
	CActiveXExcel();
	~CActiveXExcel();

	void Init();

	void DeInit();

	void InitWorkBook();

	void DeInitWorkBook();

	std::shared_ptr<CExcelWorkbook> ReadFromExcel(const QString& fileName) const;

	std::shared_ptr<CExcelWorkbook> ReadFromExcel(const QString& fileName, const QSet<QString>& sheetName) const;

	// Warning! Not threads safe
	void ReadFromExcel(CExcelWorkbook&) const;

	// Warning! Not threads safe ( include parallel with "ReadFromExcel" )
	void WriteIntoExcel(const QString& fileFullPath, const CExcelWorkbook&) const;

	void testCase();

private:
	QAxObject* m_QAxExcelInstance;
	QAxObject* m_QAxExcelWorkbooks;
};

class CActiveExcelException :public std::exception
{
public:
	CActiveExcelException(const QString& _reason)
		: m_strReason(_reason)
	{

	}
	~CActiveExcelException() = default;

	const char* what() const throw()
	{
		return m_strReason.toStdString().c_str();
	}

	const QString& GetReason() const throw()
	{
		return m_strReason;
	}

private:
	QString m_strReason;
};

extern void ConvertNumberToColName(const int, QString&);