#pragma once
// Author Lukas Landgraf
#include <esp_http_server.h>
#include "lobby.h"


esp_err_t get_handler(httpd_req_t *req);

httpd_handle_t start_webserver(const char *, Lobby * lobby);



struct user_ctx{
    char * base_path;
};
