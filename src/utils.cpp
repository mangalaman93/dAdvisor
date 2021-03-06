#include "utils.h"

int Utils::systemCmd(const string& cmd) {
  FILE* pipe{ popen(cmd.c_str(), "r")};
  assert(pipe);

  // the exit status is in top 16 bits
  return (pclose(pipe)/256);
}

void Utils::systemCmd(const string& cmd, int ret) {
  int r = Utils::systemCmd(cmd);

  if(ret != r) {
    cout<<"unsuccessful to run command: \""<<cmd<<"\"!";
    LOG_POS();
    assert(0);
  }
}

int Utils::systemCmd(const string& cmd, string& out) {
  FILE* pipe{ popen(cmd.c_str(), "r")};
  assert(pipe);

  char buffer[256];
  while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
    string file = buffer;
    out += file.substr(0, file.size()-1);
  }

  return (pclose(pipe)/256);
}

void Utils::systemCmd(const string& cmd, string& out, int ret) {
  int r = Utils::systemCmd(cmd, out);

  if(ret != r) {
    cout<<"unsuccessful to run command: \""<<cmd<<"\"!";
    LOG_POS();
    assert(0);
  }
}

string Utils::getIPAddr(string interface) {
  struct ifaddrs *ifaddr, *ifa;
  char host[NI_MAXHOST];

  // getting IP addresses
  if(getifaddrs(&ifaddr) == -1) {
    LOG_POS();
    exit(EXIT_FAILURE);
  }

  for(ifa=ifaddr; ifa!=NULL; ifa=ifa->ifa_next) {
    if(ifa->ifa_addr == NULL) {
      continue;
    }

    int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
                        NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

    if((strcmp(ifa->ifa_name, interface.c_str()) == 0) &&
       (ifa->ifa_addr->sa_family == AF_INET)) {
      if(s != 0) {
        printf("getnameinfo() failed: %s!\n", gai_strerror(s));
        LOG_POS();
        exit(EXIT_FAILURE);
      }

      break;
    }
  }

  freeifaddrs(ifaddr);
  string ip(host);
  return ip;
}

void Utils::readFile(const string& path, string& content) {
  stringstream ss;
  ss<<"cat "<<path;
  systemCmd(ss.str(), content);
}

void Utils::writeFile(const string& path, const string& content) {
  fstream file(path, fstream::out | fstream::trunc);
  if(file.good()) {
    file.write(content.c_str(), content.length());
    file.write("\n", 1);
    file.close();
  } else {
    perror("unable to write to file!");
    LOG_POS();
    exit(EXIT_FAILURE);
  }
}

string Utils::getStringBW(float bw) {
  stringstream ss;

  if(bw < 1024) {
    ss<<bw<<"b/1s";
  } else if(bw < 1024*1024) {
    bw = bw/1024;
    ss<<bw<<"kbps";
  } else if(bw < 1024*1024*1024) {
    bw = bw/1024/1024;
    ss<<bw<<"mbps";
  } else {
    ss<<bw<<"b/1s";
  }

  return ss.str();
}
