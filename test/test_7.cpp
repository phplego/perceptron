#include <thread>
#include <iostream>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

#include "colored_output.h"
#include "websocket.h"
#include "httpserver.h"

#define WS_PORT     8666
#define HTTP_PORT   8777

std::string get_process_memory(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    char line[128] = {0};

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            line[strlen(line) - 1] = 0;
            return line;
        }
    }
    fclose(file);
    return "N/A";
}

class SocketServer
{
public:
    struct CMDConnData
    {
        bool login;
    };
    using WSServer = websocket::WSServer<SocketServer, CMDConnData>;
    using WSConn = WSServer::Connection;

    void init()
    {
        if (!wsserver.init("0.0.0.0", WS_PORT))
        {
            std::cout << _RED "SocketServer init failed: " << wsserver.getLastError() << _RST << std::endl;
            return;
        }
    }

    void handle()
    {
        wsserver.poll(this);
    }

    // called when a new websocket connection is about to open
    // optional: origin, protocol, extensions will be nullptr if not exist in the request headers
    // optional: fill resp_protocol[resp_protocol_size] to add protocol to response headers
    // optional: fill resp_extensions[resp_extensions_size] to add extensions to response headers
    // return true if accept this new connection
    bool onWSConnect(WSConn &conn, const char *request_uri, const char *host, const char *origin, const char *protocol,
                     const char *extensions, char *resp_protocol, uint32_t resp_protocol_size, char *resp_extensions,
                     uint32_t resp_extensions_size)
    {
        struct sockaddr_in addr;
        conn.getPeername(addr);
        std::cout << "Receive WS connection from: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
        std::cout << "request_uri: " << request_uri << std::endl;
        std::cout << "host: " << host << std::endl;
        if (origin){
            std::cout << "origin: " << origin << std::endl;
        }
        if (protocol){
            std::cout << "protocol: " << protocol << std::endl;
        }
        if (extensions){
            std::cout << "extensions: " << extensions << std::endl;
        }
        std::cout << std::endl;
        return true;
    }

    // called when a websocket connection is closed
    // status_code 1005 means no status code in the close msg
    // status_code 1006 means not a clean close(tcp connection closed without a close msg)
    void onWSClose(WSConn &conn, uint16_t status_code, const char *reason)
    {
        std::cout << "ws close, status_code: " << status_code << ", reason: " << reason << std::endl;
    }

    // onWSMsg is used if RecvSegment == false(by default), called when a whole msg is received
    void onWSMsg(WSConn &conn, uint8_t opcode, const uint8_t *payload, uint32_t pl_len)
    {
        if (opcode == websocket::OPCODE_PING)
        {
            conn.send(websocket::OPCODE_PONG, payload, pl_len);
            return;
        }
        if (opcode != websocket::OPCODE_TEXT)
        {
            conn.close(1003, "not text msg");
            return;
        }
        const char *data = (const char *)payload;
        char buf[1000] = {0};
        strncpy(buf, data, pl_len);
        pf_yellow("<<< MSG: '%s' len: %zd \n", buf, strlen(buf));


        const char * cmd = "cn ";
        if(strncasecmp(cmd, buf, strlen(cmd)) == 0){
            char arg_buf [30];
            strcpy(arg_buf, buf + strlen(cmd));

            reply(conn, std::string() + "creating network: " + arg_buf);
            return;
        }

        cmd = "reset";
        if(strncasecmp(cmd, buf, strlen(cmd)) == 0){
            reply(conn, "resetting..");
            return;
        }
        cmd = "mem";
        if(strncasecmp(cmd, buf, strlen(cmd)) == 0){
            reply(conn, get_process_memory());
            return;
        }

        reply(conn, "default response");
    }

    // onWSSegment is used if RecvSegment == true, called when a segment is received
    // pl_start_idx: index in the whole msg for the 1st byte of payload
    // fin: whether it's the last segment
    void onWSSegment(WSConn &conn, uint8_t opcode, const uint8_t *payload, uint32_t pl_len, uint32_t pl_start_idx, bool fin)
    {
        std::cout << "error: onWSSegment should not be called" << std::endl;
    }

    void reply(WSConn &conn, std::string str)
    {
        conn.send(websocket::OPCODE_TEXT, (const uint8_t *)str.data(), str.length());
        pf_blue(">>> RPL: '%s' len: %zd \n", str.data(), str.length());
    }

private:
    WSServer wsserver;
};

SocketServer socketServer;
httpserver::HttpServer httpServer(HTTP_PORT);
bool socketServerRunning = true;
bool httpServerRunning = true;

void my_handler(int s)
{
    pf("sa_handler %d\n", s);
    socketServerRunning = false;
    httpServerRunning = false;
}

void webservertheread()
{
    pf_green("Web Server thread started.. http://127.0.0.1:%d\n", HTTP_PORT);

    httpServer.setCallback([](std::string method, std::string path){
        pf_blue("method = %s\n", method.data());
        if(path == "/"){
            std::ifstream file("test_7.html");
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        
        return std::string() + httpserver::HTTP_HEADER_404 + "404 Page not found hehe. Path=" + path;
    });

    while (httpServerRunning)
    {
        httpServer.handle();
        usleep(1000*10);
    }
    printf("Web Server thread exit.\n");
}

int main(int argc, char **argv)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    socketServer.init();

    std::thread thread1(webservertheread);
    thread1.detach();

    pf_green("WebSocket Server started.. ws://127.0.0.1:%d\n", WS_PORT);
    while (socketServerRunning)
    {
        socketServer.handle();
        usleep(1000*10);
    }

    std::cout << "Server stopped." << std::endl;
}