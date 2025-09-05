#include <bits/stdc++.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

int whichLink(std::vector<std::string> *vec, std::string format);
std::string extractURL(std::string line);
bool contains(std::string line, std::string search);
std::string getName(std::string line);
std::string getDownloadLink(std::string line);

int main(int argc, const char *argv[]) {
  std::string form = "flac"; // Format
  std::string baseAddress;
  std::string outputFolder;
  if (argc < 2) {
    std::cout << "invalid number of arguments\n";
    return -1;
  }
  for (int i = 1; i < argc; i++) {
    const char *str = argv[i];
    switch (str[0]) {
    case '-':
      // Runtime arguments go here
      switch (str[1]) {
      case 'h':
        std::cout
            << "This is a program to download music from khinsider though new websites may be added at some point.\n\
            The input arguments are as follows: \n \tbaseUrl\t\tThis must be at argument 1, and must be the base url for the khinsider album you want to download. \
            \n\tdestinationFolder\tThis must be a folder with permission to create files within.\
            \n\nExample usage:\n\t $PROGRAMNAME baseUrl destinationFolder --OPTIONS\n\n\
            Optional Parameters\n\t-h\t\t\tprint this menu\n\
            --format\t\tPrefered format type, either flac or mp3\n";
        break;
      case '-': {
        // std::cout << "debug\n";
        const char *formatWord = "format";
        if (!strcmp(argv[i], formatWord)) {
          form = argv[i + 1];
          std::cout << "Preferring " << form << "format\n";
        }

        break;
      }
      default:
        std::cout << "How did you get here?\n";
        break;
      }
      break;
    case 'h':
      if (i == 1) { // Assumes that this is an address
        baseAddress = argv[1];
      }
      break;
    default:
      if (i == 2) { // assumes that this is a folder to ouput to
        outputFolder = argv[2];
      }
      std::cout << "No Input Given\n";
      break;
    }
  }
  std::string firstCommand =
      "curl " + baseAddress + " --output processingFile.html";
  system(firstCommand.c_str());
  std::fstream fs;
  std::string suburl;

  fs.open("processingFile.html");
  if (!fs.is_open()) {
    std::cout << "FAILED TO OPEN PROCESSING FILE!!\n";
    return -2;
  }
  std::string line;
  uint i = 0;
  uint stage = 0;
  while (getline(fs, line)) {
    switch (stage) {
    case 0:
      if (contains(line, "table id=\"songlist\"")) {
        std::cout << "passed stage 0 at line " << i << std::endl;
        stage++;
      }
      break;
    case 1:
      if (contains(line, "<td class=\"clickable-row\"><a href=")) {
        std::cout << "passed stage 1 at line " << i << std::endl;
        suburl = extractURL(line);
        std::cout << "going to: " << suburl << std::endl;
        stage++;
      } // else {
        // stage = 0;
      //}
      break;
    case 2:
      std::string secondCommand =
          "curl " + suburl + " --output subProcessingFile.html";
      system(secondCommand.c_str());
      std::fstream sfs;

      sfs.open("subProcessingFile.html");
      if (!sfs.is_open()) {
        std::cout << "FAILED TO OPEN SUBPROCESSING FILE!!\n";
        return -2;
      }

      uint substage = 0;
      std::string subLine;
      std::string name;
      std::vector<std::string> downloadLinks;
      std::string downloadLink;
      int linkNum = 0;
      uint lineNum = 1;
      std::string thirdCommand;
      while (getline(sfs, subLine)) {
        // std::cout << "Start of while loop\n";
        // std::cout << "Substage = " << substage << std::endl;
        switch (substage) {

        case 0:
          if (contains(subLine, "Song name: ")) {
            name = getName(subLine);
            substage++;
            std::cout << "Passed substage 0 at line " << lineNum
                      << " song name: " << name << std::endl;
          }
          break;
        case 1:
          if (contains(subLine, "<p><a href=")) {
            std::cout << "Passed substage 1 at line " << lineNum << std::endl;
            downloadLink = getDownloadLink(subLine);
            downloadLinks.push_back(downloadLink);
            // std::cout << "Download Link: " << downloadLink << std::endl;
          }
          break;
        default:
          substage = 0;
          break;
        }

        lineNum++;
      }
      stage = 1;
      // std::cout << "finding which link\n";
      linkNum = whichLink(&downloadLinks, form);
      // std::cout << "linkNum = " << linkNum << std::endl;
      if (linkNum >= 0) {
        thirdCommand = "curl " + downloadLinks[linkNum] + " --output " +
                       outputFolder + "'" + name + "." + form + "'";
      } else {
        thirdCommand = "curl " + downloadLinks[0] + " --output " +
                       outputFolder + "'" + name + "." + "mp3" + "'";
      }

      // std::cout << thirdCommand.c_str() << std::endl;
      system(thirdCommand.c_str());
      break;
    }
    i++;
  }
  std::cout << "Cleaning Up!\n";
  // Close the file stream
  fs.close();
  // Delete the first HTML file so it won't download again on failure.
  std::ofstream ofs("processingFile.html", std::ios::trunc);
  if (ofs.is_open()) {
    std::cout << "Processing file culled\n";
  } else {
    std::cout << "ERROR: Processing file unculled!\n";
  }
  std::cout << "Program Terminating\n";
  return 0;
}

// Negative one = not in prefered format
int whichLink(std::vector<std::string> *vec, std::string format) {
  int formatNumber = -1;
  int formIndex = 0;
  // std::cout << "vec size: " << vec->size() << std::endl;
  if (vec->size() > 1) {
    for (uint i = 0; i < vec->size(); i++) {
      std::string buf = vec->at(i);
      for (uint index = 0; index < buf.length(); index++) {
        if (buf[index] == format[formIndex]) {
          formIndex++;
        } else {
          formIndex = 0;
        }
        if (formIndex == format.length()) {
          return i;
        }
      }
    }
  }
  return formatNumber;
}

bool contains(std::string line, std::string search) {
  int searchIndex = 0;
  for (uint i = 0; i < line.length(); i++) {
    if (line[i] == search[searchIndex])
      if (searchIndex == search.length() - 1) {
        return true;
      } else {
        searchIndex++;
      }
    else {
      searchIndex = 0;
    }
  }
  return false;
}

std::string extractURL(std::string line) {
  std::string candidate;
  bool oldCapture = false;
  bool capture = false;
  // int candidateIndex = 0;
  for (int i = 0; i < line.length(); i++) {

    if (!capture && oldCapture) {
      // candidateIndex = 0;
      if (contains(candidate, "/game-soundtracks")) {
        std::string returnString = "https://downloads.khinsider.com";
        returnString.append(candidate);
        return returnString;
      }
    }
    if (capture && line[i] == '"') {
      capture = false;
      continue;
    }
    oldCapture = capture;
    if (capture) {
      candidate += line[i];
    }
    if (!capture && line[i] == '"') {
      capture = true;
      candidate = "";
    }
  }
  return "";
}

std::string getName(std::string line) {
  bool toggleCapture = false;
  std::string searchTag = "<b>";
  std::string endSearchTag = "</b>";
  std::string name;
  std::string buf;
  int index = 0;
  for (int i = 0; i < line.length(); i++) {
    if (!toggleCapture) {

      if (searchTag[index] == line[i]) {
        index++;
      } else {
        index = 0;
      }
      if (index == searchTag.length()) {
        toggleCapture = true;
        index = 0;
        continue;
      }
    }
    if (toggleCapture) {
      if (endSearchTag[index] == line[i]) {
        index++;
      } else {
        index = 0;
        buf = line[i];
        name.append(buf);
      }
      if (index == endSearchTag.length()) {
        toggleCapture = false;
        index = 0;
        return name;
      }
    }
  }
}

std::string getDownloadLink(std::string line) {
  bool toggleCapture = false;
  std::string downloadLink;
  std::string buf;
  // std::cout << "line.length() = " << line.length() << std::endl;
  // std::cout << "line = " << line << std::endl;
  for (int i = 0; i < line.length(); i++) {
    if (line[i] == '"' && !toggleCapture) {
      toggleCapture = true;
      continue;
    }
    if (line[i] == '"' && toggleCapture) {
      return downloadLink;
    }
    if (toggleCapture) {
      buf = line[i];
      downloadLink.append(buf);
    }
  }
  return "n";
}
