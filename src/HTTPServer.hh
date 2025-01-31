#pragma once

#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>
#include <stdlib.h>

#include <memory>
#include <string>

#include "ProxyServer.hh"
#include "ServerState.hh"

class HTTPServer {
public:
  HTTPServer(std::shared_ptr<ServerState> state);
  HTTPServer(const HTTPServer&) = delete;
  HTTPServer(HTTPServer&&) = delete;
  HTTPServer& operator=(const HTTPServer&) = delete;
  HTTPServer& operator=(HTTPServer&&) = delete;
  virtual ~HTTPServer() = default;

  void listen(const std::string& socket_path);
  void listen(const std::string& addr, int port);
  void listen(int port);
  void add_socket(int fd);

  void schedule_stop();
  void wait_for_stop();

protected:
  class http_error : public std::runtime_error {
  public:
    http_error(int code, const std::string& what);
    int code;
  };

  std::shared_ptr<ServerState> state;
  std::shared_ptr<struct event_base> base;
  std::shared_ptr<struct evhttp> http;
  std::thread th;

  void thread_fn();

  static void dispatch_handle_request(struct evhttp_request* req, void* ctx);
  void handle_request(struct evhttp_request* req);

  static const std::unordered_map<int, const char*> explanation_for_response_code;
  static void send_response(struct evhttp_request* req, int code, const char* content_type, struct evbuffer* b);
  static void send_response(struct evhttp_request* req, int code, const char* content_type, const char* fmt, ...);

  static std::unordered_multimap<std::string, std::string> parse_url_params(const std::string& query);
  static std::unordered_map<std::string, std::string> parse_url_params_unique(const std::string& query);
  static const std::string& get_url_param(
      const std::unordered_multimap<std::string, std::string>& params,
      const std::string& key,
      const std::string* _default = nullptr);

  static JSON generate_quest_json_st(std::shared_ptr<const Quest> q);
  static JSON generate_client_config_json_st(const Client::Config& config);
  static JSON generate_license_json_st(std::shared_ptr<const License> l);
  static JSON generate_game_client_json_st(std::shared_ptr<const Client> c, std::shared_ptr<const ItemNameIndex> item_name_index);
  static JSON generate_proxy_client_json_st(std::shared_ptr<const ProxyServer::LinkedSession> ses);
  static JSON generate_lobby_json_st(std::shared_ptr<const Lobby> l, std::shared_ptr<const ItemNameIndex> item_name_index);
  JSON generate_game_server_clients_json() const;
  JSON generate_proxy_server_clients_json() const;
  JSON generate_server_info_json() const;
  JSON generate_lobbies_json() const;
  JSON generate_summary_json() const;
  JSON generate_all_json() const;

  JSON generate_ep3_cards_json(bool trial) const;
  JSON generate_common_tables_json() const;
  JSON generate_rare_tables_json() const;
  JSON generate_rare_table_json(const std::string& table_name) const;
};
