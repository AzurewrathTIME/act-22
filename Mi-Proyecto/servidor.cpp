#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using socket_t = SOCKET;
  #define CLOSE_SOCKET closesocket
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  using socket_t = int;
  #define CLOSE_SOCKET close
  #define INVALID_SOCKET (-1)
#endif

using namespace std;
namespace fs = std::filesystem;

const int PUERTO = 8080;
const string CARPETA_PUBLICA = "Public";

string leerArchivo(const string& ruta) {
    ifstream archivo(ruta, ios::binary);
    if (!archivo.is_open()) return "";
    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    return contenido;
}

string obtenerMIME(const string& ruta) {
    size_t punto = ruta.find_last_of('.');
    if (punto == string::npos) return "application/octet-stream";

    string ext = ruta.substr(punto);

    static const map<string, string> mimes = {
        {".html", "text/html; charset=utf-8"},
        {".htm",  "text/html; charset=utf-8"},
        {".css",  "text/css; charset=utf-8"},
        {".js",   "application/javascript; charset=utf-8"},
        {".json", "application/json; charset=utf-8"},
        {".txt",  "text/plain; charset=utf-8"},
        {".xml",  "application/xml; charset=utf-8"},
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif",  "image/gif"},
        {".svg",  "image/svg+xml"},
        {".webp", "image/webp"},
        {".ico",  "image/x-icon"},
        {".woff", "font/woff"},
        {".woff2","font/woff2"},
        {".ttf",  "font/ttf"},
        {".pdf",  "application/pdf"},
        {".zip",  "application/zip"},
        {".mp3",  "audio/mpeg"},
        {".mp4",  "video/mp4"}
    };

    auto it = mimes.find(ext);
    return (it != mimes.end()) ? it->second : "application/octet-stream";
}

string construirRespuesta(int codigo, const string& mime, const string& cuerpo) {
    string estado;
    if (codigo == 200) estado = "200 OK";
    else if (codigo == 400) estado = "400 Bad Request";
    else if (codigo == 403) estado = "403 Forbidden";
    else if (codigo == 404) estado = "404 Not Found";
    else if (codigo == 405) estado = "405 Method Not Allowed";
    else estado = "500 Internal Server Error";

    return "HTTP/1.1 " + estado + "\r\n"
         + "Content-Type: " + mime + "\r\n"
         + "Content-Length: " + to_string(cuerpo.size()) + "\r\n"
         + "Connection: close\r\n"
         + "\r\n"
         + cuerpo;
}

string extraerRuta(const string& peticion) {
    size_t finLinea = peticion.find("\r\n");
    string primera = (finLinea != string::npos) ? peticion.substr(0, finLinea) : peticion;

    size_t p1 = primera.find(' ');
    if (p1 == string::npos) return "/";

    size_t p2 = primera.find(' ', p1 + 1);
    if (p2 == string::npos) return "/";

    return primera.substr(p1 + 1, p2 - p1 - 1);
}

string normalizarRuta(string ruta) {
    size_t q = ruta.find('?');
    if (q != string::npos) ruta = ruta.substr(0, q);

    size_t h = ruta.find('#');
    if (h != string::npos) ruta = ruta.substr(0, h);

    if (ruta == "/" || ruta.empty()) ruta = "/index.html";

    return ruta;
}

int main() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Error al inicializar Winsock" << endl;
        return 1;
    }
#endif

    socket_t servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor == INVALID_SOCKET) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }

    sockaddr_in direccion{};
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PUERTO);

    if (bind(servidor, (sockaddr*)&direccion, sizeof(direccion)) != 0) {
        cerr << "Error en bind (puerto ocupado)" << endl;
        CLOSE_SOCKET(servidor);
        return 1;
    }

    if (listen(servidor, 10) != 0) {
        cerr << "Error en listen" << endl;
        CLOSE_SOCKET(servidor);
        return 1;
    }

    cout << "Servidor escuchando en http://localhost:" << PUERTO << endl;

    while (true) {
        sockaddr_in clienteDir{};
#ifdef _WIN32
        int tam = sizeof(clienteDir);
#else
        socklen_t tam = sizeof(clienteDir);
#endif

        socket_t cliente = accept(servidor, (sockaddr*)&clienteDir, &tam);
        if (cliente == INVALID_SOCKET) continue;

        char buffer[4096] = {0};
        int bytes = recv(cliente, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            CLOSE_SOCKET(cliente);
            continue;
        }
        string peticion(buffer, bytes);

        string ruta = extraerRuta(peticion);
        ruta = normalizarRuta(ruta);

        cout << "[REQ] " << ruta;

        if (ruta.find("..") != string::npos) {
            string cuerpo = "Ruta no permitida";
            string resp = construirRespuesta(400, "text/plain; charset=utf-8", cuerpo);
            send(cliente, resp.c_str(), resp.size(), 0);
            cout << " -> [400] Bad Request" << endl;
            CLOSE_SOCKET(cliente);
            continue;
        }

        string rutaDisco = CARPETA_PUBLICA + ruta;

        if (!fs::exists(rutaDisco) || !fs::is_regular_file(rutaDisco)) {
            string cuerpo = "<h1>404 Not Found</h1><p>Recurso no encontrado</p>";
            string resp = construirRespuesta(404, "text/html; charset=utf-8", cuerpo);
            send(cliente, resp.c_str(), resp.size(), 0);
            cout << " -> [404] Not Found" << endl;
            CLOSE_SOCKET(cliente);
            continue;
        }

        string contenido = leerArchivo(rutaDisco);
        string mime = obtenerMIME(rutaDisco);

        string resp = construirRespuesta(200, mime, contenido);
        send(cliente, resp.c_str(), resp.size(), 0);
        
        cout << " -> [200] OK (" << mime << ")" << endl;
        CLOSE_SOCKET(cliente);
    }

    CLOSE_SOCKET(servidor);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}