# 🏫 Sistema Escolar - Servidor de Archivos Estáticos

Servidor web programado en C++ capaz de entregar automáticamente cualquier archivo (HTML, CSS, JS, imágenes) sin necesidad de modificar el código fuente.

![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=flat&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=flat&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=flat&logo=javascript&logoColor=black)
![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![Sockets](https://img.shields.io/badge/TCP-Sockets-blueviolet)
![Status](https://img.shields.io/badge/status-completado-success)

---

## 📌 Descripción

El servidor ha evolucionado de rutas estáticas a un **servidor de archivos estáticos genérico**. Ahora, cualquier archivo que coloques dentro de la carpeta `Public/` será accesible de inmediato desde el navegador.

- **No requiere recompilar** al añadir archivos.
- **Detecta automáticamente** el tipo MIME (CSS, JS, imágenes, fuentes, etc.) para que el navegador los interprete correctamente.
- Incluye **seguridad básica** contra Directory Traversal (bloqueo de `..`).

---

## ✨ Características

- 🌐 **Sockets TCP nativos:** Conexiones sin librerías externas (Winsock / POSIX).
- 📂 **Mapeo automático:** `URL /ruta` → `disco Public/ruta`.
- 🏷️ **Tipos MIME:** Tabla extensa que asigna el `Content-Type` correcto según la extensión.
- 🛡️ **Seguridad:** Rechazo de rutas maliciosas (evita leer archivos fuera de `Public/`).
- 🚫 **404 Personalizado:** Página de error amigable en HTML.
- 💻 **Multiplataforma:** Código preparado con `#ifdef` para Windows y Linux/macOS.

---

## 🛠️ Tecnologías

| Tecnología | Uso |
|------------|-----|
| C++17 | Lenguaje del servidor y manejo de archivos (`<filesystem>`) |
| Sockets (TCP) | Comunicación por red |
| Protocolo HTTP/1.1 | Interpretación de peticiones y respuestas |
| HTML5 / CSS3 / JS | Frontend del sistema escolar |

---

## 📁 Estructura del proyecto

```text
Sistema-Escolar/
├── servidor.cpp       <-- Código del servidor web en C++
├── README.md
└── Public/            <-- Carpeta pública (servida automáticamente)
    ├── index.html
    ├── styless.css
    ├── app.js
    ├── favicon.ico    <-- (Opcional) Icono de la pestaña
    └── img/
        ├── mi.png
        └── fondo.gif
```

---

## 🚀 Instalación y ejecución

### 🐧 Linux / macOS
1. Abre la terminal en la carpeta raíz del proyecto.
2. Compila el servidor (requiere C++17):
   ```bash
   g++ servidor.cpp -o servidor -std=c++17
   ```
3. Ejecuta el servidor:
   ```bash
   ./servidor
   ```

### 🪟 Windows (MinGW / VS Code)
1. Abre la terminal (PowerShell o CMD) en la carpeta raíz.
2. Compila el servidor (importante el `-std=c++17` y `-lws2_32`):
   ```bash
   g++ servidor.cpp -o servidor.exe -std=c++17 -lws2_32
   ```
3. Ejecuta el servidor:
   ```cmd
   .\servidor.exe
   ```

### 🌐 Abrir en el navegador
Una vez que la terminal muestre `Servidor escuchando en http://localhost:8080`, abre tu navegador y visita:
```text
http://localhost:8080
```

---

## 🧠 Conceptos aplicados

- **Rutas URL:** Normalización de `/` a `/index.html` y eliminación de Query Strings (`?`) y fragmentos (`#`).
- **Mapeo Disco-URL:** Combinación de `Public/` con la ruta solicitada.
- **MIME Types:** Diccionario (usando `std::map`) para asociar extensiones con tipos de contenido (`text/html`, `image/png`, `application/javascript`, etc.).
- **`std::filesystem`:** Verificación de existencia de archivos (`fs::exists`) y tipo de archivo (`fs::is_regular_file`).
- **I/O Binario:** Lectura completa de archivos con `ifstream` y `istreambuf_iterator` para no dañar imágenes.
- **Seguridad:** Validación de rutas para evitar Directory Traversal.

---

## 🧪 Cómo probarlo

1. Ejecuta el servidor.
2. Entra a `http://localhost:8080/` en tu navegador.
3. **La prueba de fuego:** 
   - Crea un archivo llamado `prueba.html` dentro de `Public/`.
   - En el navegador, entra a `http://localhost:8080/prueba.html`.
   - Debería cargar al instante, **sin reiniciar el servidor**.
4. Abre las herramientas de desarrollador del navegador (F12) → pestaña **Network** (Red). Verifica que las peticiones de `.css`, `.js` y `.gif` devuelvan un estado `200` y tengan el `Content-Type` correcto.
5. Prueba la seguridad entrando a `http://localhost:8080/../servidor.cpp`. Debería devolver un error `400 Bad Request`.

---

## ⚠️ Notas importantes

- El servidor debe ejecutarse desde la **carpeta raíz** del proyecto.
- Asegúrate de compilar con `-std=c++17` para habilitar `std::filesystem`.
- Si el puerto `8080` está ocupado, puedes cambiarlo en la línea `const int PUERTO = 8080;`.

---

## 📄 Licencia

MIT License
