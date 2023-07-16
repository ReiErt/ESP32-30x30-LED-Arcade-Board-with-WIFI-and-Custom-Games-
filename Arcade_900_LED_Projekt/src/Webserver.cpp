// Author Lukas Landgraf
#include "Webserver.h"
#include <algorithm> 

#define Filepath_MAX 32
#define Buffersize 1000
#define MAX_WS_PAYLOAD 128

// holt Dateipfad aus der URL
esp_err_t get_path(char * path, const char * base_path, const char *uri){
    const char * end = strchr(uri, 0);
    if(end == NULL){
        printf("missing ?\n");
        return ESP_FAIL;
    }
    uint8_t urilen = (end - uri);
    uint8_t pathlen = urilen + strlen(base_path);
    if(pathlen + 1< Filepath_MAX){
        strcpy(path, base_path);
        strncpy(path + strlen(base_path), uri, urilen);
        path[pathlen] = '\0';
    }
    printf(path);
    return ESP_OK;
}

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

void set_content_type_from_file(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".css")) {
        httpd_resp_set_type(req, "text/css");
    } else if (IS_FILE_EXT(filename, ".html")) {
        httpd_resp_set_type(req, "text/html");
    } else if(IS_FILE_EXT(filename, ".js")) {
        httpd_resp_set_type(req, "text/javascript");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        httpd_resp_set_type(req, "image/x-icon");
    } else{
        httpd_resp_set_type(req, "text/plain");
    }
}
// HTTP Get Handler für Website
esp_err_t get_handler(httpd_req_t *req){
    char Path[Filepath_MAX];
    FILE * file = NULL;
    if(get_path(Path, ((user_ctx *)req->user_ctx)->base_path, req->uri) == ESP_FAIL){
        return ESP_FAIL;
    }

    file = fopen(Path, "r");
    if (!file) {
        printf("Couldn't open file: %s\n", Path);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Couldn't open file\n");
        return ESP_FAIL;
    }
    printf("Sending File\n");

    set_content_type_from_file(req, Path);

    char Buffer[Buffersize];
    int send = 0;
    do{
    send = fread(Buffer, 1, Buffersize, file);
        if(send > 0){
            if(httpd_resp_send_chunk(req, Buffer, send) != ESP_OK){
                printf("Error during sending of File");
                fclose(file);
                httpd_resp_sendstr_chunk(req, NULL);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error during sending of File");
                return ESP_FAIL;
            }
        }
    }while(send > 0);
    fclose(file);
    printf("File sending complete\n");
    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_FAIL;
}

// Websocket Handler
esp_err_t Websocket(httpd_req_t * req){
    if (req->method == HTTP_GET) {
        // Websocket Handshake
        return ESP_OK;
    }

    uint8_t buf[MAX_WS_PAYLOAD];
    httpd_ws_frame_t ws_pkt;
    ws_pkt.payload = buf;
    ws_pkt.len = 0;
    httpd_ws_recv_frame(req, &ws_pkt, MAX_WS_PAYLOAD);
    Lobby * lobby = (Lobby *)req->user_ctx; 

    //Knopf Befehl
    if(ws_pkt.payload[0] == 0 && req->sess_ctx != nullptr){
        Player * player = (Player *)((sess_cxt*)(req->sess_ctx))->player;
        if(ws_pkt.payload[2] == 1){
            player->Buttons[ws_pkt.payload[1]].setButton(true);
        }
        else if(ws_pkt.payload[2] == 0 && lobby->getButtonMode()){
            player->Buttons[ws_pkt.payload[1]].setButton(false);
        }
        return ESP_OK;
    }


    //Join Befehl
    if(ws_pkt.payload[0] == 2 && lobby->getState() == LobbyState::lobby_open){
        lobby->addPlayer(req);
        return ESP_OK;
    }

    //Lobby erstellen Befehl
    if(ws_pkt.payload[0] == 1 && lobby->getState() == LobbyState::lobby_closed){
        lobby->setState(LobbyState::lobby_open);
        lobby->setGameID(ws_pkt.payload[1]);
        lobby->setMaxPlayer(ws_pkt.payload[2]);
        lobby->setGameData(ws_pkt.payload[3]);
        lobby->addPlayer(req);
        return ESP_OK;
    }
    httpd_ws_frame_t ws_frame;
    uint8_t buff[1];
    ws_frame.payload = buff;
    ws_frame.len = strlen((char*)buff);
    ws_frame.type = HTTPD_WS_TYPE_TEXT;
    buff[0] = 0;
    httpd_ws_send_frame(req, &ws_frame);
    return ESP_OK;
}

// Handler für Statusabfrage von Lobby und Spielern
esp_err_t Data_response(httpd_req_t * req){
    Lobby * lobby = (Lobby *)req->user_ctx;
    char buff[6];
    buff[0] = (uint8_t)(lobby->getState());
    buff[1] = lobby->getGameID();
    buff[2] = (uint8_t)(lobby->getPlayer(0)->getconnected());
    buff[3] = (uint8_t)(lobby->getPlayer(1)->getconnected());
    buff[4] = (uint8_t)(lobby->getPlayer(2)->getconnected());
    buff[5] = (uint8_t)(lobby->getPlayer(3)->getconnected());
    httpd_resp_send(req, buff, 6);
    return ESP_OK;
}

httpd_handle_t start_webserver(const char * base_path, Lobby * lobby){
    //muss nicht frei gemcht werden weil webserver nie stoppt
    user_ctx * ctx = new user_ctx;
    ctx->base_path = new char[strlen(base_path) + 1];
    strcpy(ctx->base_path, base_path);
    httpd_uri_t uri_get = {
        .uri      = "/Webapp/*",
        .method   = HTTP_GET,
        .handler  = get_handler,
        .user_ctx = ctx,
        .is_websocket = false,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };
    httpd_uri_t Data = {
        .uri      = "/Data",
        .method   = HTTP_GET,
        .handler  = Data_response,
        .user_ctx = lobby,
        .is_websocket = false,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };
    httpd_uri_t ws = {
        .uri      = "/ws",
        .method   = HTTP_GET,
        .handler  = Websocket,
        .user_ctx = lobby,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };
    //Webserver Einstellungen
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.backlog_conn = 7;
    config.max_open_sockets = 10;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.core_id = 0;
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK){
        lobby->setServerHandle(server);
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &ws);
        httpd_register_uri_handler(server, &Data);
        printf("complete server\n");
    }
    else{
        printf("Server Error\n");
    }
    return server;
}
