/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "FileHandler.h"

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <limits.h>
#include <boost/regex.hpp>

#include <QFileInfo>

using std::string;



/******************************************************************************/
/******************************************************************************/

CFileHandler::CFileHandler(const char* filename, const char* modes)
: ifs(NULL), filePos(0), fileSize(-1)
{
	Init(filename, modes);
}


CFileHandler::CFileHandler(const string& filename, const string& modes)
: ifs(NULL), filePos(0), fileSize(-1)
{
	Init(filename, modes);
}


CFileHandler::~CFileHandler(void)
{
	delete ifs;
}


/******************************************************************************/

bool CFileHandler::TryRawFS(const string& filename)
{
	const string rawpath = filename;//filesystem.LocateFile(filename);
	ifs = new std::ifstream(rawpath.c_str(), std::ios::in | std::ios::binary);
	if (ifs && !ifs->bad() && ifs->is_open()) {
		ifs->seekg(0, std::ios_base::end);
		fileSize = ifs->tellg();
		ifs->seekg(0, std::ios_base::beg);
		return true;
	}
	delete ifs;
	ifs = NULL;
	return false;
}


//bool CFileHandler::TryModFS(const string& filename)
//{
//	if (vfsHandler == NULL) {
//		return false;
//	}

//	const string file = StringToLower(filename);
//	if (vfsHandler->LoadFile(file, fileBuffer)) {
//		//! did we allocated more mem than needed (e.g. because of incorrect usage of std::vector)?
//		assert(fileBuffer.size() == fileBuffer.capacity());

//		fileSize = fileBuffer.size();
//		return true;
//	}
//	else
//		return false;
//}


//bool CFileHandler::TryMapFS(const string& filename)
//{
//	return TryModFS(filename); // FIXME
//}


//bool CFileHandler::TryBaseFS(const string& filename)
//{
//	return TryModFS(filename); // FIXME
//}


void CFileHandler::Init(const string& _filename, const string& modes)
{
	filename = _filename;
	if (TryRawFS(filename))  { return; }
//	const char* c = modes.c_str();
//	while (c[0] != 0) {
//		if (c[0] == SPRING_VFS_RAW[0])  { if (TryRawFS(filename))  { return; } }
//		if (c[0] == SPRING_VFS_MOD[0])  { if (TryModFS(filename))  { return; } }
//		if (c[0] == SPRING_VFS_MAP[0])  { if (TryMapFS(filename))  { return; } }
//		if (c[0] == SPRING_VFS_BASE[0]) { if (TryBaseFS(filename)) { return; } }
//		c++;
//	}
}


/******************************************************************************/

bool CFileHandler::FileExists() const
{
	return (fileSize >= 0);
}


int CFileHandler::Read(void* buf,int length)
{
	if (ifs) {
		ifs->read((char*)buf, length);
		return ifs->gcount ();
	}
	else if (!fileBuffer.empty()) {
		if ((length + filePos) > fileSize) {
			length = fileSize - filePos;
		}
		if (length > 0) {
			assert(fileBuffer.size() >= filePos+length);
			memcpy(buf, &fileBuffer[filePos], length);
			filePos += length;
		}
		return length;
	}

	return 0;
}


void CFileHandler::Seek(int length, std::ios_base::seekdir where)
{
	if (ifs)
	{
		// on some machines, EOF bit does not get reset when seeking to another pos
		ifs->clear();
		ifs->seekg(length, where);
	}
	else if (!fileBuffer.empty())
	{
		if (where == std::ios_base::beg)
		{
			filePos = length;
		}
		else if (where == std::ios_base::cur)
		{
			filePos += length;
		}
		else if (where == std::ios_base::end)
		{
			filePos = fileSize + length;
		}
	}
}


int CFileHandler::Peek() const
{
	if (ifs) {
		return ifs->peek();
	}
	else if (!fileBuffer.empty()){
		if (filePos < fileSize) {
			return fileBuffer.at(filePos);
		} else {
			return EOF;
		}
	}
	return EOF;
}


bool CFileHandler::Eof() const
{
	if (ifs) {
		return ifs->eof();
	}
	if (!fileBuffer.empty()) {
		return (filePos >= fileSize);
	}
	return true;
}


int CFileHandler::FileSize() const
{
   return fileSize;
}


int CFileHandler::GetPos() const
{
	if (ifs) {
		return ifs->tellg();
	} else {
		return filePos;
	}
}


bool CFileHandler::LoadStringData(string& data)
{
	if (!FileExists()) {
		return false;
	}
	char* buf = new char[fileSize];
	Read(buf, fileSize);
	data.append(buf, fileSize);
	delete[] buf;
	return true;
}

std::string CFileHandler::GetFileExt() const
{
	return QFileInfo( QString(filename.c_str()) ).suffix().toStdString();
}

/******************************************************************************/

//std::vector<string> CFileHandler::FindFiles(const string& path,
//                                       const string& pattern)
//{
//	GML_RECMUTEX_LOCK(file); // FindFiles

//	std::vector<string> found = filesystem.FindFiles(path, pattern);
//	boost::regex regexpattern(filesystem.glob_to_regex(pattern),
//	                          boost::regex::icase);
//	std::vector<string> f;

//	if (vfsHandler) {
//		f = vfsHandler->GetFilesInDir(path);
//	}

//	for (std::vector<string>::iterator fi = f.begin(); fi != f.end(); ++fi) {
//		if (boost::regex_match(*fi, regexpattern)) {
//			found.push_back(path + *fi);
//		}
//	}
//	return found;
//}


/******************************************************************************/

//std::vector<string> CFileHandler::DirList(const string& path,
//                                     const string& pattern,
//                                     const string& modes)
//{
//	GML_RECMUTEX_LOCK(file); // DirList

//	const string pat = pattern.empty() ? "*" : pattern;

//	std::set<string> fileSet;
//	const char* c = modes.c_str();
//	while (c[0] != 0) {
//		if (c[0] == SPRING_VFS_RAW[0])  { InsertRawFiles(fileSet, path, pat);  }
//		if (c[0] == SPRING_VFS_MOD[0])  { InsertModFiles(fileSet, path, pat);  }
//		if (c[0] == SPRING_VFS_MAP[0])  { InsertMapFiles(fileSet, path, pat);  }
//		if (c[0] == SPRING_VFS_BASE[0]) { InsertBaseFiles(fileSet, path, pat); }
//		c++;
//	}
//	std::vector<string> fileVec;
//	std::set<string>::const_iterator it;
//	for (it = fileSet.begin(); it != fileSet.end(); ++it) {
//		fileVec.push_back(*it);
//	}
//	return fileVec;
//}


//bool CFileHandler::InsertRawFiles(std::set<string>& fileSet,
//                                  const string& path,
//                                  const string& pattern)
//{
//	boost::regex regexpattern(filesystem.glob_to_regex(pattern),
//	                          boost::regex::icase);

//	const std::vector<string> &found = filesystem.FindFiles(path, pattern);
//	std::vector<string>::const_iterator fi;
//	for (fi = found.begin(); fi != found.end(); ++fi) {
//		if (boost::regex_match(*fi, regexpattern)) {
//			fileSet.insert(fi->c_str());
//		}
//	}

//	return true;
//}


//bool CFileHandler::InsertModFiles(std::set<string>& fileSet,
//                                  const string& path,
//                                  const string& pattern)
//{
//	if (!vfsHandler) {
//		return false;
//	}

//	string prefix = path;
//	if (path.find_last_of("\\/") != (path.size() - 1)) {
//		prefix += '/';
//	}

//	boost::regex regexpattern(filesystem.glob_to_regex(pattern), boost::regex::icase);

//	const std::vector<string> &found = vfsHandler->GetFilesInDir(path);
//	std::vector<string>::const_iterator fi;
//	for (fi = found.begin(); fi != found.end(); ++fi) {
//		if (boost::regex_match(*fi, regexpattern)) {
//			fileSet.insert(prefix + *fi);
//		}
//	}

//	return true;
//}


//bool CFileHandler::InsertMapFiles(std::set<string>& fileSet,
//                                  const string& path,
//                                  const string& pattern)
//{
//	return InsertModFiles(fileSet, path, pattern); // FIXME
//}


//bool CFileHandler::InsertBaseFiles(std::set<string>& fileSet,
//                                   const string& path,
//                                   const string& pattern)
//{
//	return InsertModFiles(fileSet, path, pattern); // FIXME
//}


/******************************************************************************/

//std::vector<string> CFileHandler::SubDirs(const string& path,
//                                     const string& pattern,
//                                     const string& modes)
//{
//	GML_RECMUTEX_LOCK(file); // SubDirs

//	const string pat = pattern.empty() ? "*" : pattern;

//	std::set<string> dirSet;
//	const char* c = modes.c_str();
//	while (c[0] != 0) {
//		if (c[0] == SPRING_VFS_RAW[0])  { InsertRawDirs(dirSet, path, pat);  }
//		if (c[0] == SPRING_VFS_MOD[0])  { InsertModDirs(dirSet, path, pat);  }
//		if (c[0] == SPRING_VFS_MAP[0])  { InsertMapDirs(dirSet, path, pat);  }
//		if (c[0] == SPRING_VFS_BASE[0]) { InsertBaseDirs(dirSet, path, pat); }
//		c++;
//	}
//	std::vector<string> dirVec;
//	std::set<string>::const_iterator it;
//	for (it = dirSet.begin(); it != dirSet.end(); ++it) {
//		dirVec.push_back(*it);
//	}
//	return dirVec;
//}


//bool CFileHandler::InsertRawDirs(std::set<string>& dirSet,
//                                 const string& path,
//                                 const string& pattern)
//{
//	boost::regex regexpattern(filesystem.glob_to_regex(pattern),
//	                          boost::regex::icase);

//	const std::vector<string> &found = filesystem.FindFiles(path, pattern,
//	                                            FileSystem::ONLY_DIRS);
//	std::vector<string>::const_iterator fi;
//	for (fi = found.begin(); fi != found.end(); ++fi) {
//		const string& dir = *fi;
//		if (boost::regex_match(dir, regexpattern)) {
//			dirSet.insert(dir);
//		}
//	}

//	return true;
//}


//bool CFileHandler::InsertModDirs(std::set<string>& dirSet,
//                                 const string& path,
//                                 const string& pattern)
//{
//	if (!vfsHandler) {
//		return false;
//	}

//	string prefix = path;
//	if (path.find_last_of("\\/") != (path.size() - 1)) {
//		prefix += '/';
//	}

//	boost::regex regexpattern(filesystem.glob_to_regex(pattern), boost::regex::icase);

//	const std::vector<string> &found = vfsHandler->GetDirsInDir(path);
//	std::vector<string>::const_iterator fi;
//	for (fi = found.begin(); fi != found.end(); ++fi) {
//		if (boost::regex_match(*fi, regexpattern)) {
//			dirSet.insert(prefix + *fi);
//		}
//	}

//	return true;
//}


//bool CFileHandler::InsertMapDirs(std::set<string>& dirSet,
//                                  const string& path,
//                                  const string& pattern)
//{
//	return InsertModDirs(dirSet, path, pattern); // FIXME
//}


//bool CFileHandler::InsertBaseDirs(std::set<string>& dirSet,
//                                   const string& path,
//                                   const string& pattern)
//{
//	return InsertModDirs(dirSet, path, pattern); // FIXME
//}


/******************************************************************************/

//string CFileHandler::AllowModes(const string& modes, const string& allowed)
//{
//	string newModes;
//	for (unsigned i = 0; i < modes.size(); i++) {
//		if (allowed.find(modes[i]) != string::npos) {
//			newModes += modes[i];
//		}
//	}
//	return newModes;
//}


//string CFileHandler::ForbidModes(const string& modes, const string& forbidden)
//{
//	string newModes;
//	for (unsigned i = 0; i < modes.size(); i++) {
//		if (forbidden.find(modes[i]) == string::npos) {
//			newModes += modes[i];
//		}
//	}
//	return newModes;
//}


/******************************************************************************/
/******************************************************************************/