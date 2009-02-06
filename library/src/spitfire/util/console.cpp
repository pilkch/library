#include <spitfire/util/console.h>

namespace spitfire
{
  // Do not get a choice of colours, only green text, black background, preferably with a courier/terminal style font

  void cConsole::ProcessCommand_ls(const std::list<string_t>& arguments)
  {
    std::list<string_t> listOfFiles;

    if (!arguments.empty()) return;

    const std::vector<string_t>& paths = filesystem.GetSearchPaths();
    if (paths.empty()) return;

    for (size_t i = 0; i < n; i++) {
      filesystem::iterator iter(paths[i]);
      filesystem::iterator iterEnd;
      while (iter != iterEnd) {
        listOfFiles.push_back(iter.GetFile());

        iter++;
      }
    }

    if (listOfFiles.empty()) return;

    // Sort alphabetically
    listOfFiles.sort();

    // Now print out each file in the list
    string_t line;
    std::list<string_t>::iterator iter(listOfFiles.begin());
    const std::list<string_t>::iterator iterEnd(listOfFiles.end());
    while (iter != iterEnd) {
      line += *iter + TEXT(" ");

      iter++;
    }

    AddLine(spitfire::string::Trim(line));
  }

  void cConsole::ProcessCommand_cd(const std::list<string_t>& arguments)
  {
    if (arguments.empty()) return;

    if (arguments.size() != 1) return;

    const string_t destination(arguments.front());
    if (destination.empty()) return;

    // Can only cd to files within filesystem::SearchPaths and subfolders
    const std::vector<string_t>& paths = filesystem.GetSearchPaths();
    if (paths.empty()) return;

    string_t found;
    if is in subdirectory of paths then
    currentDirectory = found;
  }

  void cConsole::ProcessCommand(const string_t& command)
  {
    if (command.empty()) return;

    std::list<string_t> elements;
    spitfire::string::Split(command);
    if (elements.empty()) return;

    const string_t first_word = elements.front();
    if (command == TEXT("ls")) {
      // Remove the executable name and send to the function
      elements.pop_front();
      ProcessCommand_ls(elements);
      return;
    } else if (command == TEXT("cd")) {
      // Remove the executable name and send to the function
      elements.pop_front();
      ProcessCommand_cd(elements);
      return;
    }
  }
}
