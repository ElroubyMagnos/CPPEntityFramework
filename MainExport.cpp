#include "pch.h"
#include "ODBCFramework.h"
#include <iostream>
#include "LinqDB.h"
#include "LinqQueryExecutor.h"
using namespace std;
#define exportof __declspec(dllexport)


class Booking {
	int ID;
	string FullName;
	string PhoneNumber;
	string BookingDate;
	string BookingTime;
	string Notes;

public:
	auto as_tuple() {
		return tie(ID, FullName, PhoneNumber, BookingDate, BookingTime, Notes);
	}
};

ODBCFramework db;

vector<Booking> LoadBooks()
{
	db = ODBCFramework();
	bool succeded1 = db.connect(L"ElroubyOf", L"sa", L"123123");
	bool succeded2 = db.connect(L"Driver={SQL Server};Server=.;Database=CodexReserve;UID=sa;PWD=123123;");
	//db.connect(L"Driver={SQL Server};Server=localhost;Database=CodexReserve;Trusted_Connection=Yes;");

	LinqQueryExecutor<Booking> linq(db);
	auto data = linq.ExecuteQuery(L"Bookings").ToVector();

	return data;
}

extern "C"
{
	exportof void Loading()
	{
		auto books = LoadBooks();
	}
	
	/*exportof void MainLoad() 
	{
		ODBCFramework db;
		db.connect(L"DRIVER={SQL Server};SERVER=localhost;DATABASE=CodexCashier;UID=sa;PWD=123123;");

		LinqQueryExecutor<cc> executor(db);
		LinqDb<cc> entities = executor.ExecuteQuery(L"Logs");

		for (const auto& entity : entities.ToVector()) {
			cout << "ID: " << entity.ID << ", Title: " << entity.Title
				<< ", Description: " << entity.Description << ", UserID: " << entity.UserID << endl;
		}
	}*/
	//exportof void test()
	//{
	//	ODBCFramework fw;
	//	fw.connect(L"DRIVER={SQL Server};SERVER=localhost;DATABASE=CodexCashier;UID=sa;PWD=123123;");

	//	QueryBuilder qb(L"Logs");
	//	qb.Where(L"ID = 2");

	//	/*LinqQueryExecutor<cc> vec(fw, L"Logs");

	//	auto xx = vec.Execute();*/

	//	vector<vector<wstring>> data = fw.fetchResults(qb.Build());

	//	LinqDb<vector<wstring>> linq(data);


	//	auto z = linq.Where([](const vector<wstring>& s) 
	//		{ 
	//			return s[0] == L"2";
	//		});
	//	
	//	for (int i = 0; i < z.ToVector().size(); i++)
	//	{
	//		for (int j = 0; j < z.ToVector()[i].size(); j++)
	//		{
	//			for (int x = 0; x < z.ToVector()[i][j].size(); x++)
	//			{
	//				cout << char(z.ToVector()[i][j][x]) << "|";
	//			}
	//		}
	//	}

	//	/*auto v = linq.ToVector();

	//	for (int i = 0; i < v.size(); i++)
	//	{
	//		cout << v[i] << endl;
	//	}*/
	//	/*ODBCFramework fw;
	//	if (fw.connect(L"DRIVER={SQL Server};SERVER=localhost;DATABASE=CodexCashier;UID=sa;PWD=123123;"))
	//	{
	//		QueryBuilder qb = QueryBuilder(L"Logs");
	//		qb.Where(L"ID = 4 OR ID = 2");
	//		qb.OrderBy(L"ID");
	//		
	//		for (auto s : fw.fetchResults(qb.Build()))
	//		{
	//			for (int i = 0; i < s.size(); i++)
	//			{
	//				cout << fw.WToString(s[i]) << endl;
	//			}
	//		}
	//	}*/
	//}
}