#include "CSpreadSheet.h" 
int main()
{
	/*
	// �½�Excel�ļ�����·����TestSheetΪ�ڲ�����
CSpreadSheet SS("D:\\Almh\\study\\project\\cloudeclc\\data\\512_16\\test.xls", "TestSheet");

CStringArray sampleArray, testRow;

SS.BeginTransaction();

// �������
sampleArray.RemoveAll();
sampleArray.Add("����");
sampleArray.Add("����");
SS.AddHeaders(sampleArray);

// ��������
CString strName[] = {"�쾰��","��־��","����","ţӢ��","��С��"};
CString strAge[] = {"27","23","28","27","26"};
for(int i = 0; i < sizeof(strName)/sizeof(CString); i++)
{
sampleArray.RemoveAll();
sampleArray.Add(strName[i]);
sampleArray.Add(strAge[i]);
SS.AddRow(sampleArray);
}

SS.Commit();
*/
CSpreadSheet SS("D:\\Almh\\study\\project\\cloudeclc\\data\\512_16\\test.xls", "TestSheet");

CStringArray Rows, Column;

//����б��
m_AccessList.ResetContent();
for (int i = 1; i <= SS.GetTotalRows(); i++)
{
// ��ȡһ��
SS.ReadRow(Rows, i);
CString strContents = "";
for (int j = 1; j <= Rows.GetSize(); j++)
{
if(j == 1)
strContents = Rows.GetAt(j-1);
else
strContents = strContents + " --> " + Rows.GetAt(j-1);
}

m_AccessList.AddString(strContents);
}
	return 0;
 } 


