#ifndef HW_ZX_H
#define HW_ZX_H
#include "snmp_pp/snmp_pp.h"
#include "snmp_pp/collect.h"
#include "snmp_pp/notifyqueue.h"

#include <string>
bool  zxGetData(Snmp_pp::Pdu& pdu, int& son_sys, int& bus, std::string& jk, std::string& cw, int& code, std::string& atime);
bool  hwGetData(Snmp_pp::Pdu& pdu, int& son_sys, int& bus, std::string& jk, std::string& cw, int& code, std::string& atime);
#endif
