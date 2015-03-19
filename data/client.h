#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <iostream>
#include <utility>
#include <string>

class Client{
    friend std::istream& operator>>(std::istream&, Client&);
 public:
    Client() { }
    // st in minutes
    Client(std::string cid, std::string rid, int rt, int dt, int st):
        id(cid), id_region(rid), serv_time(st * 60), time_window(rt, dt) { }
    std::string get_id() const { return id; }
    std::string get_region() const { return id_region; }
    int get_ready_time() const { return time_window.first * 60; }
    int get_due_time() const { return time_window.second * 60; }
    int get_service_time() const { return serv_time * 60; }
 private:
    std::string id, id_region;
    int serv_time;
    std::pair<int, int> time_window;
};
#endif
