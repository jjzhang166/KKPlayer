
#include "Dir.hpp"
#include "uce-dirent.h"
#include <cstdio>
#include <queue>


namespace dir{


////////////////////////////////////////////////////////////
void listFiles(std::list<std::string>& list, const std::string& folder, const std::string& extension, bool recursive){
    DIR* dir;
    DIR* subDir;
    struct dirent *ent;
    // try to open top folder
    dir = opendir(folder.c_str());
    if (dir == NULL){
        // could not open directory
      fprintf(stderr, "Could not open \"%s\" directory.\n", folder.c_str());
      return;
    }else{
        // close, we'll process it next
        closedir(dir);
    }
    // enqueue top folder
    std::queue<std::string> folders;
    folders.push(folder);

    // run while has queued folders
    while (!folders.empty()){
        std::string currFolder = folders.front();
        folders.pop();
        dir = opendir(currFolder.c_str());
        if (dir == NULL) continue;
        // iterate through all the files and directories
        while ((ent = readdir (dir)) != NULL) {
            std::string name(ent->d_name);
            // ignore "." and ".." directories
            if ( name.compare(".") == 0 || name.compare("..") == 0) continue;
            // add path to the file name
            std::string path = currFolder;
            path.append("/");
            path.append(name);
            // check if it's a folder by trying to open it
            subDir = opendir(path.c_str());
            if (subDir != NULL){
                // it's a folder: close, we can process it later
                closedir(subDir);
                if (recursive) folders.push(path);
            }else{
                // it's a file
                if (extension.empty()){
                    list.push_back(path);
                }else{
                    // check file extension
                    size_t lastDot = name.find_last_of('.');
                    std::string ext = name.substr(lastDot+1);
                    if (ext.compare(extension) == 0){
                        // match
                        list.push_back(path);
                    }
                } // endif (extension test)
            } // endif (folder test)
        } // endwhile (nextFile)
        closedir(dir);
    } // endwhile (queued folders)

} // end listFiles


}   // namespace dir

