#pragma once

#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "MailHeaderField.h"

/*
 * ----------------------------------------------------------------------
 * MailHeaderParseListener
 * メールヘッダの解析処理をフックする
 * ----------------------------------------------------------------------
 */
class MailHeaderParseListener
{
public:
	/* 解析処理のリスナを取得する */
	MailHeaderParseListener *getListener()
	{
		return listener;
	}

	/* 解析処理のリスナを設定する */
	void setListener(MailHeaderParseListener *listener)
	{
		this->listener = listener;
	}

	/* 
	 * 解析処理をフックをする 
	 * strField
	 */
	virtual bool hookParse(std::string strField, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBegin, std::ios::pos_type posEnd)
	{
		return true;
	}

protected:
	MailHeaderParseListener *listener;  // 解析処理のリスナ
};

/**
 * ----------------------------------------------------------------------
 * MailHeaderParser
 * メールヘッダを解析をする
 * ----------------------------------------------------------------------
 */
class MailHeaderParser : public MailHeaderParseListener
{
public:
	MailHeaderParser(const std::string strHeader,                           // コンストラクタ
	                 MailHeaderField::MHFTYPE type, bool recursive);
	~MailHeaderParser(void);                                                // デストラクタ
	bool parse();                                                           // 解析する

private:
	std::string strHeader;                                                  // ヘッダ
	MailHeaderField::MHFTYPE type;                                          // 解析するメールヘッダの種類
	std::istringstream iss;                                                 // 解析対象のインプットストリーム
	bool recursive;                                                         // 解析処理のフックを再帰的に行う

	bool parseField(MailHeaderField::MHFTYPE fieldType);
	bool parseFieldAsDefault(MailHeaderField::MHFTYPE fieldType);
	MailHeaderField::MHFTYPE MailHeaderParser::lexIsLongestMatch(
		std::vector<MailHeaderField::MHFTYPE> listLexingFieldType);
	bool MailHeaderParser::lexIsGeneralField(std::string strFieldName, 
		std::vector<MailHeaderField::MHFTYPE> listFieldType);

	bool lexIsHeader();
	bool lexIsFields();
	bool lexIsOrigDate();
	bool lexIsFrom();
	bool lexIsSender();
	bool lexIsReplyTo();
	bool lexIsTo();
	bool lexIsCc();
	bool lexIsAddressListOrCFWS();
	bool lexIsBcc();
	bool lexIsMessageID();
	bool lexIsNMsgID();
	bool lexIsInReplyTo();
	bool lexIsReferences();
	bool lexIsSubject();
	bool lexIsComments();
	bool lexIsPhraseList();
	bool lexIsKeywords();
	bool lexIsResentDate();
	bool lexIsResentFrom();
	bool lexIsResentSender();
	bool lexIsResentTo();
	bool lexIsResentCc();
	bool lexIsResentBcc();
	bool lexIsResentMsgID();
	bool lexIsTrace();

	bool lexIsReturn();
	bool lexIsNReceivedTokenAndDateTime();
	bool lexIsReceived();
	bool lexIsOptionalField();

	bool lexIsObsFields();
	bool lexIsObsHeader(const char *szHeader, MailHeaderField::MHFTYPE type);
	bool lexIsObsOrigDate();
	bool lexIsObsFrom();
	bool lexIsObsSender();
	bool lexIsObsReplyTo();
	bool lexIsObsTo();
	bool lexIsObsCc();
	bool lexIsAddreeListOrCFWSList();
	bool lexIsObsBcc();
	bool lexIsObsMessageID();
	bool lexIsPhraseOrMsgID();
	bool lexIsNPhraseOrMsgID();

	bool lexIsObsInReplyTo();
	bool lexIsObsReferences();
	bool lexIsObsSubject();
	bool lexIsObsComments();
	bool lexIsObsKeywords();
	bool lexIsObsResentFrom();
	bool lexIsObsResentSend();
	bool lexIsObsResentDate();
	bool lexIsObsResentTo();
	bool lexIsObsResentCc();
	bool lexIsObsResentBcc();
	bool lexIsObsResentMid();
	bool lexIsObsResentRply();
	bool lexIsObsReturn();
	bool lexIsNReceivedToken();
	bool lexIsObsReceived();
	bool lexIsObsOptional();



	bool lexIsDQUOTE();
	bool lexIsWSP();
	bool lexIsSP();
	bool lexIsHTAB();
	bool lexIsCRLF();
	bool lexIsALPHA();
	bool lexIsDIGIT();
	bool lexIsVCHAR();
	bool lexIsNOWSCTL();
	bool lexIsText();
	bool lexIsSpecials();
	bool lexIsQuotedPair();
	bool lexIsFWS();
	bool lexIsCtext();
	bool lexIsCcontent();
	bool lexIsComment();
	bool lexIsCFWS();
	bool lexIsAtext();
	bool lexIsAtom();
	bool lexIsDotAtom();
	bool lexIsDotAtomText();
	bool lexIsQtext();
	bool lexIsQcontent();
	bool lexIsQuotedString();
	bool lexIsWord();
	bool lexIsPhrase();
	bool lexIsUnstructured();
	bool lexIsDateTime();
	bool lexIsDayOfWeek();
	bool lexIsDayName();
	bool lexIsDate();
	bool lexIsYear();
	bool lexIsMonth();
	bool lexIsDay();
	bool lexIsTime();
	bool lexIsTimeOfDay();
	bool lexIsHour();
	bool lexIsMinute();
	bool lexIsSecond();
	bool lexIsZone();

	bool lexIsAddress();
	bool lexIsMailbox();
	bool lexIsNameAddr();
	bool lexIsAngleAddr();
	bool lexIsGroup();
	bool lexIsDisplayName();
	bool lexIsMailboxList();
	bool lexIsAddressList();
	bool lexIsGroupList();

	bool lexIsAddrSpec();
	bool lexIsLocalPart();
	bool lexIsDomain();
	bool lexIsDomainLiteral();
	bool lexIsDtext();

	bool lexIsMsgID();
	bool lexIsIDLeft();
	bool lexIsIDRight();
	bool lexIsNoFoldLiteral();

	bool lexIsPath();
	bool lexIsReceivedToken();

	bool lexIsFieldName();
	bool lexIsFtext();

	bool lexIsObsNOWSCTL();
	bool lexIsObsCtext();
	bool lexIsObsQtext();
	bool lexIsObsUtext();
	bool lexIsObsQp();
	bool lexIsObsBody();
	bool lexIsObsUnstruct();
	bool lexIsObsPhrase();
	bool lexIsObsPhraseList();
	bool lexIsObsFWS();
	bool lexIsObsDayOfWeek();
	bool lexIsObsDay();
	bool lexIsObsYear();
	bool lexIsObsHour();
	bool lexIsObsMinute();
	bool lexIsObsSecond();
	bool lexIsObsZone();
	bool lexIsObsAngleAddr();
	bool lexIsObsRoute();
	bool lexIsObsDomainList();
	bool lexIsObsMboxList();
	bool lexIsObsAddrList();
	bool lexIsObsGroupList();
	bool lexIsObsLocalPart();
	bool lexIsObsDomain();
	bool lexIsObsDtext();
	bool lexIsObsIDLeft();
	bool lexIsObsIDRight();
};
