#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * MailHeaderParser
 * ÉÅÅ[ÉãÉwÉbÉ_ÇÃâêÕÉNÉâÉX
 * ----------------------------------------------------------------------
 */

/**
 * <summary>
 * ÉRÉìÉXÉgÉâÉNÉ^
 * </summary>
 */
MailHeaderParser::MailHeaderParser(const std::string strHeader, 
								   MailHeaderField::MHFTYPE type, bool recursive)
{
	this->strHeader = strHeader;
	this->iss.str(strHeader);
	this->type = type;
	this->recursive = recursive;
}

/**
 * ÉfÉXÉgÉâÉNÉ^
 */
MailHeaderParser::~MailHeaderParser(void)
{
}

/**
 * âêÕÇ∑ÇÈ
 */
bool MailHeaderParser::parse()
{
	return parseField(this->type);
}

bool MailHeaderParser::parseFieldAsDefault(MailHeaderField::MHFTYPE fieldType)
{
	switch (fieldType) {
	case MailHeaderField::HEADER:                   return lexIsHeader();               
	case MailHeaderField::FIELDS:                   return lexIsFields();               
	case MailHeaderField::ORIG_DATE:                return lexIsOrigDate();             
	case MailHeaderField::FROM:                     return lexIsFrom();                 
	case MailHeaderField::SENDER:                   return lexIsSender();               
	case MailHeaderField::REPLY_TO:                 return lexIsReplyTo();              
	case MailHeaderField::TO:                       return lexIsTo();                   
	case MailHeaderField::CC:                       return lexIsCc();                   
	case MailHeaderField::BCC:                      return lexIsBcc();                  
	case MailHeaderField::MESSAGE_ID:               return lexIsMessageID();            
	case MailHeaderField::INREPLY_TO:               return lexIsInReplyTo();            
	case MailHeaderField::REFERENCES:               return lexIsReferences();           
	case MailHeaderField::SUBJECT:                  return lexIsSubject();              
	case MailHeaderField::COMMENTS:                 return lexIsComments();             
	case MailHeaderField::KEYWORDS:                 return lexIsKeywords();             
	case MailHeaderField::RESENT_DATE:              return lexIsResentDate();           
	case MailHeaderField::RESENT_FROM:              return lexIsResentFrom();           
	case MailHeaderField::RESENT_SENDER:            return lexIsResentSender();         
	case MailHeaderField::RESENT_TO:                return lexIsResentTo();             
	case MailHeaderField::RESENT_CC:                return lexIsResentCc();             
	case MailHeaderField::RESENT_BCC:               return lexIsResentBcc();            
	case MailHeaderField::RESENT_MSGID:             return lexIsResentMsgID();          
	case MailHeaderField::TRACE:                    return lexIsTrace();                
	case MailHeaderField::RETURN:                   return lexIsReturn();               
	case MailHeaderField::RECEIVED:                 return lexIsReceived();             
	case MailHeaderField::OPTIONAL_FIELD:           return lexIsOptionalField();        
	case MailHeaderField::OBS_FIELDS:               return lexIsObsFields();            
	case MailHeaderField::OBS_ORIG_DATE:            return lexIsObsOrigDate();          
	case MailHeaderField::OBS_FROM:                 return lexIsObsFrom();              
	case MailHeaderField::OBS_SENDER:               return lexIsObsSender();            
	case MailHeaderField::OBS_REPLY_TO:             return lexIsObsReplyTo();           
	case MailHeaderField::OBS_TO:                   return lexIsObsTo();                
	case MailHeaderField::OBS_CC:                   return lexIsObsCc();                
	case MailHeaderField::OBS_BCC:                  return lexIsObsBcc();               
	case MailHeaderField::OBS_MESSAGE_ID:           return lexIsObsMessageID();         
	case MailHeaderField::OBS_INREPLY_TO:           return lexIsObsInReplyTo();         
	case MailHeaderField::OBS_REFERENCES:           return lexIsObsReferences();        
	case MailHeaderField::OBS_SUBJECT:              return lexIsObsSubject();           
	case MailHeaderField::OBS_COMMENTS:             return lexIsObsComments();          
	case MailHeaderField::OBS_KEYWORDS:             return lexIsObsKeywords();          
	case MailHeaderField::OBS_RESENT_FROM:          return lexIsObsResentFrom();        
	case MailHeaderField::OBS_RESENT_SEND:          return lexIsObsResentSend();        
	case MailHeaderField::OBS_RESENT_DATE:          return lexIsObsResentDate();        
	case MailHeaderField::OBS_RESENT_TO:            return lexIsObsResentTo();          
	case MailHeaderField::OBS_RESENT_CC:            return lexIsObsResentCc();          
	case MailHeaderField::OBS_RESENT_BCC:           return lexIsObsResentBcc();         
	case MailHeaderField::OBS_RESENT_MID:           return lexIsObsResentMid();         
	case MailHeaderField::OBS_RESENT_RPLY:          return lexIsObsResentRply();        
	case MailHeaderField::OBS_RETURN:               return lexIsObsReturn();            
	case MailHeaderField::OBS_RECEIVED:             return lexIsObsReceived();          
	case MailHeaderField::OBS_OPTIONAL:             return lexIsObsOptional();          

	case MailHeaderField::T_ADDREE_LIST_OR_CFWS_LIST: return lexIsAddreeListOrCFWSList();
	case MailHeaderField::T_ADDRESS_LIST_OR_CFWS:    return lexIsAddressListOrCFWS();
	case MailHeaderField::T_PHRASE_OR_MSGID:         return lexIsPhraseOrMsgID();        
	case MailHeaderField::T_N_PHRASE_OR_MSGID:       return lexIsNPhraseOrMsgID();       
	case MailHeaderField::T_N_RECEIVED_TOKEN:        return lexIsNReceivedToken();
	case MailHeaderField::T_N_RECEIVED_TOKEN_AND_DATE_TIME: return lexIsNReceivedTokenAndDateTime();

	case MailHeaderField::T_DQUOTE:          return lexIsDQUOTE();          
	case MailHeaderField::T_WSP:             return lexIsWSP();             
	case MailHeaderField::T_SP:              return lexIsSP();              
	case MailHeaderField::T_HTAB:            return lexIsHTAB();            
	case MailHeaderField::T_CRLF:            return lexIsCRLF();            
	case MailHeaderField::T_ALPHA:           return lexIsALPHA();           
	case MailHeaderField::T_DIGIT:           return lexIsDIGIT();           
	case MailHeaderField::T_VCHAR:           return lexIsVCHAR();           
	case MailHeaderField::T_NO_WS_CTL:       return lexIsNOWSCTL();         
	case MailHeaderField::T_TEXT:            return lexIsText();            
	case MailHeaderField::T_SPECIALS:        return lexIsSpecials();        
	case MailHeaderField::T_QUOTED_PAIR:     return lexIsQuotedPair();      
	case MailHeaderField::T_FWS:             return lexIsFWS();             
	case MailHeaderField::T_CTEXT:           return lexIsCtext();           
	case MailHeaderField::T_CCONTENT:        return lexIsCcontent();        
	case MailHeaderField::T_COMMENT:         return lexIsComment();         
	case MailHeaderField::T_CFWS:            return lexIsCFWS();            
	case MailHeaderField::T_ATEXT:           return lexIsAtext();           
	case MailHeaderField::T_ATOM:            return lexIsAtom();            
	case MailHeaderField::T_DOT_ATOM:        return lexIsDotAtom();         
	case MailHeaderField::T_DOT_ATOM_TEXT:   return lexIsDotAtomText();     
	case MailHeaderField::T_QTEXT:           return lexIsQtext();           
	case MailHeaderField::T_QCONTENT:        return lexIsQcontent();        
	case MailHeaderField::T_QUOTED_STRING:   return lexIsQuotedString();    
	case MailHeaderField::T_WORD:            return lexIsWord();            
	case MailHeaderField::T_PHRASE:          return lexIsPhrase();          
	case MailHeaderField::T_UNSTRUCTURED:    return lexIsUnstructured();    
	case MailHeaderField::T_DATE_TIME:       return lexIsDateTime();        
	case MailHeaderField::T_DAY_OF_WEEK:     return lexIsDayOfWeek();       
	case MailHeaderField::T_DAY_NAME:        return lexIsDayName();         
	case MailHeaderField::T_DATE:            return lexIsDate();            
	case MailHeaderField::T_DAY:             return lexIsDay();            
	case MailHeaderField::T_MONTH:           return lexIsMonth();           
	case MailHeaderField::T_YEAR:            return lexIsYear();             
	case MailHeaderField::T_TIME:            return lexIsTime();            
	case MailHeaderField::T_TIME_OF_DAY:     return lexIsTimeOfDay();       
	case MailHeaderField::T_HOUR:            return lexIsHour();            
	case MailHeaderField::T_MINUTE:          return lexIsMinute();          
	case MailHeaderField::T_SECOND:          return lexIsSecond();          
	case MailHeaderField::T_ZONE:            return lexIsZone();            
	case MailHeaderField::T_ADDRESS:         return lexIsAddress();         
	case MailHeaderField::T_MAILBOX:         return lexIsMailbox();         
	case MailHeaderField::T_NAME_ADDR:       return lexIsNameAddr();        
	case MailHeaderField::T_ANGLE_ADDR:      return lexIsAngleAddr();       
	case MailHeaderField::T_GROUP:           return lexIsGroup();           
	case MailHeaderField::T_DISPLAY_NAME:    return lexIsDisplayName();     
	case MailHeaderField::T_MAILBOX_LIST:    return lexIsMailboxList();     
	case MailHeaderField::T_ADDRESS_LIST:    return lexIsAddressList();     
	case MailHeaderField::T_GROUP_LIST:      return lexIsGroupList();       
	case MailHeaderField::T_ADDR_SPEC:       return lexIsAddrSpec();        
	case MailHeaderField::T_LOCAL_PART:      return lexIsLocalPart();       
	case MailHeaderField::T_DOMAIN:          return lexIsDomain();          
	case MailHeaderField::T_DOMAIN_LITERAL:  return lexIsDomainLiteral();   
	case MailHeaderField::T_DTEXT:           return lexIsDtext();           
	case MailHeaderField::T_MSG_ID:          return lexIsMsgID();           
	case MailHeaderField::T_ID_LEFT:         return lexIsIDLeft();          
	case MailHeaderField::T_ID_RIGHT:        return lexIsIDRight();         
	case MailHeaderField::T_NO_FOLD_LITERAL: return lexIsNoFoldLiteral();   
	case MailHeaderField::T_PATH:            return lexIsPath();            
	case MailHeaderField::T_RECEIVED_TOKEN:  return lexIsReceivedToken();   
	case MailHeaderField::T_FIELD_NAME:      return lexIsFieldName();       
	case MailHeaderField::T_FTEXT:           return lexIsFtext();           
	case MailHeaderField::T_OBS_NO_WS_CTL:   return lexIsObsNOWSCTL();      
	case MailHeaderField::T_OBS_CTEXT:       return lexIsObsCtext();        
	case MailHeaderField::T_OBS_QTEXT:       return lexIsObsQtext();        
	case MailHeaderField::T_OBS_UTEXT:       return lexIsObsUtext();        
	case MailHeaderField::T_OBS_QP:          return lexIsObsQp();           
	case MailHeaderField::T_OBS_BODY:        return lexIsObsBody();         
	case MailHeaderField::T_OBS_UNSTRUCT:    return lexIsObsUnstruct();     
	case MailHeaderField::T_OBS_PHRASE:      return lexIsObsPhrase();       
	case MailHeaderField::T_OBS_PHRASE_LIST: return lexIsObsPhraseList();   
	case MailHeaderField::T_OBS_FWS:         return lexIsObsFWS();          
	case MailHeaderField::T_OBS_DAY_OF_WEEK: return lexIsObsDayOfWeek();    
	case MailHeaderField::T_OBS_DAY:         return lexIsObsDay();          
	case MailHeaderField::T_OBS_YEAR:        return lexIsObsYear();         
	case MailHeaderField::T_OBS_HOUR:        return lexIsObsHour();         
	case MailHeaderField::T_OBS_MINUTE:      return lexIsObsMinute();       
	case MailHeaderField::T_OBS_SECOND:      return lexIsObsSecond();       
	case MailHeaderField::T_OBS_ZONE:        return lexIsObsZone();         
	case MailHeaderField::T_OBS_ANGLE_ADDR:  return lexIsObsAngleAddr();    
	case MailHeaderField::T_OBS_ROUTE:       return lexIsObsRoute();        
	case MailHeaderField::T_OBS_DOMAIN_LIST: return lexIsObsDomainList();   
	case MailHeaderField::T_OBS_MBOX_LIST:   return lexIsObsMboxList();     
	case MailHeaderField::T_OBS_ADDR_LIST:   return lexIsObsAddrList();     
	case MailHeaderField::T_OBS_GROUP_LIST:  return lexIsObsGroupList();    
	case MailHeaderField::T_OBS_LOCAL_PART:  return lexIsObsLocalPart();    
	case MailHeaderField::T_OBS_DOMAIN:      return lexIsObsDomain();       
	case MailHeaderField::T_OBS_DTEXT:       return lexIsObsDtext();        
	case MailHeaderField::T_OBS_ID_LEFT:     return lexIsObsIDLeft();       
	case MailHeaderField::T_OBS_ID_RIGHT:    return lexIsObsIDRight();      
	}
	return false;
}

/**
 * îCà”ÇÃÉÅÅ[ÉãÉwÉbÉ_çÄñ⁄ÇâêÕÇ∑ÇÈ
 */
bool MailHeaderParser::parseField(MailHeaderField::MHFTYPE fieldType)
{
	std::ios::pos_type beg = 0, end = 0;
	bool isContinue = false, isValidFormat = false;

	/* âêÕèàóùÇÃèÄîıÇÇ∑ÇÈ */
	beg = iss.tellg();
	isContinue = this->listener->hookParse(this->strHeader, fieldType, beg, beg);
	if (! isContinue) {
		return false;
	}

	/* âêÕÇ∑ÇÈ */
	isValidFormat = parseFieldAsDefault(fieldType);

	/* âêÕèàóùÇÃénññÇÇ∑ÇÈ */
	end = iss.tellg();
	isContinue = this->listener->hookParse(this->strHeader, fieldType, beg, end);
	if (! isContinue) {
		iss.seekg(beg, std::ios::beg); // âêÕëOÇÃèÛë‘Ç…ñﬂÇ∑
		return false;
	}
	return isValidFormat;
}

/**
 * <summary>ç≈í∑àÍívÇ∑ÇÈÉtÉBÅ[ÉãÉhÇâêÕÇ∑ÇÈ</summary>
 */
MailHeaderField::MHFTYPE MailHeaderParser::lexIsLongestMatch(
	std::vector<MailHeaderField::MHFTYPE> vecLexingFieldType)
{
	MailHeaderField::MHFTYPE fieldType = MailHeaderField::T_NOUSE_MAX;
	std::ios::pos_type posLongest = std::ios::off_type(-1);
	for (std::vector<MailHeaderField::MHFTYPE>::const_iterator i = vecLexingFieldType.begin(); i != vecLexingFieldType.end(); i++) {
		std::ios::pos_type posBeg = iss.tellg();
		bool success = parseField(*i);
		if (success) {
			if (iss.tellg() > posLongest) {
				posLongest = iss.tellg();
				fieldType = *i;
			}
		}
		iss.seekg(posBeg, std::ios::beg);
	}
	if (fieldType != MailHeaderField::T_NOUSE_MAX) {
		iss.seekg(posLongest, std::ios::beg);
	}
	return fieldType;
}

/**
 * <summary>ÉÅÅ[ÉãÉwÉbÉ_ÇâêÕÇ∑ÇÈ</summary>
 */
bool MailHeaderParser::lexIsHeader()
{
	return (parseField(MailHeaderField::FIELDS) || parseField(MailHeaderField::OBS_FIELDS));
}


/**
 * fields
 * *(trace
 *   *optional-field /
 *   *(resent-date /
 *     resent-from /
 *     resent-sender /
 *     resent-to /
 *     resent-cc /
 *     resent-bcc /
 *     resent-msg-id))
 * *(orig-date /
 *   from /
 *   sender /
 *   reply-to /
 *   to /
 *   cc /
 *   bcc /
 *   message-id /
 *   in-reply-to /
 *   references /
 *   subject /
 *   comments /
 *   keywords /
 *   optional-field)
 */
bool MailHeaderParser::lexIsFields()
{
	while (! iss.eof()) {
		if (! parseField(MailHeaderField::TRACE)) {
			break;
		}
		if (parseField(MailHeaderField::OPTIONAL_FIELD)) {
			while (parseField(MailHeaderField::OPTIONAL_FIELD));
		} else {
			MailHeaderField::MHFTYPE arrMHFTYPE[] = {
				MailHeaderField::RESENT_DATE, 
				MailHeaderField::RESENT_FROM, MailHeaderField::RESENT_SENDER, 
				MailHeaderField::RESENT_TO, MailHeaderField::RESENT_CC, 
				MailHeaderField::RESENT_BCC, MailHeaderField::RESENT_MSGID
			};
			std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
				&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
			while (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
			//while (parseField(MailHeaderField::RESENT_DATE)
			//	  || parseField(MailHeaderField::RESENT_FROM)
			//	  || parseField(MailHeaderField::RESENT_SENDER)
			//	  || parseField(MailHeaderField::RESENT_TO)
			//	  || parseField(MailHeaderField::RESENT_CC)
			//	  || parseField(MailHeaderField::RESENT_BCC)
			//	  || parseField(MailHeaderField::RESENT_MSGID));
		}
	}

	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::ORIG_DATE, MailHeaderField::FROM,
		MailHeaderField::SENDER, MailHeaderField::REPLY_TO, MailHeaderField::TO,
		MailHeaderField::CC, MailHeaderField::BCC, MailHeaderField::MESSAGE_ID,
		MailHeaderField::INREPLY_TO, MailHeaderField::REFERENCES,
		MailHeaderField::SUBJECT, MailHeaderField::COMMENTS,
		MailHeaderField::KEYWORDS
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	while ((lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX)
		|| parseField(MailHeaderField::OPTIONAL_FIELD));
	//while (parseField(MailHeaderField::ORIG_DATE)
	//	  || parseField(MailHeaderField::FROM)
	//	  || parseField(MailHeaderField::SENDER)
	//	  || parseField(MailHeaderField::REPLY_TO)
	//	  || parseField(MailHeaderField::TO)
	//	  || parseField(MailHeaderField::CC)
	//	  || parseField(MailHeaderField::BCC)
	//	  || parseField(MailHeaderField::MESSAGE_ID)
	//	  || parseField(MailHeaderField::INREPLY_TO)
	//	  || parseField(MailHeaderField::REFERENCES)
	//	  || parseField(MailHeaderField::SUBJECT)
	//	  || parseField(MailHeaderField::COMMENTS)
	//	  || parseField(MailHeaderField::KEYWORDS)
	//	  || parseField(MailHeaderField::OPTIONAL_FIELD));
	return true;
}

/**
 * <summary>îƒópÉtÉBÅ[ÉãÉhâêÕÉÅÉ\ÉbÉh</summary>
 */
bool MailHeaderParser::lexIsGeneralField(std::string strFieldName, 
										 std::vector<MailHeaderField::MHFTYPE> vecFieldType)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[64];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(strFieldName.c_str()));
	if (strcmp(buf, strFieldName.c_str()) == 0) {
		for (std::vector<MailHeaderField::MHFTYPE>::iterator i = vecFieldType.begin(); i != vecFieldType.end(); i++) {
			bool success = parseField(*i);
			if (! success) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * orig-date
 * "Date:" date-time CRLF
 */
bool MailHeaderParser::lexIsOrigDate()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DATE_TIME, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Date:", vecMHFTYPE);
}

/**
 * from
 * "From:" mailbox-list CRLF
 */
bool MailHeaderParser::lexIsFrom()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("From:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::FROM);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MAILBOX_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * sender
 * "Sender:" mailbox CRLF
 */
bool MailHeaderParser::lexIsSender()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Sender:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::SENDER);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MAILBOX)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * reply-to
 * "Reply-To:" address-list CRLF
 */
bool MailHeaderParser::lexIsReplyTo()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Reply-To:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::REPLY_TO);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MAILBOX_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * to
 * "To:" address-list CRLF
 */
bool MailHeaderParser::lexIsTo()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ADDRESS_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("To:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::TO);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_ADDRESS_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * cc
 * "Cc:" address-list CRLF
 */
bool MailHeaderParser::lexIsCc()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ADDRESS_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Cc:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::CC);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_ADDRESS_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * address-list / CFWS
 */
bool MailHeaderParser::lexIsAddressListOrCFWS()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ADDRESS_LIST, MailHeaderField::T_CFWS
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
}


/**
 * bcc
 * "Bcc:" [address-list / CFWS] CRLF
 */
bool MailHeaderParser::lexIsBcc()
{
	std::ios::pos_type pos = iss.tellg();
	std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::BCC);
	char buf[64];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(strFieldName.c_str()));
	if (strcmp(buf, strFieldName.c_str()) == 0
		&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)) {
		parseField(MailHeaderField::T_ADDRESS_LIST_OR_CFWS);
		if (! parseField(MailHeaderField::T_CRLF)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * message-id
 * "Message-ID:" msg-id CRLF
 */
bool MailHeaderParser::lexIsMessageID()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MSG_ID, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Message-ID:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::MESSAGE_ID);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MSG_ID)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * 1*msg-id
 */
bool MailHeaderParser::lexIsNMsgID()
{
	int numMsgID = 0;
	std::ios::pos_type pos = iss.tellg();
	while (parseField(MailHeaderField::T_MSG_ID)) {
		numMsgID++;
	}
	if (numMsgID == 0) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * in-reply-to
 * "In-Reply-To:" 1*msg-id CRLF
 */
bool MailHeaderParser::lexIsInReplyTo()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_N_MSG_ID, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("In-Reply-To:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::INREPLY_TO);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_N_MSG_ID)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * references
 * "References:" 1*msg-id CRLF
 */
bool MailHeaderParser::lexIsReferences()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_N_MSG_ID, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("References:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::REFERENCES);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_N_MSG_ID)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * subject
 * "Subject:" unstructured CRLF
 */
bool MailHeaderParser::lexIsSubject()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_UNSTRUCTURED, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Subject:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::SUBJECT);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_UNSTRUCTURED)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * comments
 * "Comments:" unstructured CRLF
 */
bool MailHeaderParser::lexIsComments()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_UNSTRUCTURED, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Comments:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::COMMENTS);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_UNSTRUCTURED)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * phrase *("," phrase)
 */
bool MailHeaderParser::lexIsPhraseList()
{
	std::ios::pos_type pos = iss.tellg();
	if (! parseField(MailHeaderField::T_PHRASE)) {
		return false;
	}
	while (iss.peek() == ',' && iss.seekg(1, std::ios::cur)) {
		if (! parseField(MailHeaderField::T_PHRASE)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
	}
	return true;
}

/**
 * keywords
 * "Keywords:" phrase *("," phrase) CRLF
 */
bool MailHeaderParser::lexIsKeywords()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_PHRASE_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Keywords:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::KEYWORDS);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_PHRASE_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-date
 * "Resent-Date:" date-time CRLF
 */
bool MailHeaderParser::lexIsResentDate()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DATE_TIME, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-Date:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_DATE);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_DATE_TIME)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-from
 * "Resent-From:" mailbox-list CRLF
 */
bool MailHeaderParser::lexIsResentFrom()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-From:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_FROM);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MAILBOX_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-sender
 * "Resent-Sender:" mailbox CRLF
 */
bool MailHeaderParser::lexIsResentSender()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-Sender:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_SENDER);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MAILBOX)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-to
 * "Resent-To:" address-list CRLF
 */
bool MailHeaderParser::lexIsResentTo()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ADDRESS_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-To:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_TO);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_ADDRESS_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-cc
 * "Resent-Cc:" address-list CRLF
 */
bool MailHeaderParser::lexIsResentCc()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ADDRESS_LIST, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-Cc:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_CC);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_ADDRESS_LIST)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * resent-bcc
 * "Resent-Bcc:" [address-list / CFWS] CRLF
 */
bool MailHeaderParser::lexIsResentBcc()
{
	std::ios::pos_type pos = iss.tellg();
	std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_BCC);
	char buf[64];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(strFieldName.c_str()));
	if (strcmp(buf, strFieldName.c_str()) == 0
		&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)) {
		parseField(MailHeaderField::T_ADDRESS_LIST_OR_CFWS);
		if (! parseField(MailHeaderField::T_CRLF)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-msg-id
 * "Resent-Message-ID:" msg-id CRLF
 */
bool MailHeaderParser::lexIsResentMsgID()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MSG_ID, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Resent-Message-ID:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RESENT_MSGID);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_MSG_ID)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * trace
 * [return] 1*received
 */
bool MailHeaderParser::lexIsTrace()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::RETURN);
	if (! parseField(MailHeaderField::RECEIVED)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (parseField(MailHeaderField::RECEIVED));
	return true;
}

/**
 * return
 * "Return-Path:" path CRLF
 */
bool MailHeaderParser::lexIsReturn()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_PATH, MailHeaderField::T_CRLF
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0]) - 1]);
	return lexIsGeneralField("Return-Path:", vecMHFTYPE);
	//std::ios::pos_type pos = iss.tellg();
	//std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RETURN);
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, strlen(strFieldName.c_str()));
	//if (strcmp(buf, strFieldName.c_str()) == 0
	//	&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
	//	&& parseField(MailHeaderField::T_PATH)
	//	&& parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * *received-token ";" date-time
 */
bool MailHeaderParser::lexIsNReceivedTokenAndDateTime()
{
	std::ios::pos_type pos = iss.tellg();
	while (parseField(MailHeaderField::T_RECEIVED_TOKEN));
	if (iss.peek() == ';' && iss.seekg(1, std::ios::cur)) {
		if (parseField(MailHeaderField::T_DATE_TIME)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * received
 * "Received:" *received-token ";" date-time CRLF
 */
bool MailHeaderParser::lexIsReceived()
{
	std::ios::pos_type pos = iss.tellg();
	std::string strFieldName = MailHeaderField::getFieldName(MailHeaderField::RECEIVED);
	char buf[64];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(strFieldName.c_str()));
	if (strcmp(buf, strFieldName.c_str()) == 0
		&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
		&& parseField(MailHeaderField::T_N_RECEIVED_TOKEN_AND_DATE_TIME)
		&& parseField(MailHeaderField::T_CRLF)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * optional-field
 * field-name ":" unstructured CRLF
 */
bool MailHeaderParser::lexIsOptionalField()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_FIELD_NAME)
		&& iss.peek() == ':' && iss.seekg(1, std::ios::cur)
		&& parseField(MailHeaderField::T_UNSTRUCTURED)
		&& parseField(MailHeaderField::T_CRLF)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-fields
 * *(obs-return, obs-received, obs-orig-date, obs-from, obs-sender,
 *   obs-reply-to, obs-to, obs-cc, obs-bcc, obs-message-id, 
 *   obs-in-reply-to, obs-references, obs-subject, obs-comments, 
 *   obs-keywords, obs-resent-date, obs-resent-from, obs-resent-send, 
 *   obs-resent-rply, obs-resent-to, obs-resent-cc, obs-resent-bcc,
 *   obs-resent-mid, obs-optional)
 */
bool MailHeaderParser::lexIsObsFields()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::OBS_RETURN,
		MailHeaderField::OBS_RECEIVED,
		MailHeaderField::OBS_ORIG_DATE,
		MailHeaderField::OBS_FROM,
		MailHeaderField::OBS_SENDER,
		MailHeaderField::OBS_INREPLY_TO,
		MailHeaderField::OBS_TO,
		MailHeaderField::OBS_CC,
		MailHeaderField::OBS_BCC,
		MailHeaderField::OBS_MESSAGE_ID,
		MailHeaderField::OBS_INREPLY_TO,
		MailHeaderField::OBS_REFERENCES,
		MailHeaderField::OBS_SUBJECT,
		MailHeaderField::OBS_COMMENTS,
		MailHeaderField::OBS_KEYWORDS,
		MailHeaderField::OBS_RESENT_DATE,
		MailHeaderField::OBS_RESENT_FROM,
		MailHeaderField::OBS_RESENT_SEND,
		MailHeaderField::OBS_RESENT_RPLY,
		MailHeaderField::OBS_RESENT_TO,
		MailHeaderField::OBS_RESENT_CC,
		MailHeaderField::OBS_RESENT_BCC,
		MailHeaderField::OBS_RESENT_MID
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	while (lexIsLongestMatch(vecMHFTYPE)
		|| parseField(MailHeaderField::OBS_OPTIONAL));
	//while (parseField(MailHeaderField::OBS_RETURN)
	//	  || parseField(MailHeaderField::OBS_RECEIVED)
	//	  || parseField(MailHeaderField::OBS_ORIG_DATE)
	//	  || parseField(MailHeaderField::OBS_FROM)
	//	  || parseField(MailHeaderField::OBS_SENDER)
	//	  || parseField(MailHeaderField::OBS_INREPLY_TO)
	//	  || parseField(MailHeaderField::OBS_TO)
	//	  || parseField(MailHeaderField::OBS_CC)
	//	  || parseField(MailHeaderField::OBS_BCC)
	//	  || parseField(MailHeaderField::OBS_MESSAGE_ID)
	//	  || parseField(MailHeaderField::OBS_INREPLY_TO)
	//	  || parseField(MailHeaderField::OBS_REFERENCES)
	//	  || parseField(MailHeaderField::OBS_SUBJECT)
	//	  || parseField(MailHeaderField::OBS_COMMENTS)
	//	  || parseField(MailHeaderField::OBS_KEYWORDS)
	//	  || parseField(MailHeaderField::OBS_RESENT_DATE)
	//	  || parseField(MailHeaderField::OBS_RESENT_FROM)
	//	  || parseField(MailHeaderField::OBS_RESENT_SEND)
	//	  || parseField(MailHeaderField::OBS_RESENT_RPLY)
	//	  || parseField(MailHeaderField::OBS_RESENT_TO)
	//	  || parseField(MailHeaderField::OBS_RESENT_CC)
	//	  || parseField(MailHeaderField::OBS_RESENT_BCC)
	//	  || parseField(MailHeaderField::OBS_RESENT_MID)
	//	  || parseField(MailHeaderField::OBS_OPTIONAL));
	return true;
}

/**
 * obs**
 * "XXXX" *WSP ":" YYYY CRLF
 */
bool MailHeaderParser::lexIsObsHeader(const char *szHeader, MailHeaderField::MHFTYPE type)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(szHeader));
	if (strcmp(buf, szHeader) == 0) {
		while (parseField(MailHeaderField::T_WSP));
		if (! (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		if (! (parseField(type) && parseField(MailHeaderField::T_CRLF))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-orig-date
 * "Date" *WSP ":" date-time CRLF
 */
bool MailHeaderParser::lexIsObsOrigDate()
{
	return lexIsObsHeader("Date", MailHeaderField::T_DATE_TIME);
}

/**
 * obs-from
 * "From" *WSP ":" mailbox-list CRLF
 */
bool MailHeaderParser::lexIsObsFrom()
{
	return lexIsObsHeader("From", MailHeaderField::T_MAILBOX_LIST);
}

/**
 * obs-sender
 * "Sender" *WSP ":" mailbox CRLF
 */
bool MailHeaderParser::lexIsObsSender()
{
	return lexIsObsHeader("Sender", MailHeaderField::T_MAILBOX);
}

/**
 * obs-reply-to
 * "Reply-To" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsReplyTo()
{
	return lexIsObsHeader("Reply-To", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * obs-to
 * "To" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsTo()
{
	return lexIsObsHeader("To", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * obs-cc
 * "Cc" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsCc()
{
	return lexIsObsHeader("Cc", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * (address-list / (*([CFWS] ",") [CFWS]))
 */
bool MailHeaderParser::lexIsAddreeListOrCFWSList()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* address-list */
	std::ios::pos_type posEndAddressList = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_ADDRESS_LIST)) {
		posEndAddressList = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* (*([CFWS] ",") [CFWS]) */
	std::ios::pos_type posEndCFWSList = std::ios::off_type(-1);
	while (! iss.eof()) {
		parseField(MailHeaderField::T_CFWS);
		if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
			break;
		}
	}
	parseField(MailHeaderField::T_CFWS);
	if (iss.tellg() > posBeg) {
		posEndCFWSList = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndAddressList != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndCFWSList != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndAddressList, posEndCFWSList), std::ios::beg);
		return true;
	}
	return false;
}

/**
 * obs-bcc
 * "Bcc" *WSP ":" (address-list / (*([CFWS] ",") [CFWS])) CRLF
 */
bool MailHeaderParser::lexIsObsBcc()
{
	return lexIsObsHeader("Bcc", MailHeaderField::T_ADDREE_LIST_OR_CFWS_LIST);
}

/**
 * obs-message-id
 * "Message-ID" *WSP ":" msg-id CRLF
 */
bool MailHeaderParser::lexIsObsMessageID()
{
	return lexIsObsHeader("Message-ID", MailHeaderField::T_MSG_ID);
}

/**
 * (phrase / msg-id)
 */
bool MailHeaderParser::lexIsPhraseOrMsgID()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_PHRASE, MailHeaderField::T_MSG_ID
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_PHRASE) || parseField(MailHeaderField::T_MSG_ID)) {
	//	return true;
	//}
	//return false;
}

/**
 * *(phrase / msg-id)
 */
bool MailHeaderParser::lexIsNPhraseOrMsgID()
{
	while (parseField(MailHeaderField::T_PHRASE_OR_MSGID));
	return true;
}

/**
 * obs-in-reply-to
 * "In-Reply-To" *WSP ":" *(phrase / msg-id) CRLF
 */
bool MailHeaderParser::lexIsObsInReplyTo()
{
	return lexIsObsHeader("In-Reply-To", MailHeaderField::T_N_PHRASE_OR_MSGID);
}

/**
 * obs-references
 * "References" *WSP ":" *(phrase / msg-id) CRLF
 */
bool MailHeaderParser::lexIsObsReferences()
{
	return lexIsObsHeader("References", MailHeaderField::T_N_PHRASE_OR_MSGID);
}

/**
 * obs-subject
 * "Subject" *WSP ":" unstructured CRLF
 */
bool MailHeaderParser::lexIsObsSubject()
{
	return lexIsObsHeader("Subject", MailHeaderField::T_UNSTRUCTURED);
}

/**
 * obs-comments
 * "Comments" *WSP ":" unstructured CRLF
 */
bool MailHeaderParser::lexIsObsComments()
{
	return lexIsObsHeader("Comments", MailHeaderField::T_UNSTRUCTURED);
}

/**
 * obs-keywords
 * "Keywords" *WSP ":" obs-phrase-list CRLF
 */
bool MailHeaderParser::lexIsObsKeywords()
{
	return lexIsObsHeader("Keywords", MailHeaderField::T_OBS_PHRASE_LIST);
}

/**
 * obs-resent-from
 * "Resent-From" *WSP ":" mailbox-list CRLF
 */
bool MailHeaderParser::lexIsObsResentFrom()
{
	return lexIsObsHeader("Resent-From", MailHeaderField::T_MAILBOX_LIST);
}

/**
 * obs-resent-send
 * "Resent-Sender" *WSP ":" mailbox CRLF
 */
bool MailHeaderParser::lexIsObsResentSend()
{
	return lexIsObsHeader("Resent-Sender", MailHeaderField::T_MAILBOX);
}

/**
 * obs-resent-date
 * "Resent-Date" *WSP ":" date-time CRLF
 */
bool MailHeaderParser::lexIsObsResentDate()
{
	return lexIsObsHeader("Resent-Date", MailHeaderField::T_DATE_TIME);
}

/**
 * obs-resent-to
 * "Resent-To" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsResentTo()
{
	return lexIsObsHeader("Resent-To", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * obs-resent-cc
 * "Resent-Cc" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsResentCc()
{
	return lexIsObsHeader("Resent-Cc", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * obs-resent-bcc
 * "Resent-Bcc" *WSP ":" (address-list / (*([CFWS] ",") [CFWS])) CRLF
 */
bool MailHeaderParser::lexIsObsResentBcc()
{
	return lexIsObsHeader("Resent-Bcc", MailHeaderField::T_ADDREE_LIST_OR_CFWS_LIST);
}

/**
 * obs-resent-mid
 * "Resent-Message-ID" *WSP ":" msg-id CRLF
 */
bool MailHeaderParser::lexIsObsResentMid()
{
	return lexIsObsHeader("Resent-Message-ID", MailHeaderField::T_MSG_ID);
}

/**
 * obs-resent-rply
 * "Resent-Reply-To" *WSP ":" address-list CRLF
 */
bool MailHeaderParser::lexIsObsResentRply()
{
	return lexIsObsHeader("Resent-Reply-To", MailHeaderField::T_ADDRESS_LIST);
}

/**
 * obs-return
 * "Return-Path" *WSP ":" path CRLF
 */
bool MailHeaderParser::lexIsObsReturn()
{
	return lexIsObsHeader("Return-Path", MailHeaderField::T_PATH);
}

/**
 * N received-token
 * *received-token
 */
bool MailHeaderParser::lexIsNReceivedToken()
{
	while (parseField(MailHeaderField::T_RECEIVED_TOKEN));
	return true;
}

/**
 * obs-received
 * "Received" *WSP ":" *received-token CRLF
 */
bool MailHeaderParser::lexIsObsReceived()
{
	return lexIsObsHeader("Received", MailHeaderField::T_N_RECEIVED_TOKEN);
}

/**
 * obs-optional
 * field-name *WSP ":" unstructured CRLF
 */
bool MailHeaderParser::lexIsObsOptional()
{
	std::ios::pos_type pos = iss.tellg();
	if (! parseField(MailHeaderField::T_FIELD_NAME)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (parseField(MailHeaderField::T_WSP));
	if (! (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (! (parseField(MailHeaderField::T_UNSTRUCTURED) && parseField(MailHeaderField::T_CRLF))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}



/**
 * DQUOTE
 */
bool MailHeaderParser::lexIsDQUOTE()
{
	return (iss.peek() == '\"' && iss.seekg(1, std::ios::cur));
}

/**
 * SP
 */
bool MailHeaderParser::lexIsSP()
{
	return (iss.peek() == ' ' && iss.seekg(1, std::ios::cur));
}

/**
 * HTAB
 */
bool MailHeaderParser::lexIsHTAB()
{
	return (iss.peek() == '\t' && iss.seekg(1, std::ios::cur));
}

/**
 * WSP
 * SP / HTAB
 */
bool MailHeaderParser::lexIsWSP()
{
	return (parseField(MailHeaderField::T_SP) || parseField(MailHeaderField::T_HTAB));
}

/**
 * CRLF
 */
bool MailHeaderParser::lexIsCRLF()
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "\r\n") != 0) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * ALPHA
 */
bool MailHeaderParser::lexIsALPHA()
{
	return (isalpha(iss.peek()) != 0 && iss.seekg(1, std::ios::cur));
}

/**
 * DIGIT
 */
bool MailHeaderParser::lexIsDIGIT()
{
	return (isdigit(iss.peek()) != 0 && iss.seekg(1, std::ios::cur));
}

/**
 * VCHAR
 * %x21-7E  ; â¬éã (àÛéö) ï∂éö  = %d33-126
 */
bool MailHeaderParser::lexIsVCHAR()
{
	int n = iss.peek();
	return (0x21 <= n && n <= 0x7E && iss.seekg(1, std::ios::cur));
}

/**
 * NO-WS-CTL
 * %d1-8 / %d11 / %d12 / %d14-31 / %d127
 */
bool MailHeaderParser::lexIsNOWSCTL()
{
	const char *szWS = "\t\n\r";
	int n = iss.peek();
	if ((1 <= n && n <= 31 && strchr(szWS, n) == NULL) || n == 127) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * text
 * %d1-9 / %d11 / %d12 / %d14-127
 */
bool MailHeaderParser::lexIsText()
{
	int n = iss.peek();
	if (1 <= n && n <= 127 && n != '\n' && n != '\r' && iss.seekg(1, std::ios::cur)) {
		return true;
	}
	return false;
}

/**
 * specials
 * "(" / ")" / "<" / ">" / "[" / "]" /
 * ":" / ";" / "@" / "\" / "," / "." / DQUOTE
 */
bool MailHeaderParser::lexIsSpecials()
{
	const char *szSpecials = "()<>[]:;@\\,.";
	if ((strchr(szSpecials, iss.peek()) != NULL && iss.seekg(1, std::ios::cur))
		|| parseField(MailHeaderField::T_DQUOTE)) {
		return true;
	}
	return false;
}

/**
 * quoted-pair
 * ("\" (VCHAR / WSP)) / obs-qp
 */
bool MailHeaderParser::lexIsQuotedPair()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-qp */
	std::ios::pos_type posEndOBSQP = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_QP)) {
		posEndOBSQP = iss.tellg();
		iss.seekg(posBeg);
	}

	/* ("\" (VCHAR / WSP)) */
	std::ios::pos_type posEndVCHARorWSP = std::ios::off_type(-1);
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_VCHAR, MailHeaderField::T_WSP
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	if (iss.peek() == '\\' && iss.seekg(1, std::ios::cur)
	   && lexIsLongestMatch(vecMHFTYPE)) {
		posEndVCHARorWSP = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSQP != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndVCHARorWSP != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSQP, posEndVCHARorWSP), std::ios::beg);
		return true;
	}
	return false;
	//if (lexIsLongestMatch(vecMHFTYPE)) && parseField(MailHeaderField::T_CRLF)) {
	//	return true;
	//}
	//return false
}

/**
 * FWS
 * ([*WSP CRLF] 1*WSP) / obs-FWS
 */
bool MailHeaderParser::lexIsFWS()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-FWS */
	std::ios::pos_type posEndOBSFWS = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_FWS)) {
		posEndOBSFWS = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ([*WSP CRLF] 1*WSP) */
	std::ios::pos_type posEndWSPList = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_WSP)) {
		while (parseField(MailHeaderField::T_WSP));
		if (! parseField(MailHeaderField::T_CRLF)) {
			if (posEndOBSFWS != std::ios::pos_type(std::ios::off_type(-1))) {
				iss.seekg(posEndOBSFWS, std::ios::beg);
				return true;
			}
			iss.seekg(posBeg, std::ios::beg);
			return false;
		}
		if (! parseField(MailHeaderField::T_WSP)) {
			if (posEndOBSFWS != std::ios::pos_type(std::ios::off_type(-1))) {
				iss.seekg(posEndOBSFWS, std::ios::beg);
				return true;
			}
			iss.seekg(posBeg, std::ios::beg);
			return false;
		}
		while (parseField(MailHeaderField::T_WSP));
		posEndWSPList = iss.tellg();
	} else if (parseField(MailHeaderField::T_CRLF)) {
		if (! parseField(MailHeaderField::T_WSP)) {
			if (posEndOBSFWS != std::ios::pos_type(std::ios::off_type(-1))) {
				iss.seekg(posEndOBSFWS, std::ios::beg);
				return true;
			}
			iss.seekg(posBeg, std::ios::beg);
			return false;
		}
		while (parseField(MailHeaderField::T_WSP));
		posEndWSPList = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSFWS != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndWSPList != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSFWS, posEndWSPList));
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_OBS_FWS)) {
	//	return true;
	//} else {
	//	if (parseField(MailHeaderField::T_WSP)) {
	//		while (parseField(MailHeaderField::T_WSP));
	//		if (! parseField(MailHeaderField::T_CRLF)) {
	//			iss.seekg(pos, std::ios::beg);
	//			return false;
	//		}
	//		if (! parseField(MailHeaderField::T_WSP)) {
	//			iss.seekg(pos, std::ios::beg);
	//			return false;
	//		}
	//		while (parseField(MailHeaderField::T_WSP));
	//		return true;
	//	} else if (parseField(MailHeaderField::T_CRLF)) {
	//		if (! parseField(MailHeaderField::T_WSP)) {
	//			iss.seekg(pos, std::ios::beg);
	//			return false;
	//		}
	//		while (parseField(MailHeaderField::T_WSP));
	//		return true;
	//	}
	//	iss.seekg(pos, std::ios::beg);
	//	return false;
	//}
	//return false;
}

/**
 * ctext
 * %d33-39 / %d42-91 / %d93-126 / obs-ctext
 */
bool MailHeaderParser::lexIsCtext()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-ctext */
	std::ios::pos_type posEndOBSCTEXT = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_CTEXT)) {
		posEndOBSCTEXT = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* %d33-39 / %d42-91 / %d93-126 */
	std::ios::pos_type posEndNotOBSCTEXT = std::ios::off_type(-1);
	int n = iss.peek();
	if (33 <= n && n <= 126 && strchr("()\\", n) == NULL) {
		iss.seekg(1, std::ios::cur);
		posEndNotOBSCTEXT = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSCTEXT != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndNotOBSCTEXT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSCTEXT, posEndNotOBSCTEXT));
		return true;
	}
	return false;

	//int n = iss.peek();
	//if (33 <= n && n <= 126 && strchr("()\\", n) == NULL) {
	//	iss.seekg(1, std::ios::cur);
	//	return true;
	//} else if (parseField(MailHeaderField::T_OBS_CTEXT)) {
	//	return true;
	//}
	//return false;
}

/**
 * ccontent
 * ctext / quoted-pair / comment
 */
bool MailHeaderParser::lexIsCcontent()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_CTEXT, MailHeaderField::T_QUOTED_PAIR, 
		MailHeaderField::T_COMMENT
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);

	//if (parseField(MailHeaderField::T_CTEXT) 
	//   && parseField(MailHeaderField::T_QUOTED_PAIR)
	//   && parseField(MailHeaderField::T_COMMENT)) {
	//	return true;
	//}
	//return false;
}

/**
 * comment
 * "(" *([FWS] ccontent) [FWS] ")"
 */
bool MailHeaderParser::lexIsComment()
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '(' && iss.seekg(1, std::ios::cur)) {
		while (! iss.eof()) {
			parseField(MailHeaderField::T_FWS);
			if (iss.peek() == ')') {
				iss.seekg(1, std::ios::cur);
				return true;
			} else if (! parseField(MailHeaderField::T_CCONTENT)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * CFWS
 * (1*([FWS] comment) [FWS]) / FWS
 */
bool MailHeaderParser::lexIsCFWS()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_FWS)) {
		if (! parseField(MailHeaderField::T_COMMENT)) {
			return true;
		}
		while (! iss.eof()) {
			parseField(MailHeaderField::T_FWS);
			if (! parseField(MailHeaderField::T_COMMENT)) {
				break;
			}
		}
		parseField(MailHeaderField::T_FWS);
		return true;
	} else {
		if (! parseField(MailHeaderField::T_COMMENT)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		while (! iss.eof()) {
			parseField(MailHeaderField::T_FWS);
			if (! parseField(MailHeaderField::T_COMMENT)) {
				break;
			}
		}
		parseField(MailHeaderField::T_FWS);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * atext
 * ALPHA / DIGIT / "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" /
 * "/" / "=" / "?" / "^" / "_" / "`" / "{" / "|" / "}" / "~"
 */
bool MailHeaderParser::lexIsAtext()
{
	const char *szTextPart = "!#$%&'*+-/=?^_`{|}~";
	if (parseField(MailHeaderField::T_ALPHA) || parseField(MailHeaderField::T_DIGIT)) {
		return true;
	} else if (strchr(szTextPart, iss.peek()) != NULL) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * atom
 * [CFWS] 1*atext [CFWS]
 */
bool MailHeaderParser::lexIsAtom()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! parseField(MailHeaderField::T_ATEXT)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (parseField(MailHeaderField::T_ATEXT));
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * dot-atom
 * [CFWS] dot-atom-text [CFWS]
 */
bool MailHeaderParser::lexIsDotAtom()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! parseField(MailHeaderField::T_DOT_ATOM_TEXT)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * dot-atom-text
 * 1*atext *("." 1*atext)
 */
bool MailHeaderParser::lexIsDotAtomText()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_ATEXT)) {
		while (parseField(MailHeaderField::T_ATEXT));
		while (! iss.eof()) {
			if (! (iss.peek() == '.' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			if (! parseField(MailHeaderField::T_ATEXT)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (parseField(MailHeaderField::T_ATEXT));
		}
		return true;
	}
	return false;
}

/**
 * qtext
 * %d33 / %d35-91 / %d93-126 / obs-qtext  ; %d34 = '"', %d92 = '\'
 */
bool MailHeaderParser::lexIsQtext()
{
	int n = iss.peek();
	if (33 <= n && n <= 126 && n != '\"' && n != '\\') {
		iss.seekg(1, std::ios::cur);
		return true;
	} else if (parseField(MailHeaderField::T_OBS_QTEXT)) {
		return true;
	}
	return false;
}

/**
 * qcontent
 * qtext / quoted-pair
 */
bool MailHeaderParser::lexIsQcontent()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_QTEXT, MailHeaderField::T_QUOTED_PAIR
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_QTEXT) || parseField(MailHeaderField::T_QUOTED_PAIR)) {
	//	return true;
	//}
	//return false;
}

/**
 * quoted-string
 * [CFWS] DQUOTE *([FWS] qcontent) [FWS] DQUOTE [CFWS]
 */
bool MailHeaderParser::lexIsQuotedString()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (parseField(MailHeaderField::T_DQUOTE)) {
		while (! iss.eof()) {
			parseField(MailHeaderField::T_FWS);
			if (! parseField(MailHeaderField::T_QCONTENT)) {
				break;
			}
		}
		parseField(MailHeaderField::T_FWS);
		if (! parseField(MailHeaderField::T_DQUOTE)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		parseField(MailHeaderField::T_CFWS);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * word
 * atom / quoted-string
 */
bool MailHeaderParser::lexIsWord()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_ATOM, MailHeaderField::T_QUOTED_STRING
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_ATOM) || parseField(MailHeaderField::T_QUOTED_STRING)) {
	//	return true;
	//}
	//return false;
}

/**
 * phrase
 * 1*word / obs-phrase
 */
bool MailHeaderParser::lexIsPhrase()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-phrase */
	std::ios::pos_type posEndOBSPhrase = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_PHRASE)) {
		posEndOBSPhrase = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* 1*word */
	std::ios::pos_type posEndNWord = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_WORD)) {
		while (parseField(MailHeaderField::T_WORD));
		posEndNWord = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSPhrase != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndNWord != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSPhrase, posEndNWord));
		return true;
	}
	return false;

	//std::ios::pos_type posBeg = iss.tellg();
	//std::ios::pos_type posEndNWord = 0;
	//if (parseField(MailHeaderField::T_WORD)) {
	//	while (parseField(MailHeaderField::T_WORD));
	//	posEndWord = iss.tellg();
	//}
	//if (parseField(MailHeaderField::T_OBS_PHRASE)) {
	//	if (posEndWord != 0 && iss.tellg() > posEndWord) {
	//		iss.seekg(posEndNWord, std::ios::beg);
	//	}
	//	return true;
	//}
	//if (posEndNWord != 0) {
	//	iss.seekg(posEndNWord);
	//	return true;
	//}
	//return false;
}

/**
 * unstructured
 * (*([FWS] VCHAR) *WSP) / obs-unstruct
 */
bool MailHeaderParser::lexIsUnstructured()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-unstruct */
	std::ios::pos_type posEndOBSUnstruct = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_UNSTRUCT)) {
		posEndOBSUnstruct = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* (*([FWS] VCHAR) *WSP) */
	std::ios::pos_type posEndVCHARList = std::ios::off_type(-1);
	while (! iss.eof()) {
		if (parseField(MailHeaderField::T_FWS)) {
			if (! parseField(MailHeaderField::T_VCHAR)) {
				if (posEndOBSUnstruct != std::ios::pos_type(std::ios::off_type(-1))) {
					iss.seekg(posEndOBSUnstruct, std::ios::beg);
				}
				return false;
			}
		} else {
			if (! parseField(MailHeaderField::T_VCHAR)) {
				break;
			}
		}
	}
	while (parseField(MailHeaderField::T_WSP));
	if (iss.tellg() > posBeg) {
		posEndVCHARList = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSUnstruct != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndVCHARList != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSUnstruct, posEndVCHARList));
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//while (! iss.eof()) {
	//	if (parseField(MailHeaderField::T_FWS)) {
	//		if (! parseField(MailHeaderField::T_VCHAR)) {
	//			iss.seekg(pos, std::ios::beg);
	//			return parseField(MailHeaderField::T_OBS_UNSTRUCT);
	//		}
	//	} else {
	//		if (! parseField(MailHeaderField::T_VCHAR)) {
	//			break;
	//		}
	//	}
	//}
	//while (parseField(MailHeaderField::T_WSP));
	//return true;
}

/**
 * date-time
 * [ day-of-week "," ] date time [CFWS]
 */
bool MailHeaderParser::lexIsDateTime()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_DAY_OF_WEEK)) {
		if (iss.peek() != ',') {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		iss.seekg(1, std::ios::cur);
	}
	if (parseField(MailHeaderField::T_DATE) && parseField(MailHeaderField::T_TIME)) {
		parseField(MailHeaderField::T_CFWS);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * day-of-week
 * ([FWS] day-name) / obs-day-of-week
 */
bool MailHeaderParser::lexIsDayOfWeek()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-day-of-week */
	std::ios::pos_type posEndOBSDayOfWeek = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_DAY_OF_WEEK)) {
		posEndOBSDayOfWeek = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ([FWS] day-name) */
	std::ios::pos_type posEndDayName = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_FWS)) {
		if (! parseField(MailHeaderField::T_DAY_NAME)) {
			if (posEndOBSDayOfWeek != std::ios::pos_type(std::ios::off_type(-1))) {
				iss.seekg(posEndOBSDayOfWeek, std::ios::beg);
				return true;
			}
			return false;
		}
		posEndDayName = iss.tellg();
	} else {
		if (parseField(MailHeaderField::T_DAY_NAME)) {
			posEndDayName = iss.tellg();
		}
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSDayOfWeek != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndDayName != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSDayOfWeek, posEndDayName));
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_FWS)) {
	//	if (parseField(MailHeaderField::T_DAY_NAME)) {
	//		return true;
	//	}
	//} else {
	//	if (parseField(MailHeaderField::T_DAY_NAME)) {
	//		return true;
	//	} else if (parseField(MailHeaderField::T_OBS_DAY_OF_WEEK)) {
	//		return true;
	//	}
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * day-name
 * "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"
 */
bool MailHeaderParser::lexIsDayName()
{
	const char* szDayName[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	for (int i = 0; i < sizeof(szDayName) / sizeof(szDayName[0]); i++) {
		if (strncmp(szDayName[i], buf, strlen(szDayName[i])) == 0) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * date
 * day month year
 */
bool MailHeaderParser::lexIsDate()
{
	if (parseField(MailHeaderField::T_DAY) && parseField(MailHeaderField::T_MONTH) && parseField(MailHeaderField::T_YEAR)) {
		return true;
	}
	return false;
}

/**
 * day
 * ([FWS] 1*2DIGIT FWS) / obs-day
 */
bool MailHeaderParser::lexIsDay()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-day */
	std::ios::pos_type posEndOBSDay = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_DAY)) {
		posEndOBSDay = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ([FWS] 1*2DIGIT FWS) */
	std::ios::pos_type posEnd2Digit = std::ios::off_type(-1);
	parseField(MailHeaderField::T_FWS);
	if (parseField(MailHeaderField::T_DIGIT)
	   && parseField(MailHeaderField::T_DIGIT)
	   && parseField(MailHeaderField::T_FWS)) {
		posEnd2Digit = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSDay != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd2Digit != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSDay, posEnd2Digit), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_FWS)) {
	//	if (parseField(MailHeaderField::T_DIGIT)) {
	//		parseField(MailHeaderField::T_DIGIT);
	//		if (parseField(MailHeaderField::T_FWS)) {
	//			return true;
	//		}
	//	}
	//} else if (parseField(MailHeaderField::T_OBS_DAY)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * month
 * "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", 
 * "Sep", "Oct", "Nov", "Dec"
 */
bool MailHeaderParser::lexIsMonth()
{
	const char* szMonthName[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	for (int i = 0; i < sizeof(szMonthName) / sizeof(szMonthName[0]); i++) {
		if (strncmp(szMonthName[i], buf, strlen(szMonthName[i])) == 0) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * year
 * (FWS 4*DIGIT FWS) / obs-year
 */
bool MailHeaderParser::lexIsYear()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-year */
	std::ios::pos_type posEndOBSYear = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_YEAR)) {
		posEndOBSYear = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* (FWS 4*DIGIT FWS) */
	std::ios::pos_type posEnd4Digit = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_FWS)) {
		char buf[5];
		memset(buf, 0, sizeof(buf));
		iss.read(buf, sizeof(buf) - 1);
		int year = 0;
		int nMatched = sscanf_s(buf, "%04d", &year);
		if (nMatched == 1) {
			if (parseField(MailHeaderField::T_FWS)) {
				posEnd4Digit = iss.tellg();
			}
		}
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSYear != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd4Digit != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSYear, posEnd4Digit), std::ios::beg);
		return true;
	}
	return false;
	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_FWS)) {
	//	char buf[5];
	//	memset(buf, 0, sizeof(buf));
	//	iss.read(buf, sizeof(buf) - 1);
	//	int year = 0;
	//	int nMatched = sscanf_s(buf, "%04d", &year);
	//	if (nMatched == 1) {
	//		if (parseField(MailHeaderField::T_FWS)) {
	//			return true;
	//		}
	//	}
	//	iss.seekg(pos, std::ios::beg);
	//}

	//if (parseField(MailHeaderField::T_OBS_YEAR)) {
	//	return true;
	//}
	//return false;
}

/**
 * time
 * time-of-day zone
 */
bool MailHeaderParser::lexIsTime()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_TIME_OF_DAY) && parseField(MailHeaderField::T_ZONE)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * time-of-day
 * hour ":" minute [ ":" second ]
 */
bool MailHeaderParser::lexIsTimeOfDay()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_HOUR) && iss.peek() == ':') {
		iss.seekg(1, std::ios::cur);
		if (parseField(MailHeaderField::T_MINUTE)) {
			if (iss.peek() == ':') {
				iss.seekg(1, std::ios::cur);
				if (parseField(MailHeaderField::T_SECOND)) {
					return true;
				}
			}
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * hour
 * 2DIGIT / obs-hour
 */
bool MailHeaderParser::lexIsHour()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-hour */
	std::ios::pos_type posEndOBSHour = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_HOUR)) {
		posEndOBSHour = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* 2DIGIT */
	std::ios::pos_type posEnd2DIGIT = std::ios::off_type(-1);
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int hour = 0;
	int matched = sscanf_s(buf, "%02d", &hour);
	if (matched == 1) {
		posEnd2DIGIT = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSHour != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd2DIGIT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSHour, posEnd2DIGIT), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//char buf[3];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, sizeof(buf) - 1);
	//int hour = 0;
	//int matched = sscanf_s(buf, "%02d", &hour);
	//if (matched == 1) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//
	//if (parseField(MailHeaderField::T_OBS_HOUR)) {
	//	return true;
	//}
	//return false;
}

/**
 * minute
 * 2DIGIT / obs-minute
 */
bool MailHeaderParser::lexIsMinute()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-minute */
	std::ios::pos_type posEndOBSMinute = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_MINUTE)) {
		posEndOBSMinute = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* 2DIGIT */
	std::ios::pos_type posEnd2DIGIT = std::ios::off_type(-1);
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int minute = 0;
	int matched = sscanf_s(buf, "%02d", &minute);
	if (matched == 1) {
		posEnd2DIGIT = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSMinute != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd2DIGIT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSMinute, posEnd2DIGIT), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//char buf[3];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, sizeof(buf) - 1);
	//int minute = 0;
	//int matched = sscanf_s(buf, "%02d", &minute);
	//if (matched == 1) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//
	//if (parseField(MailHeaderField::T_OBS_MINUTE)) {
	//	return true;
	//}
	//return false;
}

/**
 * second
 * 2DIGIT / obs-second
 */
bool MailHeaderParser::lexIsSecond()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-minute */
	std::ios::pos_type posEndOBSSecond = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_SECOND)) {
		posEndOBSSecond = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* 2DIGIT */
	std::ios::pos_type posEnd2DIGIT = std::ios::off_type(-1);
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int minute = 0;
	int matched = sscanf_s(buf, "%02d", &minute);
	if (matched == 1) {
		posEnd2DIGIT = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSSecond != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd2DIGIT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSSecond, posEnd2DIGIT), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//char buf[3];
	//memset(buf, 0, sizeof(buf));
	//iss.read(buf, sizeof(buf) - 1);
	//int second = 0;
	//int matched = sscanf_s(buf, "%02d", &second);
	//if (matched == 1) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//
	//if (parseField(MailHeaderField::T_OBS_SECOND)) {
	//	return true;
	//}
	//return false;
}

/**
 * zone
 * (FWS ( "+" / "-" ) 4DIGIT) / obs-zone
 */
bool MailHeaderParser::lexIsZone()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-zone */
	std::ios::pos_type posEndOBSZone = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_ZONE)) {
		posEndOBSZone = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* (FWS ( "+" / "-" ) 4DIGIT) */
	std::ios::pos_type posEnd4DIGIT = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_FWS)) {
		if (iss.peek() == '+' || iss.peek() == '-') {
			iss.seekg(1, std::ios::cur);
			char buf[5];
			memset(buf, 0, sizeof(buf));
			iss.read(buf, sizeof(buf) - 1);
			int zone = 0;
			int matched = sscanf_s(buf, "%04d", &zone);
			if (matched == 1) {
				posEnd4DIGIT = iss.tellg();
			}
		}
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSZone != std::ios::pos_type(std::ios::off_type(-1))
	   || posEnd4DIGIT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSZone, posEnd4DIGIT), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_FWS)) {
	//	if (iss.peek() == '+' || iss.peek() == '-') {
	//		iss.seekg(1, std::ios::cur);
	//		char buf[5];
	//		memset(buf, 0, sizeof(buf));
	//		iss.read(buf, sizeof(buf) - 1);
	//		int zone = 0;
	//		int matched = sscanf_s(buf, "%04d", &zone);
	//		if (matched == 1) {
	//			return true;
	//		}
	//	}
	//	iss.seekg(pos, std::ios::beg);
	//}

	//if (parseField(MailHeaderField::T_OBS_ZONE)) {
	//	return true;
	//}
	//return false;
}

/**
 * address
 * mailbox / group
 */
bool MailHeaderParser::lexIsAddress()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX, MailHeaderField::T_GROUP
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_MAILBOX) || parseField(MailHeaderField::T_GROUP)) {
	//	return true;
	//}
	//return false;
}

/**
 * mailbox
 * name-addr / addr-spec
 */
bool MailHeaderParser::lexIsMailbox()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_NAME_ADDR, MailHeaderField::T_ADDR_SPEC
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_NAME_ADDR) || parseField(MailHeaderField::T_ADDR_SPEC)) {
	//	return true;
	//}
	//return false;
}

/**
 * name-addr
 * [display-name] angle-addr
 */
bool MailHeaderParser::lexIsNameAddr()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_DISPLAY_NAME);
	if (! parseField(MailHeaderField::T_ANGLE_ADDR)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * angle-addr
 * [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
 */
bool MailHeaderParser::lexIsAngleAddr()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-angle-addr */
	std::ios::pos_type posEndOBSAngleAddr = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_OBS_ANGLE_ADDR)) {
		posEndOBSAngleAddr = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* [CFWS] "<" addr-spec ">" [CFWS] */
	std::ios::pos_type posEndAddrSpec = std::ios::off_type(-1);
	parseField(MailHeaderField::T_CFWS);
	if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
		if (parseField(MailHeaderField::T_ADDR_SPEC) && iss.peek() == '>'
		   && iss.seekg(1, std::ios::cur)) {
			parseField(MailHeaderField::T_CFWS);
			posEndAddrSpec = iss.tellg();
		}
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSAngleAddr != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndAddrSpec != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSAngleAddr, posEndAddrSpec), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//parseField(MailHeaderField::T_CFWS);
	//if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
	//	if (parseField(MailHeaderField::T_ADDR_SPEC) && iss.peek() == '>'
	//	   && iss.seekg(1, std::ios::cur)) {
	//		parseField(MailHeaderField::T_CFWS);
	//		return true;
	//	}
	//}
	//iss.seekg(pos, std::ios::beg);
	//if (parseField(MailHeaderField::T_OBS_ANGLE_ADDR)) {
	//	return true;
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * group
 * display-name ":" [group-list] ";" [CFWS]
 */
bool MailHeaderParser::lexIsGroup()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_DISPLAY_NAME)
	   && (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		parseField(MailHeaderField::T_GROUP_LIST);
		if (iss.peek() == ';' && iss.seekg(1, std::ios::cur)) {
			parseField(MailHeaderField::T_CFWS);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * display-name
 * phrase
 */
bool MailHeaderParser::lexIsDisplayName()
{
	return parseField(MailHeaderField::T_PHRASE);
}

/**
 * mailbox-list
 * (mailbox *("," mailbox)) / obs-mbox-list
 */
bool MailHeaderParser::lexIsMailboxList()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-mbox-list */
	std::ios::pos_type posEndOBSMboxList = std::ios::pos_type(std::ios::off_type(-1));
	if (parseField(MailHeaderField::T_OBS_MBOX_LIST)) {
		posEndOBSMboxList = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}
	
	/* (mailbox *("," mailbox)) */
	std::ios::pos_type posEndMailboxList = std::ios::pos_type(std::ios::off_type(-1));
	if (parseField(MailHeaderField::T_MAILBOX)) {
		while (! iss.eof()) {
			if (iss.peek() != ',') {
				break;
			}
			if (! parseField(MailHeaderField::T_MAILBOX)) {
				if (posEndOBSMboxList != std::ios::pos_type(std::ios::off_type(-1))) {
					iss.seekg(posEndOBSMboxList);
					return true;
				}
				iss.seekg(posBeg, std::ios::beg);
				return false;
			}
		}
		posEndMailboxList = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSMboxList != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndMailboxList != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSMboxList, posEndMailboxList), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_OBS_MBOX_LIST)) {
	//	return true;
	//} else {
	//	if (parseField(MailHeaderField::T_MAILBOX)) {
	//		while (! iss.eof()) {
	//			if (iss.peek() != ',') {
	//				break;
	//			}
	//			if (! parseField(MailHeaderField::T_MAILBOX)) {
	//				iss.seekg(pos, std::ios::beg);
	//				return false;
	//			}
	//		}
	//		return true;
	//	}
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * address-list
 * (address *("," address)) / obs-addr-list
 */
bool MailHeaderParser::lexIsAddressList()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-addr-list */
	std::ios::pos_type posEndOBSAddrList = std::ios::pos_type(std::ios::off_type(-1));
	if (parseField(MailHeaderField::T_OBS_ADDR_LIST)) {
		posEndOBSAddrList = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}
	
	/* (address *("," address)) */
	std::ios::pos_type posEndAddressList = std::ios::pos_type(std::ios::off_type(-1));
	if (parseField(MailHeaderField::T_ADDRESS)) {
		while (! iss.eof()) {
			if (iss.peek() != ',') {
				break;
			}
			if (! parseField(MailHeaderField::T_ADDRESS)) {
				if (posEndAddressList != std::ios::pos_type(std::ios::off_type(-1))) {
					iss.seekg(posEndAddressList, std::ios::beg);
					return true;
				}
				iss.seekg(posBeg, std::ios::beg);
				return false;
			}
		}
		posEndAddressList = iss.tellg();
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndOBSAddrList != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndAddressList != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSAddrList, posEndAddressList));
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_OBS_ADDR_LIST)) {
	//	return true;
	//} else {
	//	if (parseField(MailHeaderField::T_ADDRESS)) {
	//		while (! iss.eof()) {
	//			if (iss.peek() != ',') {
	//				break;
	//			}
	//			if (! parseField(MailHeaderField::T_ADDRESS)) {
	//				iss.seekg(pos, std::ios::beg);
	//				return false;
	//			}
	//		}
	//		return true;
	//	}
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * group-list
 * mailbox-list / CFWS / obs-group-list
 */
bool MailHeaderParser::lexIsGroupList()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_MAILBOX_LIST, MailHeaderField::T_CFWS,
		MailHeaderField::T_OBS_GROUP_LIST
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//return (parseField(MailHeaderField::T_MAILBOX_LIST)
	//       || parseField(MailHeaderField::T_CFWS)
	//	   || parseField(MailHeaderField::T_OBS_GROUP_LIST));
}

/**
 * addr-spec
 * local-part "@" domain
 */
bool MailHeaderParser::lexIsAddrSpec()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_LOCAL_PART) && (iss.peek() == '@' && iss.seekg(1, std::ios::cur))
	   && parseField(MailHeaderField::T_DOMAIN)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * local-part
 * dot-atom / quoted-string / obs-local-part
 */
bool MailHeaderParser::lexIsLocalPart()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DOT_ATOM, MailHeaderField::T_QUOTED_STRING,
		MailHeaderField::T_OBS_LOCAL_PART
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_DOT_ATOM) || parseField(MailHeaderField::T_QUOTED_STRING)
	//   || parseField(MailHeaderField::T_OBS_LOCAL_PART)) {
	//	return true;
	//}
	//return false;
}

/**
 * domain
 * dot-atom / domain-literal / obs-domain
 */
bool MailHeaderParser::lexIsDomain()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DOT_ATOM, MailHeaderField::T_DOMAIN_LITERAL,
		MailHeaderField::T_OBS_DOMAIN
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_DOT_ATOM) || parseField(MailHeaderField::T_DOMAIN_LITERAL)
	//   || parseField(MailHeaderField::T_OBS_DOMAIN)) {
	//	return true;
	//}
	//return false;
}

/**
 * domain-literal
 * [CFWS] "[" *([FWS] dtext) [FWS] "]" [CFWS]
 */
bool MailHeaderParser::lexIsDomainLiteral()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (iss.peek() == '[' && iss.seekg(1, std::ios::cur)) {
		while (! iss.eof()) {
			if (! parseField(MailHeaderField::T_FWS)) {
				break;
			}
			if (! parseField(MailHeaderField::T_DTEXT)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		parseField(MailHeaderField::T_FWS);
		if (iss.peek() == ']' && iss.seekg(1, std::ios::cur)) {
			parseField(MailHeaderField::T_CFWS);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * dtext
 * %d33-90 / %d94-126 / obs-dtext
 */
bool MailHeaderParser::lexIsDtext()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* obs-dtext */
	std::ios::pos_type posEndOBSDTEXT = 0;
	if (parseField(MailHeaderField::T_OBS_DTEXT)) {
		posEndOBSDTEXT = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* %d33-90 / %d94-126 */
	std::ios::pos_type posEndNotOBSDTEXT = 0;
	const char *szNonDtext = "[]\\";
	int n = iss.peek();
	if (33 <= n && n <= 126 && strchr(szNonDtext, n) == NULL) {
		iss.seekg(1, std::ios::cur);
		posEndNotOBSDTEXT = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ç≈í∑àÍív */
	if (posEndOBSDTEXT != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndNotOBSDTEXT != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndOBSDTEXT, posEndNotOBSDTEXT), std::ios::beg);
		return true;
	}
	return false;

	//const char *szNonDtext = "[]\\";
	//int n = iss.peek();
	//if (33 <= n && n <= 126 && strchr(szNonDtext, n) == NULL) {
	//	iss.seekg(1, std::ios::cur);
	//	return true;
	//} else if (parseField(MailHeaderField::T_OBS_DTEXT)) {
	//	return true;
	//}
	//return false;
}

/**
 * msg-id
 * [CFWS] "<" id-left "@" id-right ">" [CFWS]
 */
bool MailHeaderParser::lexIsMsgID()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
		if (parseField(MailHeaderField::T_ID_LEFT)
			&& iss.peek() == '@' && iss.seekg(1, std::ios::cur)) {
			if (parseField(MailHeaderField::T_ID_RIGHT)
				&& iss.peek() == '>' && iss.seekg(1, std::ios::cur)) {
				parseField(MailHeaderField::T_CFWS);
				return true;
			}
		}
		iss.seekg(pos, std::ios::beg);
	}
	return false;
}

/**
 * id-left
 * dot-atom-text / obs-id-left
 */
bool MailHeaderParser::lexIsIDLeft()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DOT_ATOM_TEXT, MailHeaderField::T_OBS_ID_LEFT
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_DOT_ATOM_TEXT) || parseField(MailHeaderField::T_OBS_ID_LEFT)) {
	//	return true;
	//}
	//return false;
}

/**
 * id-right
 * dot-atom-text / no-fold-literal / obs-id-right
 */
bool MailHeaderParser::lexIsIDRight()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DOT_ATOM_TEXT, MailHeaderField::T_NO_FOLD_LITERAL,
		MailHeaderField::T_OBS_ID_RIGHT
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//if (parseField(MailHeaderField::T_DOT_ATOM_TEXT) || parseField(MailHeaderField::T_NO_FOLD_LITERAL)
	//   || parseField(MailHeaderField::T_OBS_ID_RIGHT)) {
	//	return true;
	//}
	//return false;
}

/**
 * no-fold-literal
 * "[" *dtext "]"
 */
bool MailHeaderParser::lexIsNoFoldLiteral()
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '[' && iss.seekg(1, std::ios::cur)) {
		while (parseField(MailHeaderField::T_DTEXT));
		if (iss.peek() == ']' && iss.seekg(1, std::ios::cur)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * path
 * angle-addr / ([CFWS] "<" [CFWS] ">" [CFWS])
 */
bool MailHeaderParser::lexIsPath()
{
	std::ios::pos_type posBeg = iss.tellg();

	/* angle-addr */
	std::ios::pos_type posEndAngleAddr = std::ios::off_type(-1);
	if (parseField(MailHeaderField::T_ANGLE_ADDR)) {
		posEndAngleAddr = iss.tellg();
		iss.seekg(posBeg, std::ios::beg);
	}

	/* ([CFWS] "<" [CFWS] ">" [CFWS]) */
	std::ios::pos_type posEndCFWS = std::ios::off_type(-1);
	parseField(MailHeaderField::T_CFWS);
	if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
		parseField(MailHeaderField::T_CFWS);
		if (iss.peek() == '>' && iss.seekg(1, std::ios::cur)) {
			parseField(MailHeaderField::T_CFWS);
			posEndCFWS = iss.tellg();
		}
	}
	iss.seekg(posBeg, std::ios::beg);

	/* ç≈í∑àÍív */
	if (posEndAngleAddr != std::ios::pos_type(std::ios::off_type(-1))
	   || posEndCFWS != std::ios::pos_type(std::ios::off_type(-1))) {
		iss.seekg(std::max(posEndAngleAddr, posEndCFWS), std::ios::beg);
		return true;
	}
	return false;

	//std::ios::pos_type pos = iss.tellg();
	//if (parseField(MailHeaderField::T_ANGLE_ADDR)) {
	//	return true;
	//}
	//parseField(MailHeaderField::T_CFWS);
	//if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
	//	parseField(MailHeaderField::T_CFWS);
	//	if (iss.peek() == '>' && iss.seekg(1, std::ios::cur)) {
	//		parseField(MailHeaderField::T_CFWS);
	//		return true;
	//	}
	//}
	//iss.seekg(pos, std::ios::beg);
	//return false;
}

/**
 * received-token
 * word / angle-addr / addr-spec / domain
 */
bool MailHeaderParser::lexIsReceivedToken()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_DOMAIN, MailHeaderField::T_ANGLE_ADDR,
		MailHeaderField::T_ADDR_SPEC, MailHeaderField::T_WORD
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);

	//return (parseField(MailHeaderField::T_DOMAIN)
	//		|| parseField(MailHeaderField::T_ANGLE_ADDR)
	//		|| parseField(MailHeaderField::T_ADDR_SPEC)
	//		|| parseField(MailHeaderField::T_WORD));
}

/**
 * field-name
 * 1*ftext
 */
bool MailHeaderParser::lexIsFieldName()
{
	if (parseField(MailHeaderField::T_FTEXT)) {
		while (parseField(MailHeaderField::T_FTEXT));
		return true;
	}
	return false;
}

/**
 * ftext
 * %d33-57 / %d59-126
 */
bool MailHeaderParser::lexIsFtext()
{
	int n = iss.peek();
	if (33 <= n && n <= 126 && n != 58 && iss.seekg(1, std::ios::cur)) {
		return true;
	}
	return false;
}

/**
 * obs-NO-WS-CTL
 * %d1-8 / %d11 / %d12 / %d14-31 / %d127  ; ïúãAÅAâ¸çsÅAãÛîíÇèúÇ≠US-ASCII êßå‰ï∂éö
 */
bool MailHeaderParser::lexIsObsNOWSCTL()
{
	const char *szWS = "\t\n\r";
	int n = iss.peek();
	if ((1 <= n && n <= 31 && strchr(szWS, n) == NULL) || n == 127) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * obs-ctext
 * obs-NO-WS-CTL
 */
bool MailHeaderParser::lexIsObsCtext()
{
	return parseField(MailHeaderField::T_OBS_NO_WS_CTL);
}

/**
 * obs-qtext
 * obs-NO-WS-CTL
 */
bool MailHeaderParser::lexIsObsQtext()
{
	return parseField(MailHeaderField::T_OBS_NO_WS_CTL);
}

/**
 * obs-utext
 * %d0 / obs-NO-WS-CTL / VCHAR
 */
bool MailHeaderParser::lexIsObsUtext()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_OBS_NO_WS_CTL, MailHeaderField::T_VCHAR
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (iss.eof() || (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX));

	//return (iss.eof()
	//        || parseField(MailHeaderField::T_OBS_NO_WS_CTL)
	//		|| parseField(MailHeaderField::T_VCHAR));
}

/**
 * obs-qp
 * "\" (%d0 / obs-NO-WS-CTL / LF / CR)
 */
bool MailHeaderParser::lexIsObsQp()
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '\\' && iss.seekg(1, std::ios::cur)) {
		if (iss.eof()
		   || parseField(MailHeaderField::T_NO_WS_CTL)
		   || (iss.peek() == '\n' && iss.seekg(1, std::ios::cur))
		   || (iss.peek() == '\r' && iss.seekg(1, std::ios::cur))) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-body
 * *((*LF *CR *((%d0 / text) *LF *CR)) / CRLF)
 */
bool MailHeaderParser::lexIsObsBody()
{
	std::ios::pos_type pos = -1;
	while (pos != iss.tellg()) {
		if (parseField(MailHeaderField::T_CRLF)) {
			pos = iss.tellg();
			continue;
		}
		while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
		while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
		while (! iss.eof()) {
			if (! (iss.eof() || parseField(MailHeaderField::T_TEXT))) {
				break;
			}
			while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
			while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
		}
	}
	return true;
}

/**
 * obs-unstruct
 * *((*LF *CR *(obs-utext *LF *CR)) / FWS)
 */
bool MailHeaderParser::lexIsObsUnstruct()
{
	std::ios::pos_type pos = iss.tellg();
	do {
		if (parseField(MailHeaderField::T_FWS)) {
			continue;
		}
		while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
		while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
		while (! iss.eof()) {
			if (! parseField(MailHeaderField::T_OBS_UTEXT)) {
				break;
			}
			while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
			while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
		}
	} while (! iss.eof() && pos != iss.tellg());
	return true;
}

/**
 * obs-phrase
 * word *(word / "." / CFWS)
 */
bool MailHeaderParser::lexIsObsPhrase()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_WORD)) {
		while (! iss.eof()) {
			if (iss.peek() == '.') {
				iss.seekg(1, std::ios::cur);
				continue;
			} else if (! (parseField(MailHeaderField::T_WORD) || parseField(MailHeaderField::T_CFWS))) {
				break;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-phrase-list
 * [phrase / CFWS] *("," [phrase / CFWS])
 */
bool MailHeaderParser::lexIsObsPhraseList()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_PHRASE);
	parseField(MailHeaderField::T_CFWS);
	while (! iss.eof()) {
		if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
			break;
		}
		if (! (parseField(MailHeaderField::T_PHRASE) || parseField(MailHeaderField::T_CFWS))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
	}
	return true;
}

/**
 * obs-FWS
 * 1*WSP *(CRLF 1*WSP)
 */
bool MailHeaderParser::lexIsObsFWS()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_WSP)) {
		while (parseField(MailHeaderField::T_WSP));
		while (! iss.eof()) {
			if (! parseField(MailHeaderField::T_CRLF)) {
				break;
			}
			if (! parseField(MailHeaderField::T_WSP)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (parseField(MailHeaderField::T_WSP));
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-day-of-week
 * [CFWS] day-name [CFWS]
 */
bool MailHeaderParser::lexIsObsDayOfWeek()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! parseField(MailHeaderField::T_DAY_NAME)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-day
 * [CFWS] 1*2DIGIT [CFWS]
 */
bool MailHeaderParser::lexIsObsDay()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (parseField(MailHeaderField::T_DIGIT)) {
		parseField(MailHeaderField::T_DIGIT);
		parseField(MailHeaderField::T_CFWS);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-year
 * [CFWS] 2*DIGIT [CFWS]
 */
bool MailHeaderParser::lexIsObsYear()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! (parseField(MailHeaderField::T_DIGIT) && parseField(MailHeaderField::T_DIGIT))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (parseField(MailHeaderField::T_DIGIT));
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-hour
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeaderParser::lexIsObsHour()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! (parseField(MailHeaderField::T_DIGIT) && parseField(MailHeaderField::T_DIGIT))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-minute
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeaderParser::lexIsObsMinute()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! (parseField(MailHeaderField::T_DIGIT) && parseField(MailHeaderField::T_DIGIT))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-second
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeaderParser::lexIsObsSecond()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if (! (parseField(MailHeaderField::T_DIGIT) && parseField(MailHeaderField::T_DIGIT))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-zone
 * "UT" / "GMT" / "EST" / "EDT" / "CST" /
 * "CDT" / "MST" / "MDT" / "PST" / "PDT" /
 * %d65-73 / %d75-90 / %d97-105 / %d107-122
 */
bool MailHeaderParser::lexIsObsZone()
{
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, 2);
	if (strcmp(buf, "UT") == 0) {
		return true;
	}

	const char *arrZone[] = { "GMT", "EST", "EDT", "CST", 
	                          "CDT", "MST", "MDT", "PST", "PDT" };
	iss.read(&buf[2], 1);
	for (int i = 0; i < sizeof(arrZone) / sizeof(arrZone[0]); i++) {
		if (strcmp(buf, arrZone[i]) == 0) {
			return true;
		}
	}
	if (isalpha(buf[0]) && buf[0] != 'j' && buf[0] != 'J') {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-angle-addr
 * [CFWS] "<" obs-route addr-spec ">" [CFWS]
 */
bool MailHeaderParser::lexIsObsAngleAddr()
{
	std::ios::pos_type pos = iss.tellg();
	parseField(MailHeaderField::T_CFWS);
	if ((iss.peek() == '<' && iss.seekg(1, std::ios::cur))
		&& parseField(MailHeaderField::T_OBS_ROUTE) && parseField(MailHeaderField::T_ADDR_SPEC)
		&& (iss.peek() == '>' && iss.seekg(1, std::ios::cur))) {
		parseField(MailHeaderField::T_CFWS);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-route
 * obs-domain-list ":"
 */
bool MailHeaderParser::lexIsObsRoute()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_OBS_DOMAIN_LIST)
	   && (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-domain-list
 * *(CFWS / ",") "@" domain *("," [CFWS] ["@" domain])
 */
bool MailHeaderParser::lexIsObsDomainList()
{
	std::ios::pos_type pos = iss.tellg();
	while (parseField(MailHeaderField::T_CFWS) || (iss.peek() == ',' && iss.seekg(1, std::ios::cur)));
	if ((iss.peek() == '@' && iss.seekg(1, std::ios::cur))
	   && parseField(MailHeaderField::T_DOMAIN)) {
		while (! iss.eof()) {
			if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			parseField(MailHeaderField::T_CFWS);
			if (iss.peek() == '@' && iss.seekg(1, std::ios::cur)) {
				if (! parseField(MailHeaderField::T_DOMAIN)) {
					iss.seekg(pos, std::ios::beg);
					return false;
				}
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-mbox-list
 * *([CFWS] ",") mailbox *("," [mailbox / CFWS])
 */
bool MailHeaderParser::lexIsObsMboxList()
{
	std::ios::pos_type pos = iss.tellg();
	while (! iss.eof()) {
		parseField(MailHeaderField::T_CFWS);
		if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
			break;
		}
	}
	if (parseField(MailHeaderField::T_MAILBOX)) {
		while (! iss.eof()) {
			if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			if (! (parseField(MailHeaderField::T_MAILBOX) || parseField(MailHeaderField::T_CFWS))) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-addr-list
 * *([CFWS] ",") address *("," [address / CFWS])
 */
bool MailHeaderParser::lexIsObsAddrList()
{
	std::ios::pos_type pos = iss.tellg();
	while (! iss.eof()) {
		parseField(MailHeaderField::T_CFWS);
		if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
			break;
		}
	}
	if (parseField(MailHeaderField::T_ADDRESS)) {
		while (! iss.eof()) {
			if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			if (! (parseField(MailHeaderField::T_ADDRESS) || parseField(MailHeaderField::T_CFWS))) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-group-list
 * 1*([CFWS] ",") [CFWS]
 */
bool MailHeaderParser::lexIsObsGroupList()
{
	std::ios::pos_type pos = iss.tellg();
	int nGroup = 0;
	while (! iss.eof()) {
		parseField(MailHeaderField::T_CFWS);
		if (! (iss.peek() == ',' && iss.seekg(1, std::ios::cur))) {
			break;
		}
		nGroup++;
	}
	if (nGroup < 1) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseField(MailHeaderField::T_CFWS);
	return true;
}

/**
 * obs-local-part
 * word *("." word)
 */
bool MailHeaderParser::lexIsObsLocalPart()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_WORD)) {
		while (! iss.eof()) {
			if (! (iss.peek() != '.') && iss.seekg(1, std::ios::cur)) {
				break;
			}
			if (! parseField(MailHeaderField::T_WORD)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	return false;
}

/**
 * obs-domain
 * atom *("." atom)
 */
bool MailHeaderParser::lexIsObsDomain()
{
	std::ios::pos_type pos = iss.tellg();
	if (parseField(MailHeaderField::T_ATOM)) {
		while (! iss.eof()) {
			if (! (iss.peek() != '.' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			if (! parseField(MailHeaderField::T_DOMAIN)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-dtext
 * obs-NO-WS-CTL / quoted-pair
 */
bool MailHeaderParser::lexIsObsDtext()
{
	MailHeaderField::MHFTYPE arrMHFTYPE[] = {
		MailHeaderField::T_OBS_NO_WS_CTL, MailHeaderField::T_QUOTED_PAIR
	};
	std::vector<MailHeaderField::MHFTYPE> vecMHFTYPE(
		&arrMHFTYPE[0], &arrMHFTYPE[sizeof(arrMHFTYPE) / sizeof(arrMHFTYPE[0])]);
	return (lexIsLongestMatch(vecMHFTYPE) != MailHeaderField::T_NOUSE_MAX);
	//return (parseField(MailHeaderField::T_OBS_NO_WS_CTL) || parseField(MailHeaderField::T_QUOTED_PAIR));
}

/**
 * obs-id-left
 * local-part
 */
bool MailHeaderParser::lexIsObsIDLeft()
{
	return parseField(MailHeaderField::T_LOCAL_PART);
}

/**
 * obs-id-right
 * domain
 */
bool MailHeaderParser::lexIsObsIDRight()
{
	return parseField(MailHeaderField::T_DOMAIN);
}





