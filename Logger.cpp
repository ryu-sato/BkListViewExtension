#include "Logger.h"

/**
 * �R���X�g���N�^
 */
Logger::Logger()
{
	this->strPath = _T("C:\\Program Files (x86)\\RimArts\\B2\\PlugIns\\");
	this->strPluginName = _T("BkListViewExtension");
}

/**
 * �f�X�g���N�^
 */
Logger::~Logger()
{
}

/**
 * �C���X�^���X���擾�i�V���O���g���p�^�[���j
 */
Logger& Logger::getInstance() {
	static Logger instance;
	return instance;
}

/**
 * ���O���o��
 */
void Logger::print(LPCTSTR strMsg)
{
	Logger logger = getInstance();

	/* ���O�t�@�C�����J���Ȃ�������G���[�\�����ďI�� */
	basic_string<TCHAR> strFName = logger.strPath + logger.strPluginName + _T(".log");
	basic_ofstream<TCHAR> ofs(strFName.c_str(), ofstream::app);
	if (! ofs) {
		cerr << _T("cannot open file to write: ") << logger.strPath << std::endl;
		return;
	}

	/* �s���Ɍ��ݎ����ƃv���O�������𖄂ߍ��� */
	basic_stringstream<TCHAR> strPrefix;
	TCHAR szTime[1024];
	struct tm tmDate = {{0}};
	time_t now;
	time(&now);
	localtime_s(&tmDate, &now);
	_tcsftime(szTime, sizeof(szTime) / sizeof(szTime[0]), _T("%b %m %H:%M:%S"), &tmDate);
	strPrefix << szTime << _T(" ") << logger.strPluginName << _T(": ");

	/* ���O���������� */
	ofs << strPrefix.rdbuf() << strMsg << endl;
	ofs.flush();
	ofs.close();
}

/**
 * ���O���o��
 */
void Logger::print(std::string strMsg)
{
	Logger::print(strMsg.c_str());
}

void Logger::invalidParameterHandler(const wchar_t* expression, const wchar_t* function, 
									 const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	wprintf(L"Invalid parameter detected in function %s. File: %s Line: %d\n",
		    function, file, line);
	wprintf(L"Expression: %s", expression);
}