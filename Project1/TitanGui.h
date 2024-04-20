#pragma once
#include <iostream>
#include "Filter.h"

class TableRuleData {
public:

	TableRuleData() : m_type(FLT_PATH_TYPE_EQUAL), m_isDenyAccess(false), m_isHideFile(false), m_isChecked(false) {
		
	}

	std::string getProcessName() {
		return m_processName;
	}

	void setProcessName(const std::string& ProcessName) {
		m_processName = ProcessName;
	}
	
	std::string getFilePath() {
		return m_filePath;
	}

	void setFilePath(const std::string& FilePath) {
		m_filePath = FilePath;
	}

	FLT_PATH_TYPE getPathType() {
		return m_type;
	}

	void setPathType(FLT_PATH_TYPE PathType) {
		m_type = PathType;
	}

	bool isDenyAccess() {
		return m_isDenyAccess;
	}

	void setDenyAccess(bool DenyAccess) {
		m_isDenyAccess = DenyAccess;
	}

	bool isHideFile() {
		return m_isHideFile;
	}

	void setHideFile(bool HideFile) {
		m_isHideFile = HideFile;
	}

	bool isChecked() {
		return m_isChecked;
	}

	bool* getCheckedPtr() {
		return &m_isChecked;
	}

	bool getChecked() {
		return m_isChecked;
	}

private:
	std::string m_processName;
	std::string m_filePath;
	FLT_PATH_TYPE m_type;
	bool m_isDenyAccess;
	bool m_isHideFile;
	bool m_isChecked;

};

void DrawCallBack();