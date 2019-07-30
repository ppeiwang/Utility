#include "QActiveExcel.h"
#include <assert.h>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <windows.h> 
#include <set>
#include "logger.hpp"

#define SAFE_DELTE(x) if(x){delete x; x = nullptr;}

/*
********************	CExcelSheet	  *****************
*/

const QList<QVariant>& CExcelSheet::GetTitle() const
{
	const QList<QVariant>* ptr = nullptr;
	if (m_vecSheetValues.size() == 0)
	{
		throw CActiveExcelException(QString(u8"WorkBook [%1] try to GetTile failed. [%2] sheet is empty!").arg(workBookName).arg(sheetName));
	}

	return m_vecSheetValues[0];
}

void CExcelSheet::SetWorkBookName(const QString& _name)
{
	workBookName = _name;
}

const QString& CExcelSheet::GetWorkBookName() const
{
	return workBookName;
}

const QString& CExcelSheet::GetSheetName() const
{
	return sheetName;
}

void CExcelSheet::SetSheetName(const QString& _name)
{
	sheetName = _name;
}

void CExcelSheet::Insert(QList<QVariant>&& line)
{
	m_vecSheetValues.push_back(line);
}

void CExcelSheet::Insert(const QList<QVariant>& line)
{
	m_vecSheetValues.push_back(line);
}

void CExcelSheet::RemoveAt(int index)
{
	m_vecSheetValues.removeAt(index);
}

const QList<QList<QVariant>>& CExcelSheet::Value() const
{
	return m_vecSheetValues;
}

int CExcelSheet::GetColumnsCount() const
{
	int count = 0;
	const QList<QVariant>& Titles = GetTitle();

	for (const auto& cell : Titles)
	{
		if (cell.isValid())
		{
			count++;
		}
	}

	return count;
}

int CExcelSheet::GetRowsCount() const
{
	return m_vecSheetValues.size();
}

void CExcelSheet::SetTitleMap() const noexcept
{
	int titleIndex = 0;
	try
	{
		const QList<QVariant>& vecTitles = GetTitle();
		for (const auto& title : vecTitles)
		{
			if (title.canConvert<QString>())
			{
				QString strTitle = title.toString();
				strTitle = strTitle.toUpper();
				m_mapTitle.insert(strTitle, titleIndex);
			}
			else
			{
				QString strColumnName;
				ConvertNumberToColName(titleIndex + 1, strColumnName);
				CLogger::GetInstance().OutputWarning(QString(R"(The [%1] of [%2] at [%3] title consists of unknown type item!)").\
					arg(workBookName).arg(sheetName).arg(strColumnName));
			}

			titleIndex++;
		}
	} // 部分功能要求空表转出空bin
	catch (const CActiveExcelException& e)
	{
		CLogger::GetInstance().OutputError(QString(u8"SetTitleMap failed: [%1]").arg(e.GetReason()));
	}
}

int CExcelSheet::TrytoFindTitleIndex(const QString& strTarget)const noexcept
{
	if (m_mapTitle.size() == 0 && GetColumnsCount() > 0)
	{
		SetTitleMap();
	}

	int index = -1;

	auto itr = m_mapTitle.constFind(strTarget);
	if (itr != m_mapTitle.constEnd())
	{
		index = itr.value();
	}

	return index;
}

int CExcelSheet::GetTargetStrIndexFromTitles(const QString& strTarget) const
{
	if (m_mapTitle.size() == 0 && GetColumnsCount() > 0)
	{
		SetTitleMap();
	}

	int index = -1;

	auto itr = m_mapTitle.constFind(strTarget.toUpper());
	if (itr != m_mapTitle.constEnd())
	{
		index = itr.value();
	}
	else
	{
		throw CActiveExcelException(QString(u8"在[%1]的表格[%2]中找不到对应的标题[%3]").\
		arg(workBookName).arg(sheetName).arg(strTarget));
	}

	return index;
}

bool CExcelSheet::CheckTitleUnique(QString& info) const noexcept
{
	bool bRtn = true;
	std::set<QString> uniqueSet;
	const QList<QVariant>& vecTitles = GetTitle();
	for (const auto& title : vecTitles)
	{
		if (title.canConvert<QString>()) // prevent bad set data from invalid cell
		{
			QString strTitle = title.toString();
			auto result = uniqueSet.insert(strTitle.toUpper());
			if (result.second == false)
			{
				info = QString(u8"在[%1]的表格[%2]中存在相同的标题列[%3]. Case insensible").\
					arg(workBookName).arg(sheetName).arg(strTitle);
				bRtn = false;
				break;
			}
		}
	}

	return bRtn;
}

int CExcelSheet::RemoveEmptyLine()
{
	int iErrorCode = State_CExcel_Sheet::E_OK;
	const auto columnCount = GetColumnsCount();
	size_t rowCount = 0;
	size_t index = 1;
	size_t emptyRowCount = 0;
	for (const auto& rowValues : m_vecSheetValues)
	{
		if (rowValues.size() > columnCount)
		{
			iErrorCode |= State_CExcel_Sheet::E_COLUMN_OUT_RANGE;
		}

		bool emptyFlag = false;
		for (const auto& cell : rowValues)
		{
			if (cell.toString().isEmpty())
			{
				emptyFlag = true;
			}
			else
			{
				break;
			}
		}

		if (emptyFlag)
		{
			iErrorCode |= State_CExcel_Sheet::E_EMPTY_ROW;
			m_vecSheetValues.removeAt(rowCount);

			CLogger::GetInstance().OutputWarning(QString(u8"Excel[%1]的Sheet[%2]第[%3]行是空行").\
				arg(workBookName).arg(sheetName).arg(index));

			emptyRowCount++;

			if (emptyRowCount > 2000) // 空行太多，会导致日志激增。这里做一个上限检测
			{
				throw CActiveExcelException(QString(u8"在[%1]的表格[%2]中存在大量空行，请清理空行").\
					arg(workBookName).arg(sheetName));
			}
		}
		else
		{
			++rowCount;
		}
		index++;
	}

	return  iErrorCode;
}


/*
********************	CExcelSheet End	  *****************
*/


void CExcelWorkbook::RemoveAllSheetsEmptyLine()
{
	for (auto& sheet : m_vecSheets)
	{
		sheet.RemoveEmptyLine();
	}
}


/*
********************	CExcelWorkBook End	  *****************
*/



CActiveXExcel::CActiveXExcel() :
	m_QAxExcelInstance(nullptr)
	, m_QAxExcelWorkbooks(nullptr)
{

}

CActiveXExcel::~CActiveXExcel()
{
	SAFE_DELTE(m_QAxExcelWorkbooks);

	if (m_QAxExcelInstance)
	{
		m_QAxExcelInstance->dynamicCall("Quit()");
		SAFE_DELTE(m_QAxExcelInstance);
	}

	CLogger::GetInstance().OutputInfo("Cleaning Up The ActiveX Instance...");
}

void CActiveXExcel::Init()
{
	HRESULT r = OleInitialize(0);
	if (r != S_OK && r != S_FALSE)
	{
		qWarning(u8"Qt:初始化Ole失败（error %x）", (unsigned int)r);
		throw CActiveExcelException(QString(u8"OleInitialize Failed"));
	}

	m_QAxExcelInstance = new QAxObject("Excel.Application", nullptr);
	if (m_QAxExcelInstance)
	{
		m_QAxExcelInstance->dynamicCall("SetVisible(bool)", false);
		m_QAxExcelInstance->setProperty("DisplayAlerts", false);
		m_QAxExcelInstance->setProperty("EnableEvents", false);
		m_QAxExcelWorkbooks = m_QAxExcelInstance->querySubObject("Workbooks");
	}
	else
	{
		throw CActiveExcelException(QString(u8"CActiveXExcel 构造QAxExcelInstance 失败， 请确认Mirosoft Excel COM组件已经配置"));
	}

	if (m_QAxExcelWorkbooks == nullptr)
	{
		throw CActiveExcelException(QString(u8"CActiveXExcel Query Workbooks 失败， 请确认OLE已经成功初始化"));
	}
}

void CActiveXExcel::DeInit()
{
	SAFE_DELTE(m_QAxExcelWorkbooks);

	if (m_QAxExcelInstance)
	{
		m_QAxExcelInstance->dynamicCall("Quit()");
		SAFE_DELTE(m_QAxExcelInstance);
	}

	OleUninitialize();
}

void CActiveXExcel::InitWorkBook()
{
	DeInitWorkBook();

	if (m_QAxExcelInstance && m_QAxExcelWorkbooks == nullptr)
	{
		m_QAxExcelWorkbooks = m_QAxExcelInstance->querySubObject("Workbooks");
	}

	if (m_QAxExcelWorkbooks == nullptr)
	{
		throw CActiveExcelException(QString(u8"InitWorkBook CActiveXExcel Query Workbooks失败. 请确认Excel文件是否被占用"));
	}
}

void CActiveXExcel::DeInitWorkBook()
{
	if (m_QAxExcelWorkbooks)
	{
		m_QAxExcelWorkbooks->dynamicCall("Close()");
		SAFE_DELTE(m_QAxExcelWorkbooks);
	}
}

void ConvertNumberToColName(const int num, QString& strColumnName)
{
	//assert(num > 0 && num < 65535);
	int tempData = num / 26;
	if (tempData > 0)
	{
		int mode = num % 26;
		ConvertNumberToColName(mode, strColumnName);
		ConvertNumberToColName(tempData, strColumnName);
	}
	else
	{
		const QChar ch = num + 0x40; // ASCII 'A' is 0x40
		strColumnName = (ch + strColumnName);
	}
}

std::shared_ptr<CExcelWorkbook> CActiveXExcel::ReadFromExcel(const QString& fileName) const
{
	std::shared_ptr<CExcelWorkbook> pworkBook{ new CExcelWorkbook{QFile{fileName}} };

	const QFileInfo& fileInfo = pworkBook->GetFileInfo();
	const QString& fileFullPath = fileInfo.absoluteFilePath();
	CLogger::GetInstance().OutputInfo(QString("Reading from: ") + fileFullPath);

	if (m_QAxExcelInstance && m_QAxExcelWorkbooks && fileInfo.exists())
	{
		QAxObject* QAxWorbook = m_QAxExcelWorkbooks->querySubObject("Open(QString&)", fileFullPath);
		if (QAxWorbook)
		{
			QAxObject* QAxWorksheets = QAxWorbook->querySubObject("WorkSheets");
			if (QAxWorksheets)
			{
				const int iSheetsCount = QAxWorksheets->property("Count").toInt();

				pworkBook->m_vecSheets.reserve(iSheetsCount);

				for (int iSheetIndex = 1; iSheetIndex <= iSheetsCount; iSheetIndex++)
				{
					QAxObject* QAxWorksheet = QAxWorksheets->querySubObject("Item(int)", iSheetIndex);
					if (QAxWorksheet)
					{
						CExcelSheet sheet;
						sheet.SetWorkBookName(pworkBook->m_name);
						sheet.SetSheetName(QAxWorksheet->property("Name").toString());
						QAxObject* QAxUsedrange = QAxWorksheet->querySubObject("UsedRange");
						if (QAxUsedrange)
						{
							QVariant varUsedRange = QAxUsedrange->dynamicCall("Value");
							SAFE_DELTE(QAxUsedrange);

							QVariantList varRows = varUsedRange.toList();
							if (varRows.isEmpty() == false)
							{
								for (int i = 0; i < varRows.size(); ++i)
								{
									QVariantList rowData = varRows[i].toList();
									sheet.Insert(std::move(rowData));
								}
							}
						}
						else
						{
							throw CActiveExcelException(QString(u8"Query UsedRange failed! The sheet number is: %1").arg(iSheetIndex));
						}

						pworkBook->m_vecSheets.push_back(std::move(sheet));
					}
					else
					{
						throw CActiveExcelException(QString(u8"Query workSheet failed! The sheet number is:: %1").arg(iSheetIndex));
					}

				} // sheet for loop end

				SAFE_DELTE(QAxWorksheets);
			}
			else
			{
				throw CActiveExcelException(QString(u8"Query workSheets failed!"));
			}

			QAxWorbook->dynamicCall("Close()");
			SAFE_DELTE(QAxWorbook);
		}
		else
		{
			throw CActiveExcelException(QString(u8"Open workbook failed! Reading has been aborted!"
				"必须安装Office，且激活成功"));
		}
	}
	else
	{
		if (m_QAxExcelInstance == nullptr)
		{
			throw CActiveExcelException(QString(u8"尝试读取Excel失败，设备未经初始化"));
		}

		if (fileInfo.exists() == false)
		{
			throw CActiveExcelException(QString(u8"未找到尝试读取的文件[%1]").arg(fileFullPath));
		}
	}

	return pworkBook;
}

std::shared_ptr<CExcelWorkbook> CActiveXExcel::ReadFromExcel(const QString& fileName, const QSet<QString>& sheetNameSet) const
{
	std::shared_ptr<CExcelWorkbook> pworkBook{ new CExcelWorkbook{ QFile{ fileName } } };

	const QFileInfo& fileInfo = pworkBook->GetFileInfo();
	const QString& fileFullPath = fileInfo.absoluteFilePath();
	CLogger::GetInstance().OutputInfo(QString("Reading from: ") + fileFullPath);

	if (m_QAxExcelInstance && m_QAxExcelWorkbooks && fileInfo.exists())
	{
		QAxObject* QAxWorbook = m_QAxExcelWorkbooks->querySubObject("Open(QString&)", fileFullPath);
		if (QAxWorbook)
		{
			QAxObject* QAxWorksheets = QAxWorbook->querySubObject("WorkSheets");
			if (QAxWorksheets)
			{
				const int iSheetsCount = QAxWorksheets->property("Count").toInt();

				pworkBook->m_vecSheets.reserve(sheetNameSet.size());

				for (int iSheetIndex = 1; iSheetIndex <= iSheetsCount; iSheetIndex++)
				{
					QAxObject* QAxWorksheet = QAxWorksheets->querySubObject("Item(int)", iSheetIndex);
					if (QAxWorksheet)
					{
						const QString _sheetName = QAxWorksheet->property("Name").toString();
						auto itrSheetSet = sheetNameSet.find(_sheetName);
						if (itrSheetSet == sheetNameSet.cend())
						{
							continue;	
						}

						CExcelSheet sheet;
						sheet.SetWorkBookName(pworkBook->m_name);
						sheet.SetSheetName(_sheetName);
						QAxObject* QAxUsedrange = QAxWorksheet->querySubObject("UsedRange");
						if (QAxUsedrange)
						{
							QVariant varUsedRange = QAxUsedrange->dynamicCall("Value");
							SAFE_DELTE(QAxUsedrange);

							QVariantList varRows = varUsedRange.toList();
							if (varRows.isEmpty() == false)
							{
								for (int i = 0; i < varRows.size(); ++i)
								{
									QVariantList rowData = varRows[i].toList();
									sheet.Insert(std::move(rowData));
								}
							}
						}
						else
						{
							throw CActiveExcelException(QString(u8"Query UsedRange failed! The sheet number is: %1").arg(iSheetIndex));
						}

						pworkBook->m_vecSheets.push_back(std::move(sheet));
					}
					else
					{
						throw CActiveExcelException(QString(u8"Query workSheet failed! The sheet number is:: %1").arg(iSheetIndex));
					}

				} // sheet for loop end

				SAFE_DELTE(QAxWorksheets);
			}
			else
			{
				throw CActiveExcelException(QString(u8"Query workSheets failed!"));
			}

			QAxWorbook->dynamicCall("Close()");
			SAFE_DELTE(QAxWorbook);
		}
		else
		{
			throw CActiveExcelException(QString(u8"Open workbook failed! Reading has been aborted!"
				"必须安装Office，且激活成功"));
		}
	}
	else
	{
		if (m_QAxExcelInstance == nullptr)
		{
			throw CActiveExcelException(QString(u8"尝试读取Excel失败，设备未经初始化"));
		}

		if (fileInfo.exists() == false)
		{
			throw CActiveExcelException(QString(u8"未找到尝试读取的文件[%1]").arg(fileFullPath));
		}
	}

	return pworkBook;
}

void CActiveXExcel::ReadFromExcel(CExcelWorkbook& workBook) const
{
	const QFileInfo& fileInfo = workBook.GetFileInfo();
	const QString& fileFullPath = fileInfo.absoluteFilePath();
	CLogger::GetInstance().OutputInfo(QString("Reading from: ") + fileFullPath);

	if (m_QAxExcelInstance && m_QAxExcelWorkbooks && fileInfo.exists())
	{
		QAxObject* QAxWorbook = m_QAxExcelWorkbooks->querySubObject("Open(QString&)", fileFullPath);
		if (QAxWorbook)
		{
			QAxObject* QAxWorksheets = QAxWorbook->querySubObject("WorkSheets");
			if (QAxWorksheets)
			{
				const int iSheetsCount = QAxWorksheets->property("Count").toInt();

				workBook.m_vecSheets.reserve(iSheetsCount);

				for (int iSheetIndex = 1; iSheetIndex <= iSheetsCount; iSheetIndex++)
				{
					QAxObject* QAxWorksheet = QAxWorksheets->querySubObject("Item(int)", iSheetIndex);
					if (QAxWorksheet)
					{
						CExcelSheet sheet;
						sheet.SetWorkBookName(workBook.m_name);
						sheet.SetSheetName(QAxWorksheet->property("Name").toString());
						QAxObject* QAxUsedrange = QAxWorksheet->querySubObject("UsedRange");
						if (QAxUsedrange)
						{
							QVariant varUsedRange = QAxUsedrange->dynamicCall("Value");
							SAFE_DELTE(QAxUsedrange);

							QVariantList varRows = varUsedRange.toList();
							if (varRows.isEmpty() == false)
							{
								for (int i = 0; i < varRows.size(); ++i)
								{
									QVariantList rowData = varRows[i].toList();
									sheet.Insert(std::move(rowData));
								}
							}
						}
						else
						{
							throw CActiveExcelException(QString(u8"Query UsedRange failed! The sheet number is: %1").arg(iSheetIndex));
						}

						workBook.m_vecSheets.push_back(std::move(sheet));
					}
					else
					{
						throw CActiveExcelException(QString(u8"Query workSheet failed! The sheet number is: %1").arg(iSheetIndex));
					}

				} // sheet for loop end

				SAFE_DELTE(QAxWorksheets);
			}
			else
			{
				throw CActiveExcelException(QString(u8"Query workSheets failed!"));
			}

			QAxWorbook->dynamicCall("Close()");
			SAFE_DELTE(QAxWorbook);
		}
		else
		{
			throw CActiveExcelException(QString(u8"Open workbook failed! Reading has to be aborted!"
				"必须安装Office，且激活成功"));
		}
	}
	else
	{
		if (m_QAxExcelInstance == nullptr)
		{
			throw CActiveExcelException(QString(u8"尝试读取Excel失败，设备未经初始化"));
		}

		if (fileInfo.exists() == false)
		{
			throw CActiveExcelException(QString(u8"未找到尝试读取的文件[%1]").arg(fileFullPath));
	}
}
}

void CActiveXExcel::WriteIntoExcel(const QString& fileFullPath, const CExcelWorkbook& sourceWorkBook) const
{
	CLogger::GetInstance().OutputInfo(QString("Writting into: %1").arg(fileFullPath));

#ifdef DEBUG
	QElapsedTimer timer;
	timer.start();
#endif

	if (m_QAxExcelInstance && m_QAxExcelWorkbooks)
	{
		m_QAxExcelWorkbooks->dynamicCall("Add");
		QAxObject* QAxWorkbook = m_QAxExcelInstance->querySubObject("ActiveWorkBook");
		if (QAxWorkbook)
		{
			QAxObject* QAxWorkSheets = QAxWorkbook->querySubObject("Sheets");
			if (QAxWorkSheets)
			{
				const int iSheetsCount = sourceWorkBook.m_vecSheets.size();
				for (int iSheetIndex = 1; iSheetIndex <= iSheetsCount; iSheetIndex++)
				{
					QAxObject* QAxWorksheet = nullptr;
					if (iSheetIndex > 1)
					{
						QAxObject* QAxLastSheet = QAxWorkSheets->querySubObject("Item(int)", iSheetIndex - 1);
						if (QAxLastSheet)
						{
							QAxWorksheet = QAxWorkSheets->querySubObject("Add(QVariant)", QAxLastSheet->asVariant());
							QAxLastSheet->dynamicCall("Move(QVariant)", QAxWorksheet->asVariant());
						}
					}
					else
					{
						QAxWorksheet = QAxWorkSheets->querySubObject("Item(int)", iSheetIndex);
					}

					if (QAxWorksheet)
					{
						const CExcelSheet& sourceWorkSheet = sourceWorkBook.m_vecSheets[iSheetIndex - 1];
						const int iRowsCount = sourceWorkSheet.GetRowsCount();
						const int iColumnsCount = sourceWorkSheet.GetColumnsCount();

						const QString& strSheetName = sourceWorkSheet.GetSheetName();
						QAxWorksheet->dynamicCall("SetName(const QString&)", strSheetName);

						if (iColumnsCount > 0 && iRowsCount > 0)
						{
							QString strRange;
							ConvertNumberToColName(iColumnsCount, strRange);
							strRange += QString::number(iRowsCount);
							strRange = "A1:" + strRange;
#ifdef DEBUG
							qDebug() << strRange;
#endif

							QAxObject* QAxRange = QAxWorksheet->querySubObject("Range(const QString&)", strRange);
							if (QAxRange)
							{
								const QList<QList<QVariant>>& sheetData = sourceWorkSheet.Value();
								QVariantList varList;
								for (int iRowIndex = 0; iRowIndex < iRowsCount; iRowIndex++)
								{
									varList.append(QVariant(sheetData[iRowIndex]));
								}
								QAxRange->setProperty("Value", varList);
								SAFE_DELTE(QAxRange);
							}

						}
					}
					else
					{
						throw CActiveExcelException(QString(u8"Add workSheet failed!"));
						//CLogger::GetInstance().OutputError("Add workSheet failed!");
					}

				}

				SAFE_DELTE(QAxWorkSheets);
			}
			else
			{
				CLogger::GetInstance().OutputError("Query workSheets failed!");
			}

			QAxWorkbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileFullPath));
			SAFE_DELTE(QAxWorkbook);
			}
		else
		{
			CLogger::GetInstance().OutputError("Add workBook failed!");
		}
		}

#ifdef DEBUG
	qDebug() << timer.elapsed();
#endif
}

void CActiveXExcel::testCase()
{
	const QString sourceFilePath = QDir::currentPath() + "/testImmenseExcel.xlsx";
	const QString copyFilePath = QDir::currentPath() + "/copiedExcel.xlsx";

	CExcelWorkbook workBook = CExcelWorkbook(QFileInfo(sourceFilePath));
	ReadFromExcel(workBook);

#if 0
	for (const auto& sheetItr : workBook.m_vecSheets)
	{
		qDebug() << sheetItr.sheetName;
		for (const auto& rowItr : sheetItr.m_vecSheetValues)
		{
			for (const auto& itr : rowItr)
			{
				qDebug() << itr;
			}
		}
	}
#endif

	WriteIntoExcel(copyFilePath, workBook);
	}