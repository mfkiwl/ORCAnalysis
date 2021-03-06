#include "ORCA/FileSystem/Directory.hh"
#include "ORCA/FileSystem/File.hh"

//! Default directory constructor
ORCA::FileSystem::Directory::Directory(){
	fDirName = cwd();
}

//! Construct directory with name
ORCA::FileSystem::Directory::Directory(const std::string& dirName ){
	fDirName = dirName;
}


ORCA::FileSystem::Directory::~Directory(){}

std::string ORCA::FileSystem::Directory::GetName() const{
	return fDirName;
}

//! Determines validity
bool ORCA::FileSystem::Directory::IsValid(){
	DIR* dir=NULL;
	dir = opendir(fDirName.c_str() );
	bool ret = (dir);				
	closedir(dir);
	return ret;
}

bool ORCA::FileSystem::Directory::Read(){
		DIR* dir= opendir(fDirName.c_str() );
		if(!dir || fDirEntries.size() )
			return false;
	struct dirent *directoryEntry;
	while(directoryEntry = readdir(dir) ){
		std::string item = directoryEntry->d_name;
		//see if this is a directory
		Directory temp( GetAbsolutePath()+"/"+ item);
		if(temp.IsValid() ){
			if(item!="." && item!=".." ){
				fDirEntries.push_back(temp);
			}
		}
		else
			fFileEntries.push_back( File( GetAbsolutePath()+"/"+ item) ); 
	}
	closedir(dir);
	return true;
}

unsigned ORCA::FileSystem::Directory::GetNDirs(){return fDirEntries.size(); }
unsigned ORCA::FileSystem::Directory::GetNFiles(){return fFileEntries.size(); }

ORCA::FileSystem::File ORCA::FileSystem::Directory::FileAt(unsigned i){return fFileEntries[i];}
ORCA::FileSystem::Directory ORCA::FileSystem::Directory::DirAt(unsigned i){return fDirEntries[i]; }

//! Get back the absolute path
std::string ORCA::FileSystem::Directory::GetAbsolutePath(){

	if(this->fDirName.at(0) == '/' ){
		return fDirName;
	}
	fDirName = cwd() +"/"+ fDirName;
	return simplifyPath();
}

std::string ORCA::FileSystem::Directory::simplifyPath(){
	std::vector<std::string> path;
	std::stringstream input(fDirName);
	std::string segment;
	while(std::getline(input, segment, '/')){
		if(segment.size() || segment != std::string(" ") )
				path.push_back(segment);
	}

	typedef std::vector<std::string>::iterator tokit;
	std::string here = ".";
	std::string before = "..";

	for(tokit it = path.begin(); it!=path.end(); ++it){
		if( (*it) ==  here){
			path.erase(it);
			it = path.begin();
			}
		else if( (*it) == before ){
			tokit retstatement = it;
			tokit tobeerased = it;
			path.erase(--tobeerased);
			path.erase(retstatement);
			it = path.begin();
		}
	}
	fDirName="";
	std::stringstream outbuffer;
	for(tokit it = path.begin(); it!=path.end(); ++it)
		if( (*it).size()>0 )
			outbuffer<<"/"<< std::string(*it);
	outbuffer>>fDirName;
	return fDirName;
}
