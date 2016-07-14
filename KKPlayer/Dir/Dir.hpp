

#ifndef DIR_HPP
#define DIR_HPP

#include <string>
#include <list>

namespace dir{

////////////////////////////////////////////////////////////
/// \brief List files of a directory.
///
/// \param list The output files list.
/// \param folder The folder where to search in
/// \param extension A file extension filter,
/// empty extension will match all files.
/// \param recursive If true it will list files in
/// sub directories as well.
///
////////////////////////////////////////////////////////////
void listFiles(
    std::list<std::string>& list,
    const std::string& folder,
    const std::string& extension = "",
    bool recursive = false
);


}   // namespace dir

#endif 
